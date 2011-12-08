#!/bin/sh

NAME=s3-server

. /etc/opt/rcs/$NAME.conf

DAEMON=/opt/rcs/bin/$NAME

test -x $DAEMON || exit 0
while true
do
	($DAEMON \
		--pidfile $S3_SERVER_PID_FILE \
		--logfile $S3_SERVER_LOG_FILE \
		--dbconn $S3_SERVER_DB_CONNECTION \
		--param tmp_dir=$S3_SERVER_TMP_DIR \
		--param port=$S3_SERVER_PORT \
		--param log_level=$S3_SERVER_LOG_LEVEL \
		--param nb_threads=$S3_NB_THREADS \
		--param session_max_duration=$S3_SESSION_MAX_DURATION
	) || (echo "Failed!" && exit 1)
	rm -f $S3_SERVER_PID_FILE

	mail $S3_EMAIL_SEGFAULT -s "SYNTHESE3 crash on `hostname`" <<ENDOFMAIL
SYNTHESE3 was rebooted on `hostname`, automatic restart is in progress.

`tail -n 100 /var/log/rcs/s3-server.log`

ENDOFMAIL

done
