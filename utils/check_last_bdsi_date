#!/usr/bin/python
#
# This script checks that a log file containing the date
# of the MAJ table of the BDSI is not too old.
#
# On the BDSI serveur, add this line in the crontab:
# *  *    * * *   root    mysql -uroot -pXXXXX -Ns bdsi \
#   --execute 'select date_fin,heure_fin from MAJ;' \
#   > /var/lib/synthese/bdsi_last_maj
#
# On the icinga nrpe server side, add this
# command[check_bdsi_t2c]=/usr/lib/nagios/plugins/check_last_bdsi_date \
#   /var/lib/lxc/t2c-prod/rootfs/var/lib/synthese/bdsi_last_maj -a 300
#
#

import optparse
import sys
import os
from datetime import datetime
from datetime import timedelta

NAGIOS_STATUS = { "OK": 0, "WARNING": 1, "CRITICAL": 2, "UNKNOWN": 3 }

parser = optparse.OptionParser()
parser.set_usage("%prog [options] <bdsi_last_maj_file>")
parser.add_option("-a", "--age", dest="age", metavar="AGE",
    type="string", default="300",
    help="maximum age, in seconds - default is 300s.")
(options, args) = parser.parse_args()

if len(args) != 1:
    parser.print_help(file=sys.stderr)
    sys.exit(1)

statusfile = args[0]

max_age = int(options.age)

# let's see if it exists
if not os.path.exists(statusfile):
    print "UNKNOWN: %s does not exist."%(statusfile)
    sys.exit(NAGIOS_STATUS['UNKNOWN'])

bdsi_date=[]
with open(statusfile) as f:
    bdsi_date = f.readline().strip('\n').split()

if bdsi_date == []:
    print "UNKNOWN: %s does not exist."%(statusfile)
    sys.exit(NAGIOS_STATUS['UNKNOWN'])

bdsi_date_str=bdsi_date[0] + " " + bdsi_date[1]
date_object = datetime.strptime(bdsi_date_str, '%Y-%m-%d %H:%M:%S')
now = datetime.now()

if now - date_object > timedelta(seconds=max_age):
    # BDSI update is too old
    print "ERROR: BDSI is %d minutes old."%((now - date_object).seconds / 60)
    sys.exit(NAGIOS_STATUS['CRITICAL'])

print "OK: BDSI is %d minutes old"%((now - date_object).seconds / 60)
sys.exit(NAGIOS_STATUS['OK'])


