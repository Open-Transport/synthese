#!/usr/bin/python
#
# A slave simulator for InterSYNTHESE
#
import sys, httplib
from datetime import datetime
import time

def usage():
    print "slave_simu.py master_host:port slave_id [period]"


if len(sys.argv) < 2:
    usage()
    sys.exit(0)

master = sys.argv[1]
slave_id = sys.argv[2]
period = -1
try:
    period = sys.argv[3]
except:
    pass

conn = None

while True:
    date_start = datetime.now()
    print date_start.isoformat() + " Requesting update for slave " + slave_id


    if not conn:
        conn = httplib.HTTPConnection(master)
        time.sleep(1)
        continue

    try:
      conn.request("GET", "/?SERVICE=slave_update&slave_id=" + slave_id)
    except:
      time.sleep(1)
      continue

    r1 = conn.getresponse()
    print r1.status, r1.reason

    if r1.status != 200:
        time.sleep(1)
        conn.close()
        continue

    range_begin = None
    range_end = None
    size = 0
    line_count = 0
    data1 = r1.read()

    if data1 == "no_content_to_sync!" \
            or data1 == "we are processing your initial dump. come back soon!" \
            or data1 == "sorry another initial dump is running, come back soon":
        print "  " + data1 + " " + slave_id
        time.sleep( float(period) )
        conn.close()
        continue

    for line in data1.split('\n'):

        # with open('slate_data.txt', 'a') as f:
        #     f.write(line + '\n')

        #print "$$" + line + "$$"
        splitted_line = line.split(':', 3)
        if len(splitted_line[0]) != 17:
            continue
        try:
            int(splitted_line[0])
            int(splitted_line[2])
        except:
            continue

        range_end = splitted_line[0]
        if not range_begin:
            range_begin = range_end
        line_size = int(splitted_line[2])
        size += line_size
        line_count += 1

    print "  range= " + range_begin + " end= " + range_end
    if range_end:
        print "  Got " + str(line_count) + " items in " + str(size) + " bytes. Send ack for slave " + slave_id
        print "  range= " + range_begin + " " + range_end
        slave_ack_rqst = "/?SERVICE=inter_synthese_update_ack&range_begin=" + \
            range_begin + "&range_end=" + range_end + "&slave_id=" + slave_id

        conn.request("GET", slave_ack_rqst)
        r1 = conn.getresponse()
        print r1.status, r1.reason
        data1 = r1.read()

    print "  Duration = %d" % (datetime.now() - date_start).seconds

    conn.close()

    if period > 0:
        time.sleep( float(period) )
    else:
        sys.exit(0)
