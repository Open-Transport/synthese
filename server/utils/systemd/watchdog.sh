#!/bin/bash

PORT=$1
DEBUG=$2

MAXTRIES=20
WAIT=30 # seconds

# Kill Synthese
function killSynthese ()
{
	# In debug mode, produce a core dump. 
	# Else, just kill synthese
        if [[ $DEBUG == "1" ]]; then
		echo "Creating a dump file (debug activated)"
                killall -SIGABRT s3-server
        else
                killall -9 s3-server
        fi
}

echo "Synthese watchdog started"

while [ 1 ]; do

	echo "Waiting for Synthese to startup"

	# Wait for the server to startup (max $MAXTRIES tries)
	cnt=0	
	until curl -o /dev/null --silent -m 30 http://localhost:$PORT/; do

		sleep $WAIT

		cnt=$((cnt+1))

		if [ $cnt -ge $MAXTRIES ]; then
			echo "Synthese not responding after $MAXTRIES tries, killing it"
			killSynthese 
			cnt=0
		fi

	done

	echo "Synthese started"

	# Wait for a connection problem to happend
	while curl -o /dev/null --silent -m 30  http://localhost:$PORT/; do
		sleep $WAIT
	done

	echo "SYNTHESE did not answer to port $PORT and will be killed by the watchdog"

	# Kill synthese
	killSynthese

done
