#!/bin/bash

set -e

HERE=$(cd $(dirname $0) && pwd)
SYNTHESE_DIR=$(realpath $HERE/../../..)
LIB_DIR=$(realpath $SYNTHESE_DIR/packages/lib/src)
LIB_SYNTHESE_DIR=$(realpath $LIB_DIR/synthese)

SITES="riv"

# Synthese base

cd $LIB_SYNTHESE_DIR
npm i
if [ ! -d node_modules/synthese ]; then
  mkdir node_modules/synthese
fi

ln -snf ../../base node_modules/synthese/base
ln -snf ../../base.ts node_modules/synthese/base.ts
ln -snf ../../modules node_modules/synthese/modules

./node_modules/.bin/tsc

# Sites

for site in $SITES; do

  cd $SYNTHESE_DIR/packages/$site/src
  npm i
  ln -snf ../../../../lib/src/synthese/base node_modules/synthese/base
  ln -snf ../../../../lib/src/synthese/modules node_modules/synthese/modules
  ln -snf ../../../../lib/src/synthese/base.ts node_modules/synthese/base.ts

  ./node_modules/.bin/tsc

done
