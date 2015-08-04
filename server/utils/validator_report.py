#!/usr/bin/python
#
# Print a report of what SYNTHESE sends to the valideur (Vix Format)
#
import sys
import re

if len(sys.argv) != 2:
    print 'Usage: valideurReport.py <synthese.log> (use - for stdin)'
    sys.exit(1)

logfile = sys.argv[1]

gps_stop_id = None
gps_stop_name = ''
vix_stop = None
previous_vix_stop = None

missing_stop_id = set()
missing_stop_name = set()
mismatch = set()

def print_errors(title, data):
    if not data:
        return

    print title
    for name in data:
        print '"' + name + '"'

input = sys.stdin if logfile == '-' else open(logfile)

for line in input:
    gps_search = re.search('GPSdFileFormat : Stop is (.*) name=(.*) lat=', line)
    if gps_search:
        # In some case there is several operator code, we take only thhe first one
        gps_stop_id = gps_search.group(1).split(',')[0]
        gps_stop_name = gps_search.group(2)
        if not gps_stop_id:
            missing_stop_id.add(gps_stop_name)
        if not gps_stop_name:
                missing_stop_name.add(gps_stop_id)

    vix_search = re.search('[A-Z]+ # (.*)\.[0-9]+ # .* # .*num_stop=([0-9]+)', line)
    if vix_search and vix_stop != vix_search.group(1):
        vix_date = vix_search.group(1)
        vix_stop = vix_search.group(2)
        if gps_stop_id and vix_stop != gps_stop_id:
            mismatch.add("For '%s' (%s) we send '%s' to the valideur" \
                %(gps_stop_id, gps_stop_name, vix_stop))
        elif not previous_vix_stop or previous_vix_stop != gps_stop_id:
            print vix_date + ' ' + vix_stop + ' (' + gps_stop_name + ')'
            previous_vix_stop = vix_stop


print ''
print_errors("MISSING STOP NAME FOR STOP ID:", missing_stop_name)
print_errors("MISSING STOP ID FOR STOP NAME:", missing_stop_id)
print_errors("MISSMATCH BETWEEN KNOW POSITION AND SENT ONE:", mismatch)
