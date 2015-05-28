#!/bin/bash

# This script is aimed at synchronizing a local filesystem directory
# with a remote synthese cms package. Since it uses connect to do its
# job, connect must have been installed first through service 'Package'.

# Presence of metadata.json in execution dir preserves special
# properties assigned to pages.
# Consequently, a trick to reimport everything from scratch
# is to delete everything but the site part in metadata.json.

if [ "$#" -lt 2 ]
then
  echo "Usage: cms_sync_package.sh HOST PORT [CONTAINER]"
  exit 1
fi

HOST=$1
PORT=$2
CONTAINER=$3

[ -f metadata.json ] || { echo Cannot find metadata.json ; exit 1; }

TMP_PACKAGE_NAME=`mktemp -u package_XXXXXXXX`
TMP_COOKIE="/tmp/cookie_$TMP_PACKAGE_NAME"
TMP_PACKAGE_DIR="/tmp/$TMP_PACKAGE_NAME"
REMOTE_TMP_PACKAGE_DIR="/var/lib/lxc/$CONTAINER/rootfs/tmp/$TMP_PACKAGE_NAME"

if [ -z "$CONTAINER" ]
then
  REMOTE_TMP_PACKAGE_DIR="/tmp/$TMP_PACKAGE_NAME"
fi

echo "-----------------------------------------"
echo "Host               : $HOST"
echo "Port               : $PORT"
echo "Container          : $CONTAINER"
echo "Package name       : $TMP_PACKAGE_NAME"
echo "Cookie             : $TMP_COOKIE"
echo "Package dir        : $TMP_PACKAGE_DIR"
echo "Remote package dir : $REMOTE_TMP_PACKAGE_DIR"
echo "-----------------------------------------"

SITE_ID=`cat metadata.json | cut -d "}"  -f 1 | grep -o "\"id\":\"[0-9]*" | grep -o "[0-9]*"`
echo "Parsed site id from metadata.json is $SITE_ID"

# ======================= LOGIN
curl -s -c $TMP_COOKIE -F "a=login" -F "actionParamlogin=root" -F "actionParampwd=root" http://$HOST:$PORT/connect/ >& /dev/null

# ======================= SYNC PACKAGE FILES TO REMOTE
rsync -qavz --delete . root@$HOST:$REMOTE_TMP_PACKAGE_DIR/ 
echo "Synchronized package files from local to remote"

# ======================= DUMMY DATA SOURCE
DATA_SOURCE_ID=`curl -s -b $TMP_COOKIE "http://$HOST:$PORT/connect/client?SERVICE=objectcreate&table_id=59&field_name=$TMP_PACKAGE_NAME&of=xml" | grep -o "[0-9]*"`
echo "Created dummy data source $DATA_SOURCE_ID"

# ======================= IMPORT CMS PACKAGE
CMS_IMPORT_ID=`curl -s -b $TMP_COOKIE "http://$HOST:$PORT/connect/client?SERVICE=objectcreate&table_id=105&field_name=$TMP_PACKAGE_NAME&field_data_source_id=$DATA_SOURCE_ID&field_active=0&field_file_format=CMS%20Import&of=xml" | grep -o "[0-9]*"`
echo "Created dummy cms import $CMS_IMPORT_ID"

# no not specify site id here, otherwise it is not created it it does not exist on remote side
curl -s -c $TMP_COOKIE -F "roid=$CMS_IMPORT_ID" -F "directory=$TMP_PACKAGE_DIR" -F "parent_id=" -F "import_site_id=" -F "with_metadata=1" -F "run_import=2" -F "min_log_level=0" http://$HOST:$PORT/connect/client >& /dev/null

# ======================= EXPORT CMS PACKAGE
CMS_EXPORT_ID=`curl -s -b $TMP_COOKIE "http://$HOST:$PORT/connect/client?SERVICE=objectcreate&table_id=105&field_name=$TMP_PACKAGE_NAME&field_data_source_id=$DATA_SOURCE_ID&field_active=0&field_file_format=CMS%20Export&of=xml" | grep -o "[0-9]*"`
echo "Created dummy cms export $CMS_EXPORT_ID"

curl -s -c $TMP_COOKIE -F "roid=$CMS_EXPORT_ID" -F "directory=$TMP_PACKAGE_DIR" -F "parent_id=" -F "import_site_id=$SITE_ID" -F "with_metadata=1" -F "run_import=2" -F "min_log_level=0" http://$HOST:$PORT/connect/client >& /dev/null

# ======================= SYNC BACK PACKAGE FILES FROM REMOTE
rsync -qavz --delete root@$HOST:$REMOTE_TMP_PACKAGE_DIR/ . 
echo "Synchronized back package files from remote to local"

# ======================= DELETE TMP EXPORT IMPORT
curl -s -c $TMP_COOKIE -F "a=RemoveObjectAction" -F "actionParam_object_id=$CMS_EXPORT_ID" http://$HOST:$PORT/connect/ >& /dev/null
echo "Deleted dummy cms export $CMS_EXPORT_ID"

curl -s -c $TMP_COOKIE -F "a=RemoveObjectAction" -F "actionParam_object_id=$CMS_IMPORT_ID" http://$HOST:$PORT/connect/ >& /dev/null
echo "Deleted dummy cms import $CMS_IMPORT_ID"

curl -s -c $TMP_COOKIE -F "a=RemoveObjectAction" -F "actionParam_object_id=$DATA_SOURCE_ID" http://$HOST:$PORT/connect/ >& /dev/null
echo "Deleted dummy data source $DATA_SOURCE_ID"

ssh root@$HOST rm -rf $REMOTE_TMP_PACKAGE_DIR/ 
echo "Deleted remote package dir $REMOTE_TMP_PACKAGE_DIR/"
