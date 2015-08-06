#!/usr/bin/python2

import datetime
import re
import HTMLParser
import synthese

try:
  from lxml import etree
except ImportError:
  print("la lib lxml n'est pas disponible")


# Request headers
root = etree.Element("BivLineMan" + type + "MessageRequest")
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

# Tts
if int(needs_play_tts) != 0:
  if int(message[0]["play_tts"]) != 0:
    childTts = etree.SubElement(childMessaging, "Tts")
    childTts.text = message[0]["content"]

# {Start,Stop}Date
childStartDate = etree.SubElement(childMessaging, "StartDate")
childStartDate.text = "01/01/1970"
childStopDate = etree.SubElement(childMessaging, "StopDate")
childStopDate.text = "31/12/2037"

# {Start,Stop}Time

# Alternance

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
