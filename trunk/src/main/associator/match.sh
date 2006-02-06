#!/bin/sh

[ $# -ne 1 ] && exit
id=$1
[ "`echo $id | cut -c1`" == "*" ] && {
  file=gares.csv
  match=":\\$id$"
} || {
  file=routes.csv
  match=":$id$"
}
grep "$match" $file
