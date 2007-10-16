#!/bin/bash

cd /usr/share/synthese3-server/

while true
do
  ./synthese3-server $@ >> /var/log/synthese3-server.log
done




