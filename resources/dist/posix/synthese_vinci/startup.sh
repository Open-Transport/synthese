#!/bin/sh

CURDIR=`pwd`

D=`dirname "$0"`
B=`basename "$0"`
ABSPATH="`cd \"$D\" 2>/dev/null && pwd || echo \"$D\"`/$B"

EXECDIR=`dirname $ABSPATH`  
EXECLIBSDIR=$EXECDIR/libs

export LD_LIBRARY_PATH=$EXECLIBSDIR:$LD_LIBRARY_PATH

cd $EXECDIR

while true
do
  ./#DISTNAME# $@ >> #INSTALLLOGFILE#
done




