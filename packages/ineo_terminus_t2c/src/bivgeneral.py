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


# Request headers
namespace = "http://www.w3.org/2001/XMLSchema-instance"
locationAttribute = "{%s}noNameSpaceSchemaLocation" % namespace
root = etree.Element("BivGeneral" + type + "MessageRequest", attrib={locationAttribute: xsd_location} if len(xsd_location) > 0 else {})
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

# {Start,Stop}{Date,Time}
childStartDate = etree.SubElement(childMessaging, "StartDate")
childStartDate.text = "01/01/1970"
childStopDate = etree.SubElement(childMessaging, "StopDate")
childStopDate.text = "31/12/2037"

# Varying
childVarying = etree.SubElement(childMessaging, "Varying")
childVarying.text = "oui" if int(message[0]["priority"]) < 99 else "non"

# Duration
childDuration = etree.SubElement(childMessaging, "Duration")
childDuration.text = message[0]["display_duration"]

# DiodFlashing


# Text
# Extract HTML text lines 
htmlParser = HTMLTextExtractor()
unicode_message_text = unicode(message_text, "utf-8", "ignore")
htmlParser.feed(unicode_message_text)

# TypeBIV = BUS4L
childText1 = etree.SubElement(childMessaging, "Text")
childTypeBIV1 = etree.SubElement(childText1, "TypeBIV")
childTypeBIV1.text = "BUS4L"
childFixedText1 = etree.SubElement(childText1, "FixedText")
# BUS4L accepts 4 lines * [0..24] characters (must pad with blank lines)
contentLines = htmlParser.wrap_lines(4, 24)
blankLinesNb = 4 - len(contentLines)
for contentLine in contentLines:
  childLine1 = etree.SubElement(childFixedText1, "Line")
  childLine1.text = contentLine
# Blank lines for padding
for i in range(0, blankLinesNb):
  childLine1 = etree.SubElement(childFixedText1, "Line")

# TypeBIV = BUS8L
childText2 = etree.SubElement(childMessaging, "Text")
childTypeBIV2 = etree.SubElement(childText2, "TypeBIV")
childTypeBIV2.text = "BUS8L"
childFixedText2 = etree.SubElement(childText2, "FixedText")
# BUS8L accepts 8 lines * [0..24] characters (must pad with blank lines)
contentLines = htmlParser.wrap_lines(8, 24)
blankLinesNb = 8 - len(contentLines)
for contentLine in contentLines:
  childLine2 = etree.SubElement(childFixedText2, "Line")
  childLine2.text = contentLine
# Blank lines for padding
for i in range(0, blankLinesNb):
  childLine2 = etree.SubElement(childFixedText2, "Line")

# TypeBIV = ER
childText3 = etree.SubElement(childMessaging, "Text")
childTypeBIV3 = etree.SubElement(childText3, "TypeBIV")
childTypeBIV3.text = "ER"
childFixedText3 = etree.SubElement(childText3, "FixedText")
# ER accepts 8 lines * [0..30] characters (must pad with blank lines)
contentLines = htmlParser.wrap_lines(8, 30)
blankLinesNb = 8 - len(contentLines)
for contentLine in contentLines:
  childLine3 = etree.SubElement(childFixedText3, "Line")
  childLine3.text = contentLine
# Blank lines for padding
for i in range(0, blankLinesNb):
  childLine3 = etree.SubElement(childFixedText3, "Line")

# TypeBIV = TFT
childText4 = etree.SubElement(childMessaging, "Text")
childTypeBIV4 = etree.SubElement(childText4, "TypeBIV")
childTypeBIV4.text = "TFT"
childFixedText4 = etree.SubElement(childText4, "FixedText")
# TFT accepts 6 lines * [0..40] characters (must pad with blank lines)
contentLines = htmlParser.wrap_lines(6, 40)
blankLinesNb = 6 - len(contentLines)
for contentLine in contentLines:
  childLine4 = etree.SubElement(childFixedText4, "Line")
  childLine4.text = contentLine
# Blank lines for padding
for i in range(0, blankLinesNb):
  childLine4 = etree.SubElement(childFixedText4, "Line")

# TypeBIV = TTS
childText5 = etree.SubElement(childMessaging, "Text")
childTypeBIV5 = etree.SubElement(childText5, "TypeBIV")
childTypeBIV5.text = "TTS"
childFixedText5 = etree.SubElement(childText5, "TtsText")
# TTS accepts 1 lines * [1..100] characters
contentLines = htmlParser.wrap_lines(1, 100)
for contentLine in contentLines:
  childLine5 = etree.SubElement(childFixedText5, "Line")
  childLine5.text = contentLine

# TypeBIV = TW3L
childText6 = etree.SubElement(childMessaging, "Text")
childTypeBIV6 = etree.SubElement(childText6, "TypeBIV")
childTypeBIV6.text = "TW3L"
childFixedText6 = etree.SubElement(childText6, "FixedText")
# TW3L accepts 3 lines * [0..30] characters (must pad with blank lines)
contentLines = htmlParser.wrap_lines(3, 30)
blankLinesNb = 3 - len(contentLines)
for contentLine in contentLines:
  childLine6 = etree.SubElement(childFixedText6, "Line")
  childLine6.text = contentLine
# Blank lines for padding
for i in range(0, blankLinesNb):
  childLine6 = etree.SubElement(childFixedText6, "Line")
# Scrolling text accepts 1 line * [1..100] characters
childScrollingText6 = etree.SubElement(childText6, "ScrollingText")
contentLines = htmlParser.wrap_lines(1, 100)
for contentLine in contentLines:
  childScrollingLine6 = etree.SubElement(childScrollingText6, "Line")
  childScrollingLine6.text = contentLine


# Recipients
childRecipients = etree.SubElement(childMessaging, "Recipients")
recipients = message[0]["recipients"][0]
hasAllNetwork = False
if "line" in recipients:
  # Scan the 'line' recipients to check if the whole transport network is selected
  for line in recipients["line"]:
    hasAllNetwork = hasAllNetwork or (line["id"] == network_id)
  # If it is, use 'AllNetwork' tag
  if hasAllNetwork:
    childAllNetwork = etree.SubElement(childRecipients, "AllNetwork")
  # Else add the Ineo code of each commercial line in the recipients
  else:
    childLinesWays = etree.SubElement(childRecipients, "LinesWays")
    for line in recipients["line"]:
      linkParameters = line["link_parameter"] if "link_parameter" in line else ""
      parameters = { "roid": line["id"] }
      linePM = synthese.service("LinesListFunction2", parameters)
      lineCodesStr = linePM["line"][0]["creator_id"]
      lineCodes = map(lambda x: x.split('|'), lineCodesStr.split(','))
      for lineCode in lineCodes:
        if lineCode[0] == datasource_id:
          childLineWay = etree.SubElement(childLinesWays, "LineWay")
          childLine = etree.SubElement(childLineWay, "Line")
          childLine.text = lineCode[1]
          childInWard = etree.SubElement(childLineWay, "InWard")
          childInWard.text = "non" if linkParameters == "1" else "oui"
          childOutWard = etree.SubElement(childLineWay, "OutWard")
          childOutWard.text = "non" if linkParameters == "0" else "oui"

# Print resulting XML to output stream
print(etree.tostring(root, pretty_print=True, xml_declaration=True, encoding="iso-8859-1"))
