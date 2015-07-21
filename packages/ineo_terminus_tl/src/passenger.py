#!/usr/bin/python2

import datetime
import re
import HTMLParser
import synthese

try:
  from lxml import etree
except ImportError:
  print("la lib lxml n'est pas disponible")


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
  # TODO : change
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


networkId=network_id
dataSourceId=datasource_id

# Request headers
root = etree.Element("Passenger" + type + "MessageRequest")
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

# Tts (text)
if int(needs_play_tts) != 0:
  if int(message[0]["play_tts"]) != 0:
    childTts = etree.SubElement(childMessaging, "Tts")
    childTts.text = message[0]["content"]

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
  childRepeatPeriod = etree.SubElement(childMessaging, "RepeatPeriod")
  childRepeatPeriod.text = str(int(message[0]["repeat_interval"]) / 60)

# Inhibition
childInhibition = etree.SubElement(childMessaging, "Inhibition")
childInhibition.text = "non"

# Color
childColor = etree.SubElement(childMessaging, "Color")
# TODO : change
ineoColor = "Vert"
# Search for 'color' attributes in the message
match = re.search("(?<=color: #)[0-9a-fA-F]{6}", message[0]["content"])
if match:
  # Convert the RGB of the first 'color' attribute into the Ineo enumerate
  ineoColor = convert_to_ineo_color(match.group(0))
childColor.text = ineoColor

# Tts (boolean)
if int(needs_play_tts) != 0:
  childTts = etree.SubElement(childMessaging, "Tts")
  childTts.text = "oui" if int(message[0]["play_tts"]) != 0 else "non"

# Jingle

# {Start,End}StopPoint
if needs_start_stop_point:
  startStopPointId = int(message[0]["start_stop_point"])
  if startStopPointId > 0:
    childStartStopPoint = etree.SubElement(childMessaging, "StartStopPoint")
    childStartStopPoint.text = message[0]["start_stop_point"]
if needs_end_stop_point:
  endStopPointId = int(message[0]["end_stop_point"])
  if endStopPointId > 0:
    childEndStopPoint = etree.SubElement(childMessaging, "EndStopPoint")
    childEndStopPoint.text = message[0]["end_stop_point"]

# Chaining

# Priority

# Text
# Split text around <br /> and \n
childText = etree.SubElement(childMessaging, "Text")
for line in re.split('<br />|\n',message[0]["content"]):
  h = HTMLParser.HTMLParser()
  childLine = etree.SubElement(childText, "Line")
  childLine.text = h.unescape(line)

# Recipients
childRecipients = etree.SubElement(childMessaging, "Recipients")
recipients = message[0]["recipients"][0]
hasAllNetwork = False
if 'line' in recipients:
  # Scan the 'line' recipients to check if the whole transport network is selected
  for line in recipients["line"]:
    hasAllNetwork = hasAllNetwork or (line["id"] == networkId)
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
        if lineCode[0] == dataSourceId:
          childLine = etree.SubElement(childLines, "Line")
          childLine.text = lineCode[1]

# Print resulting XML to output stream
print(etree.tostring(root, pretty_print=True, xml_declaration=True, encoding="iso-8859-1"))