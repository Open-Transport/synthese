#!/bin/sh

CURDIR=`pwd`
EXECDIR=`dirname $CURDIR/$0`  
EXECLIBSDIR=$EXECDIR/libs

export LD_LIBRARY_PATH=$EXECLIBSDIR:$LD_LIBRARY_PATH

cd $EXECDIR
./97_synthese_refres --db config.db3



