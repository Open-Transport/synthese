#!/bin/sh
#
# Start/stops #DISTNAME# daemon
#
#

# Defaults

PIDDIR=/var/run/#DISTNAME#
PIDFILE=$PIDDIR/#DISTNAME#.pid


# See if the daemons are there
test -x #INSTALLDIR#/#DISTNAME#/startup.sh || ( echo "startup.sh not found !" && exit 0 )

. /lib/lsb/init-functions

case "$1" in
	start)
		log_daemon_msg "Starting #DISTNAME# service..."

		# Make sure we have our PIDDIR, even if it's on a tmpfs
		install -o root -g root -m 755 -d $PIDDIR

		if ! start-stop-daemon --start --background --make-pidfile --pidfile $PIDFILE --quiet --oknodo --startas #INSTALLDIR#/#DISTNAME#/startup.sh -- --param port=#INSTALLPORT# --param db_port=#INSTALLDBPORT# --param log_level=#INSTALLLOGLEVEL# ; then
			log_end_msg 1
			exit 1
		fi

		log_end_msg 0
		;;
	stop)
		log_daemon_msg "Stopping #DISTNAME# service..."

		start-stop-daemon --stop --quiet --pidfile $PIDFILE
		kill -2 `pidof #DISTNAME#`

		# Wait a little and remove stale PID file
		sleep 1
		if [ -f $PIDFILE ] && ! ps h `cat $PIDFILE` > /dev/null
		then
			# Stale PID file,
			# remove it
			rm -f $PIDFILE
		fi

		log_end_msg 0

		;;
	restart)
		$0 stop
		sleep 1
		$0 start
		;;
	*)
		log_success_msg "Usage: /etc/init.d/#DISTNAME# {start|stop|restart}"
		exit 1
		;;
esac

exit 0
