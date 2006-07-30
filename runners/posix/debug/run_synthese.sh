#!/bin/sh

if [ $# -eq 0 ] ; then
  CMD="../../../build/posix/debug/main/99_synthese/99_synthese --db config_only.db3"
else
  CMD="../../../build/posix/debug/main/99_synthese/99_synthese $@"
fi

echo
echo $CMD
echo

$CMD



