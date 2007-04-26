#!/bin/sh

# Usage : install.sh INSTALLPORT INSTALLDBPORT INSTALLLOGLEVEL INSTALLLOGFILE

INSTALLDIR=/srv/synthese3
DISTNAME=#DISTNAME#
INSTALLPORT=${1:-3593}
INSTALLDBPORT=${2:-3594}
INSTALLLOGLEVEL=${3:-0}
INSTALLLOGFILE=${4:-/var/log/$DISTNAME.log}

echo Install dir is set to $INSTALLDIR
mkdir -p $INSTALLDIR/$DISTNAME

echo Stopping service $DISTNAME
test -x /etc/init.d/$DISTNAME && /etc/init.d/$DISTNAME stop

echo Installing server in $INSTALLDIR
cp -rf _$DISTNAME/* $INSTALLDIR/$DISTNAME/

echo Creating new init.d script
cp initd.sh /etc/init.d/$DISTNAME
sed -i "s?#INSTALLDIR#?$INSTALLDIR?" /etc/init.d/$DISTNAME
sed -i "s?#INSTALLPORT#?$INSTALLPORT?" /etc/init.d/$DISTNAME
sed -i "s?#INSTALLDBPORT#?$INSTALLDBPORT?" /etc/init.d/$DISTNAME
sed -i "s?#INSTALLLOGLEVEL#?$INSTALLLOGLEVEL?" /etc/init.d/$DISTNAME

sed -i "s?#INSTALLLOGFILE#?$INSTALLLOGFILE?" $INSTALLDIR/$DISTNAME/startup.sh

echo Starting service $DISTNAME
test -x /etc/init.d/$DISTNAME && /etc/init.d/$DISTNAME start






