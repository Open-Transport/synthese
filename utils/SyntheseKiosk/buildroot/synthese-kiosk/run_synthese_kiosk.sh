#!/bin/sh

rm -rf /tmp/kiosk_config
mkdir /tmp/kiosk_config
cp -r /etc/synthese_kiosk/config.json /tmp/kiosk_config
xrandr -d :0 --output LVDS1 --rotate left
python /usr/bin/synthese-kiosk -c /tmp/kiosk_config
