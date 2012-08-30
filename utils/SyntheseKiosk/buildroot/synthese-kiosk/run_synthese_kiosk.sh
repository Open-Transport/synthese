#!/bin/sh

rm -rf /tmp/kiosk_config
cp -r /usr/share/synthese_kiosk/kiosk_config_template /tmp/kiosk_config
python /usr/bin/synthese-kiosk -c /tmp/kiosk_config
