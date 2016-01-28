#!/bin/bash

# This script is used to update the Angular 2 libraries in the lib package.
# It requires the Node.JS "npm" package to be installed.
# On Debian, you can install it with "apt-get install npm"

set -e

HERE=$(cd $(dirname $0) && pwd)
SYNTHESE_DIR=$(realpath $HERE/../../..)
LIB_DIR=$(realpath $SYNTHESE_DIR/packages/lib/src)
LIB_SYNTHESE_DIR=$(realpath $LIB_DIR/synthese)
CACHE_DIR=~/.cache/synthese/angular

if [ ! -d $CACHE_DIR ]; then
  mkdir -p $CACHE_DIR
fi

TO_COPY_FILES="
es6-shim/es6-shim.js
angular2/bundles/angular2-polyfills.js
systemjs/dist/system.src.js
rxjs/bundles/Rx.js
angular2/bundles/angular2.dev.js
angular2/bundles/http.dev.js
angular2/bundles/router.dev.js
typescript/lib/typescript.js
"

cp $LIB_SYNTHESE_DIR/package.json $CACHE_DIR/

(cd $CACHE_DIR && npm i)

for f in $TO_COPY_FILES; do
  (cd $CACHE_DIR/node_modules && cp -v --parents $f $LIB_DIR/angular2/)
done
