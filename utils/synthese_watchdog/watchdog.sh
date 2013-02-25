#!/bin/sh
sleep 240
while true
do 
	supervisor_status=`supervisorctl status synthese | grep RUNNING`
	if [ ! -z "$supervisor_status" ]
	then
		curl -o /dev/null --silent -m 5  http://localhost/
		rc=$?
		if [ $rc -ne 0 ]
		then
			date >> /var/log/synthese.log
			echo " SYNTHESE did not answer to port 80 and will be restarted by the watchdog" >> /var/log/synthese.log
			supervisorctl restart synthese
			sleep 240
		fi
	fi
	sleep 10
done
