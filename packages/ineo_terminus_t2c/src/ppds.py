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
root = etree.Element("Ppds" + type + "MessageRequest")
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
childStartTime = etree.SubElement(childMessaging, "StartTime")
childStartTime.text = "00:00:00"
childStopTime = etree.SubElement(childMessaging, "StopTime")
childStopTime.text = "23:59:00"

# Text
# Split text around <br /> and \n
childText = etree.SubElement(childMessaging, "Text")
h = HTMLParser.HTMLParser()
for line in re.split('<br />|\n',message[0]["content"]):
  childLine = etree.SubElement(childText, "Line")
  childLine.text = h.unescape(line)

# Recipients
childRecipients = etree.SubElement(childMessaging, "Recipients")
childAllNetwork = etree.SubElement(childRecipients, "AllNetwork")

# Print resulting XML to output stream
print(etree.tostring(root, pretty_print=True, xml_declaration=True, encoding="iso-8859-1"))
