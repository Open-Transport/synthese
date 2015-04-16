#!/bin/sh
#
# Restart synthese if it does not answer on http
# To avoid restarting a starting synthese, it only triggers once
# a given synthese pid answered at least once.
#
PORT=$1
pid=0
while true
do 
	supervisor_status=`supervisorctl status synthese | grep RUNNING`
	if [ ! -z "$supervisor_status" ]
	then
		newpid=`supervisorctl status synthese | sed 's/.*pid \([0-9]\+\).*/\1/g'`
		curl -o /dev/null --silent -m 15  http://localhost:$PORT/
		rc=$?
		if [ $rc -ne 0 ] && [ $newpid -eq $pid ]
		then
			fmtdate=`date +'%Y/%m/%d %T.%N'`
			echo "WATCHDOG # $fmtdate # SYNTHESE did not answer to port $PORT and is restarted by the watchdog" >> /var/log/synthese.log
			supervisorctl restart synthese
		elif [ $rc -eq 0 ]
		then
			pid=$newpid
		fi
	fi
	sleep 10
done
