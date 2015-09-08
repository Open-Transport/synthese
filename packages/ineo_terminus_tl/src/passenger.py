#!/usr/bin/python2

import datetime
import re
from HTMLParser import HTMLParser
import synthese

try:
  from lxml import etree
except ImportError:
  print("la lib lxml n'est pas disponible")


# Custom subclass of HTMLParser that extracts the lines of text from a HTML document
class HTMLTextExtractor(HTMLParser):
  def __init__(self):
    from HTMLParser import HTMLParser
    HTMLParser.__init__(self)
    self.lines = []
    self.current_line = ''
    self.after_entity = False

  def handle_starttag(self, tag, attrs):
    # If tag is a <br/>, append current line and start new line
    if tag == 'br':
      self.lines.append(self.current_line)
      self.current_line = ''
      self.after_entity = False

  def handle_data(self, data):
    # Concatenate data to current line
    self.current_line += data if len(self.current_line) == 0 else (('' if self.after_entity else ' ') + data)
    self.after_entity = False

  def handle_entityref(self, data):
    # Decode entity and concatenate it to current line
    from htmlentitydefs import name2codepoint
    character = ' ' if data == 'nbsp' else unichr(name2codepoint[data])
    self.current_line = self.current_line + character
    self.after_entity = True

  def feed(self, data):
    from HTMLParser import HTMLParser
    HTMLParser.feed(self, data)
    if len(self.current_line) > 0:
      self.lines.append(self.current_line)
      self.current_line = ''

  def get_lines(self):
    return self.lines

  def wrap_lines(self, max_lines_count, max_line_size):
    split_lines = []
    merged_lines = []

    # Break lines to match max line size
    for line in self.lines:
      if len(line) == 0:
        split_lines.append(line)
      else:
        start_index = 0
        end_index = max_line_size
        while start_index < len(line):
          split_line = line[start_index:end_index]
          split_lines.append(split_line)
          start_index += max_line_size
          end_index += max_line_size

    # If there are too many lines, first remove empty lines
    if len(split_lines) > max_lines_count:
      split_lines[:] = [line for line in split_lines if len(line.strip()) > 0]

    # If there are still too many lines, try to concatenate them up to max_line_size
    if len(split_lines) <= max_lines_count:
      merged_lines = split_lines
    else:
      merged_line = ''
      for split_line in split_lines:
        nb_chars = max_line_size - len(merged_line)
        if len(merged_line) > 0:
          nb_chars = nb_chars - 1
          merged_line = merged_line + ' ' + split_line[0:nb_chars]
        else:
          merged_line = split_line[0:nb_chars]
        if len(merged_line) == max_line_size:
          merged_lines.append(merged_line)
          merged_line = split_line[nb_chars:]
      if len(merged_line) > 0:
        merged_lines.append(merged_line)

    return merged_lines


def convert_to_ineo_color( hexcolor ):
  import colorsys

  # Convert hexa integer into decimal integer
  color  = int(hexcolor, 16)  
  # Extract RGB values
  blue   = (color % 256)
  green  = (color / 256) % 256
  red    = (color / (256*256))
  # Normalize them
  bluef  = float(blue)  / 256.0
  greenf = float(green) / 256.0
  redf   = float(red)   / 256.0

  # Convert to HSV
  hsv = colorsys.rgb_to_hsv(redf, greenf, bluef)
  hue = hsv[0] * 360

  # Default value is 'Vert'
  colorStr = 'Vert'
  
  # Hue E ]10..45]  => 'Orange'
  if hue > 10.0 and hue <= 45.0:
    colorStr = 'Orange'
  # Hue E [60..170] => 'Vert'
  if hue >= 60.0 and hue <= 170.0:
    colorStr = 'Vert'
  # Hue E [0..10] or [290..360] => 'Rouge'
  if hue <= 10.0 or hue >= 290.0:
    colorStr = 'Rouge'

  return colorStr


# Request headers
namespace = "http://www.w3.org/2001/XMLSchema-instance"
locationAttribute = "{%s}noNameSpaceSchemaLocation" % namespace
root = etree.Element("Passenger" + type + "MessageRequest", attrib={locationAttribute: xsd_location} if len(xsd_location) > 0 else {})
childID = etree.SubElement(root, "ID")
childID.text = ID
childRequestTimeStamp = etree.SubElement(root, "RequestTimeStamp")
now = datetime.datetime.now()
childRequestTimeStamp.text = now.strftime("%d/%m/%Y %X")
childRequestorRef = etree.SubElement(root, "RequestorRef")
childRequestorRef.text = "Terminus"
childMessaging = etree.SubElement(root, "Messaging")

# Name
# Ineo SAE requires the name to be defined as ‘XXXX aaaaaaaaaaaaaaaaaaaaaaaaaaa’ where 'XXXX' is a unique message id
childName = etree.SubElement(childMessaging, "Name")
messageID = int(message[0]["message_id"]) % 10000
childName.text = "{:04d} {:.27s}".format(messageID, message[0]["title"])

# Dispatching
childDispatching = etree.SubElement(childMessaging, "Dispatching")
childDispatching.text = "Repete"

# {Start,Stop}{Date,Time}
childStartDate = etree.SubElement(childMessaging, "StartDate")
childStartDate.text = "01/01/1970"
childStopDate = etree.SubElement(childMessaging, "StopDate")
childStopDate.text = "31/12/2037"
childStartTime = etree.SubElement(childMessaging, "StartTime")
childStartTime.text = "00:00:00"
childStopTime = etree.SubElement(childMessaging, "StopTime")
childStopTime.text = "23:59:00"

# NumberShow

# RepeatPeriod
if int(needs_repeat_interval) != 0:
  repeatPeriod = int(message[0]["repeat_interval"]) / 60
  if repeatPeriod == 0:
    repeatPeriod = 1
  childRepeatPeriod = etree.SubElement(childMessaging, "RepeatPeriod")
  childRepeatPeriod.text = str(repeatPeriod)

# Inhibition
childInhibition = etree.SubElement(childMessaging, "Inhibition")
childInhibition.text = "non"

# Color
childColor = etree.SubElement(childMessaging, "Color")
ineoColor = "Vert"
# Search for 'color' attributes in the message
match = re.search("(?<=color: #)[0-9a-fA-F]{6}", message[0]["content"])
if match:
  # Convert the RGB of the first 'color' attribute into the Ineo enumerate
  ineoColor = convert_to_ineo_color(match.group(0))
childColor.text = ineoColor

# TtsBroadcasting
if int(needs_play_tts) != 0:
  childTts = etree.SubElement(childMessaging, "TtsBroadcasting")
  childTts.text = "oui" if int(message[0]["play_tts"]) != 0 else "non"

# Jingle

# {Start,End}StopPoint
if int(needs_start_stop_point) != 0:
  startStopPointId = int(message[0]["start_stop_point"])
  if startStopPointId > 0:
    # Convert this SYNTHESE stop point id into an Ineo stop point id
    parameters = { "roid": startStopPointId }
    stopPointPM = synthese.service("object", parameters)
    if 'operator_code' in stopPointPM:
      # Split the operator codes and find the Ineo code
      stopCodes = map(lambda x: x.split('|'), stopPointPM["operator_code"].split(','))
      for stopCode in stopCodes:
        if stopCode[0] == datasource_id:
          childStartStopPoint = etree.SubElement(childMessaging, "StartStopPoint")
          # Ineo stop point ids may start with "MNLP_**_", but this prefix must not be sent to Ineo
          childStartStopPoint.text = (stopCode[1] if stopCode[1].startswith(ineo_stop_point_prefix) == False else stopCode[1][len(ineo_stop_point_prefix):])

if int(needs_end_stop_point) != 0:
  endStopPointId = int(message[0]["end_stop_point"])
  if endStopPointId > 0:
    # Convert this SYNTHESE stop point id into an Ineo stop point id
    parameters = { "roid": endStopPointId }
    stopPointPM = synthese.service("object", parameters)
    if 'operator_code' in stopPointPM:
      # Split the operator codes and find the Ineo code
      stopCodes = map(lambda x: x.split('|'), stopPointPM["operator_code"].split(','))
      for stopCode in stopCodes:
        if stopCode[0] == datasource_id:
          childEndStopPoint = etree.SubElement(childMessaging, "EndStopPoint")
          # Ineo stop point ids may start with "MNLP_**_", but this prefix must not be sent to Ineo
          childEndStopPoint.text = (stopCode[1] if stopCode[1].startswith(ineo_stop_point_prefix) == False else stopCode[1][len(ineo_stop_point_prefix):])

# Chaining

# Priority

# Text
# Extract HTML text lines 
childText = etree.SubElement(childMessaging, "Text")
htmlParser = HTMLTextExtractor()
htmlParser.feed(message_text)
# 'Text' node accepts [1..4] lines * [0..25] characters
lines = htmlParser.wrap_lines(4, 25)
for line in lines:
  childLine = etree.SubElement(childText, "Line")
  childLine.text = line

# Tts
if int(needs_play_tts) != 0:
  if int(message[0]["play_tts"]) != 0:
    childTts = etree.SubElement(childMessaging, "Tts")
    htmlParser = HTMLTextExtractor()
    htmlParser.feed(message_text)
    # 'Text' node accepts 1 line * [0..300] characters
    lines = htmlParser.wrap_lines(1, 300)
    for line in lines:
      childLine = etree.SubElement(childText, "Line")
      childLine.text = line

# Recipients
childRecipients = etree.SubElement(childMessaging, "Recipients")
recipients = message[0]["recipients"][0]
hasAllNetwork = False
if 'line' in recipients:
  # Scan the 'line' recipients to check if the whole transport network is selected
  for line in recipients["line"]:
    hasAllNetwork = hasAllNetwork or (line["id"] == network_id)
  # If it is, use 'AllNetwork' tag
  if hasAllNetwork:
    childAllNetwork = etree.SubElement(childRecipients, "AllNetwork")
  # Else add the Ineo code of each commercial line in the recipients
  else:
    childLines = etree.SubElement(childRecipients, "Lines")
    for line in recipients["line"]:
      parameters = { "roid": line["id"] }
      linePM = synthese.service("LinesListFunction2", parameters)
      lineCodesStr = linePM["line"][0]["creator_id"]
      lineCodes = map(lambda x: x.split('|'), lineCodesStr.split(','))
      for lineCode in lineCodes:
        if lineCode[0] == datasource_id:
          childLine = etree.SubElement(childLines, "Line")
          childLine.text = lineCode[1]

# Print resulting XML to output stream
print(etree.tostring(root, pretty_print=True, xml_declaration=True, encoding="iso-8859-1"))
