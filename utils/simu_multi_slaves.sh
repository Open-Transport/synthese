#!/bin/sh

for var in "$@"
do
    python slave_simu.py localhost:8080 $var 1 &
done
