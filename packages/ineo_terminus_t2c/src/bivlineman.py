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
    data_with_br = data.replace("\n", "<br/>")
    HTMLParser.feed(self, data_with_br)
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


# Request headers
namespace = "http://www.w3.org/2001/XMLSchema-instance"
locationAttribute = "{%s}noNameSpaceSchemaLocation" % namespace
root = etree.Element("BivLineMan" + type + "MessageRequest", attrib={locationAttribute: xsd_location} if len(xsd_location) > 0 else {})
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
messageTitle = unicode(message[0]["title"], "utf-8", "ignore")
childName.text = u"{:04d} {:.27s}".format(messageID, messageTitle)

# Tts
if int(needs_play_tts) != 0:
  if int(message[0]["play_tts"]) != 0:
    childTts = etree.SubElement(childMessaging, "Tts")
    # Extract HTML text lines 
    htmlParser = HTMLTextExtractor()
    unicode_message_text = unicode(message_text, "utf-8", "ignore")
    htmlParser.feed(unicode_message_text)
    # 'Tts' node accepts [1..6] lines * [0..25] characters
    lines = htmlParser.wrap_lines(6, 25)
    for line in lines:
      childLine = etree.SubElement(childTts, "Line")
      childLine.text = line

# {Start,Stop}{Date,Time}
childStartDate = etree.SubElement(childMessaging, "StartDate")
childStartDate.text = "01/01/1970"
childStopDate = etree.SubElement(childMessaging, "StopDate")
childStopDate.text = "31/12/2037"
childStartTime = etree.SubElement(childMessaging, "StartTime")
childStartTime.text = "03:00:00"
childStopTime = etree.SubElement(childMessaging, "StopTime")
childStopTime.text = "27:00:00"

# Alternance
childAlternance = etree.SubElement(childMessaging, "Alternance")
childAlternance.text = "oui" if int(message[0]["priority"]) < 50 else "non"

# Text
# Extract HTML text lines 
childText = etree.SubElement(childMessaging, "Text")
htmlParser = HTMLTextExtractor()
unicode_message_text = unicode(message_text, "utf-8", "ignore")
htmlParser.feed(unicode_message_text)
# 'Text' node accepts 1 line * [0..20] characters
lines = htmlParser.wrap_lines(1, 20)
for line in lines:
  childLine = etree.SubElement(childText, "Line")
  childLine.text = line

# Recipients
childRecipients = etree.SubElement(childMessaging, "Recipients")
recipients = message[0]["recipients"][0]
if 'line' in recipients:
  lineId = int(recipients["line"][0]["id"])
  lineTableId = (lineId / (2**48))
  if lineTableId == 42:
    parameters = { "roid": lineId }
    linePM = synthese.service("LinesListFunction2", parameters)
    lineCodesStr = linePM["line"][0]["creator_id"]
    lineCodes = map(lambda x: x.split('|'), lineCodesStr.split(','))
    for lineCode in lineCodes:
      if lineCode[0] == datasource_id:
        childLine = etree.SubElement(childRecipients, "Line")
        childLine.text = lineCode[1]

# Print resulting XML to output stream
print(etree.tostring(root, pretty_print=True, xml_declaration=True, encoding="iso-8859-1"))
