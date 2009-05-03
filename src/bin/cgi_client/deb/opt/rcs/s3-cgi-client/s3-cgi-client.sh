#!/bin/bash

source /etc/opt/rcs/s3-server.conf

/opt/rcs/bin/s3-cgi-client localhost $S3_SERVER_PORT

