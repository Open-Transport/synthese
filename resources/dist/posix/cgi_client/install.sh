#!/bin/sh

# Usage : install.sh 

INSTALLDIR=/srv/synthese3
DISTNAME=cgi-client

echo Install dir is set to $INSTALLDIR
mkdir -p $INSTALLDIR/$DISTNAME

#test -x /etc/init.d/apache2 && /etc/init.d/apache2 stop

echo Installing in $INSTALLDIR
cp -rf _$DISTNAME/* $INSTALLDIR/$DISTNAME/

#cp site /etc/apache2/sites-available/$DISTNAME
#ln -sf /etc/apache2/sites-available/$DISTNAME /etc/apache2/sites-enabled/$DISTNAME

#sed -i "s?#INSTALLDIR#?$INSTALLDIR?" /etc/apache2/sites-available/$DISTNAME

#sed -i "s?#INSTALLHOST#?$INSTALLHOST?" $INSTALLDIR/$DISTNAME/startup.sh
#sed -i "s?#INSTALLPORT#?$INSTALLPORT?" $INSTALLDIR/$DISTNAME/startup.sh


#test -x /etc/init.d/apache2 && /etc/init.d/apache2 start







