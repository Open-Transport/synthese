#!/bin/sh

NAME=s3-server

source /etc/opt/rcs/$NAME.conf

DAEMON=/opt/rcs/bin/$NAME

test -x $DAEMON || exit 0

while true
do
	($DAEMON --pidfile $S3_SERVER_PID_FILE --logfile $S3_SERVER_LOG_FILE --db $S3_SERVER_DB_FILE --param tmp_dir=$S3_SERVER_TMP_DIR --param port=$S3_SERVER_PORT --param db_port=$S3_SERVER_DB_PORT --param log_level=$S3_SERVER_LOG_LEVEL --param dbring_node_id=$S3_SERVER_NODE_ID --param dbring_authority=$S3_SERVER_AUTHORITY_NODE_ID) || (echo "Failed!" && exit 1)
	rm -f $S3_SERVER_PID_FILE

	mail $S3_EMAIL_SEGFAULT -s "Crash SYNTHESE3 sur `hostname`" <<ENDOFMAIL
SYNTHESE3 a rebooté sur `hostname`, demarrage automatique en cours.

`tail -n 100 /var/log/rcs/s3-server.log`

ENDOFMAIL

done

