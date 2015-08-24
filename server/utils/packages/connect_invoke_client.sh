#!/bin/bash

# This script is aimed at invoking client "import" entity in Connect. It is
# designed to be used to quickly trigger CMS Import, after manual creation of
# the entity with source folder and target site ID set in default parameters.

if [ "$#" -lt 1 ]
then
  echo "Usage: connect_invoke_client.sh CONNECT_CLIENT_ID [HOST [PORT]]"
  exit 1
fi

CONNECT_CLIENT_ID=${1}
HOST=${2:-localhost}
PORT=${3:-8080}

# Typical use does not require access through proxy
CURL_OPTS="--noproxy '*' --progress-bar --connect-timeout 5"
# Additional trace level for debugging failed communication
#CURL_OPTS="${CURL_OPTS} --verbose"
#CURL_OPTS="${CURL_OPTS} --trace -"

# Generated files
TMP_DIR=`mktemp -u /tmp/connect_invoke_XXXXXXXX`
mkdir ${TMP_DIR}
TMP_COOKIE="${TMP_DIR}/cookie"
TMP_OUTPUT="${TMP_DIR}/output"
TMP_RESULT="${TMP_DIR}/result"
TMP_LOG="${TMP_DIR}/log"

cat <<-EOF
	-----------------------------------------
	Connect client ID  : $CONNECT_CLIENT_ID
	Host               : $HOST
	Port               : $PORT
	Cookie             : $TMP_COOKIE
	HTML Output        : $TMP_OUTPUT
	Text Result        : $TMP_RESULT
	Extract import log : $TMP_LOG
	-----------------------------------------
EOF

# ======================= LOGIN
curl ${CURL_OPTS} --cookie-jar ${TMP_COOKIE} --form "a=login" \
     --form "actionParamlogin=root" --form "actionParampwd=root" --form "roid=${CONNECT_CLIENT_ID}" "http://${HOST}:${PORT}/connect/client"

if [ $? != 0 ]; then
    echo "Login failed. curl return code: $?"
    exit 1;
fi

# ======================= TRIGGER CLIENT IMPORT
curl ${CURL_OPTS} --cookie ${TMP_COOKIE} --cookie-jar ${TMP_COOKIE} --output ${TMP_OUTPUT} \
    --form "tab=run" --form "roid=${CONNECT_CLIENT_ID}" --form "run_import=2" --form "min_log_level=0" "http://${HOST}:${PORT}/connect/client"

html2text -ascii -style compact -o ${TMP_RESULT} ${TMP_OUTPUT}

# TODO: grep for "Journal d'import" and only display it. If none, error
cat ${TMP_RESULT}

if [ $? == 0 ]; then
    # Success
    rm -f ${TMP_COOKIE} ${TMP_OUTPUT} ${TMP_RESULT} ${TMP_LOG}
    rmdir ${TMP_DIR}
    exit 0
fi
exit 1
