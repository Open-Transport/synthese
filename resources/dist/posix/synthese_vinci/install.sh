#!/bin/sh

# Usage : install.sh INSTALLPORT INSTALLDBPORT INSTALLLOGLEVEL INSTALLLOGFILE

INSTALLDIR=/srv/synthese3
DISTNAME=#DISTNAME#
INSTALLPORT=${1:-3595}
INSTALLDBPORT=${2:-3596}
INSTALLLOGLEVEL=${3:-0}
INSTALLLOGFILE=${4:-/var/log/$DISTNAME.log}

echo Install dir is set to $INSTALLDIR
mkdir -p $INSTALLDIR/$DISTNAME


WASSTARTED=0
(pgrep -x $DISTNAME >> /dev/null) && {
    WASSTARTED=1
}



[ "$WASSTARTED" != 0 ] && {
  test -x /etc/init.d/$DISTNAME && /etc/init.d/$DISTNAME stop
}


echo Installing server in $INSTALLDIR
cp -rf _$DISTNAME/* $INSTALLDIR/$DISTNAME/

echo Creating new init.d script
cp initd.sh /etc/init.d/$DISTNAME
sed -i "s?#INSTALLDIR#?$INSTALLDIR?" /etc/init.d/$DISTNAME
sed -i "s?#INSTALLPORT#?$INSTALLPORT?" /etc/init.d/$DISTNAME
sed -i "s?#INSTALLDBPORT#?$INSTALLDBPORT?" /etc/init.d/$DISTNAME
sed -i "s?#INSTALLLOGLEVEL#?$INSTALLLOGLEVEL?" /etc/init.d/$DISTNAME

sed -i "s?#INSTALLLOGFILE#?$INSTALLLOGFILE?" $INSTALLDIR/$DISTNAME/startup.sh

[ "$WASSTARTED" != 0 ] && {
  test -x /etc/init.d/$DISTNAME && /etc/init.d/$DISTNAME start
}








