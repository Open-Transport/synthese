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
root = etree.Element("Driver" + type + "MessageRequest")
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

# MultipleStop
if int(needs_multiple_stops) != 0:
  childMultipleStop = etree.SubElement(childMessaging, "MultipleStop")
  childMultipleStop.text = "oui" if int(message[0]["multiple_stops"]) != 0 else "non"

# EndStopPoint
childEndStopPoint = etree.SubElement(childMessaging, "EndStopPoint")
childEndStopPoint.text = "non"

# Way

# StopPoint
recipients = message[0]["recipients"][0]
if 'stoparea' in recipients:
  childStopPoint = etree.SubElement(childMessaging, "StopPoint")
  # SYNTHESE has stop area recipients, but Ineo expects stop points so we request all the stop points of each stop area
  parameters = { "roid": recipients["stoparea"][0]["id"], "output_stops": 1, "of" : "text" }
  stopAreasPM = synthese.service("StopAreasListFunction", parameters)
  # Loop over stop points
  for stop in stopAreasPM["stopArea"][0]["stop"]:
    # Split the operator codes and find the Ineo code
    stopCodes = map(lambda x: x.split('|'), stop["operator_code"].split(','))
    for stopCode in stopCodes:
      if stopCode[0] == datasource_id:
        childStopPoint.text = stopCode[1]

# RepeatPeriod

# Confirm

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
