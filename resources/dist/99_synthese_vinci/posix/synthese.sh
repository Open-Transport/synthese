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
        echo "demarrage..."
	./99_synthese_vinci --db config.db3 --param log_level=0  --param port=3595 --param db_port 3596
done




