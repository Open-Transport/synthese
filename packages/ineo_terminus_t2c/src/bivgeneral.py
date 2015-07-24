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
root = etree.Element("BivGeneral" + type + "MessageRequest")
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
# Split text around <br /> and \n
contentLines = re.split('<br />|\n',message[0]["content"])
h = HTMLParser.HTMLParser()

# TypeBIV = BUS4L
childText1 = etree.SubElement(childMessaging, "Text")
childTypeBIV1 = etree.SubElement(childText1, "TypeBIV")
childTypeBIV1.text = "BUS4L"
childFixedText1 = etree.SubElement(childText1, "FixedText")
for contentLine in contentLines:
  childLine1 = etree.SubElement(childFixedText1, "Line")
  childLine1.text = h.unescape(contentLine)

# TypeBIV = BUS8L
childText2 = etree.SubElement(childMessaging, "Text")
childTypeBIV2 = etree.SubElement(childText2, "TypeBIV")
childTypeBIV2.text = "BUS8L"
childFixedText2 = etree.SubElement(childText2, "FixedText")
for contentLine in contentLines:
  childLine2 = etree.SubElement(childFixedText2, "Line")
  childLine2.text = h.unescape(contentLine)

# TypeBIV = ER
childText3 = etree.SubElement(childMessaging, "Text")
childTypeBIV3 = etree.SubElement(childText3, "TypeBIV")
childTypeBIV3.text = "ER"
childFixedText3 = etree.SubElement(childText3, "FixedText")
for contentLine in contentLines:
  childLine3 = etree.SubElement(childFixedText3, "Line")
  childLine3.text = h.unescape(contentLine)

# TypeBIV = TFT
childText4 = etree.SubElement(childMessaging, "Text")
childTypeBIV4 = etree.SubElement(childText4, "TypeBIV")
childTypeBIV4.text = "TFT"
childFixedText4 = etree.SubElement(childText4, "FixedText")
for contentLine in contentLines:
  childLine4 = etree.SubElement(childFixedText4, "Line")
  childLine4.text = h.unescape(contentLine)

# TypeBIV = TTS
childText5 = etree.SubElement(childMessaging, "Text")
childTypeBIV5 = etree.SubElement(childText5, "TypeBIV")
childTypeBIV5.text = "TTS"
childFixedText5 = etree.SubElement(childText5, "TtsText")
childLine5 = etree.SubElement(childFixedText5, "Line")
childLine5.text = h.unescape(contentLines[0])

# TypeBIV = TW3L
childText6 = etree.SubElement(childMessaging, "Text")
childTypeBIV6 = etree.SubElement(childText6, "TypeBIV")
childTypeBIV6.text = "TW3L"
childFixedText6 = etree.SubElement(childText6, "FixedText")
for contentLine in contentLines:
  childLine6 = etree.SubElement(childFixedText6, "Line")
  childLine6.text = h.unescape(contentLine)
childScrollingText6 = etree.SubElement(childText6, "ScrollingText")
childScrollingLine6 = etree.SubElement(childScrollingText6, "Line")
childScrollingLine6.text = h.unescape(contentLines[0])


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
