#!/usr/bin/python
#
# This tool take a SYNTHESE database and a date as input and
# create a file containing the GPS coordinates for the specified traccar id
#
# The tool that receives the information is:
# https://www.traccar.org/
#
# To extract and send the data in one shot:
# ./synthese2traccar.py | nc optymo-tracker.rcsmobility.com 5005
#
#

import pyspatialite.dbapi2 as db

# Replace here the date and base to export to traccar
date='2015-04-09%'
base='config.db3'
traccar_imei='799'

# don't include initial "$" or trailing "*XX"
# getChecksum("GPRMC,004422.767,A,3601.9719,N,07950.6023,W,0.00,,180804,,,A")
# should return a "*6F"    
def getChecksum(item):
    s = 0
    for i in range( len(item) ):
        s = s ^ ord(item[i])

    checksum = "*"
    #convert to hex
    s = "%02X" % s
    checksum += s
    return checksum

# Must return
#TRCCR,20140111000000.000,A,60.000000,60.000000,0.00,0.00,0.00,50
def getGPSString(date, imei, lon, lat):
    datetime = date.split(" ")
    d = datetime[0].split('-')
    t = datetime[1].split(':')
    return "TRCCR," + d[0] + d[1] + d[2] + "{:0>2d}".format(int(t[0]) - 2) + t[1] + t[2] + \
        ".000,A," + str(lat) + "," + str(lon) + ",0.00,0.00,0.00,0"

imei=traccar_imei
conn = db.connect(base)
c = conn.cursor()
request="select time, X(geometry), Y(geometry) from t072_vehicle_positions where time like " + date


id="PCPTI,test4," + imei + "," + imei + ".0,56"
print "$" + id + getChecksum(id) 

for row in c.execute(request):
    if(row[1]):
        gpsStr = getGPSString(row[0], imei, row[1], row[2])
        print "$" + gpsStr + "," + getChecksum(gpsStr)


