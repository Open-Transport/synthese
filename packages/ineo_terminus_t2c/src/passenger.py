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
childNumberShow = etree.SubElement(childMessaging, "NumberShow")
childNumberShow.text = "20"

# RepeatPeriod
if int(needs_repeat_interval) != 0:
  repeatPeriod = int(message[0]["repeat_interval"]) / 60
  if repeatPeriod > 0:
    childRepeatPeriod = etree.SubElement(childMessaging, "RepeatPeriod")
    childRepeatPeriod.text = str(repeatPeriod)

# Inhibition

# Color

# TtsBroadcasting
if int(needs_play_tts) != 0:
  childTts = etree.SubElement(childMessaging, "TtsBroadcasting")
  childTts.text = "oui" if int(message[0]["play_tts"]) != 0 else "non"

# Jingle
childJingle = etree.SubElement(childMessaging, "Jingle")
childJingle.text = "non"

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
childPriority = etree.SubElement(childMessaging, "Priority")
priority = int(message[0]["priority"])
childPriority.text = "oui" if priority >= 50 else "non"

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
