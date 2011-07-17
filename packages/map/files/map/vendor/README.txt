TODO: move libraries used by other packages to the core package.

OpenLayers:
  Version: 2.11-rc1
  http://openlayers.org/download/OpenLayers-2.11-rc1.zip
  
  Removed unneeded files:
  cd OpenLayers
  rm -rf apidoc_config art build doc doc_config examples license.txt readme.txt release-license.txt repository-license.txt tests tools
  mv lib lib_tmp
  mkdir lib
  (cd lib_tmp; cp --parents OpenLayers/Lang/en.js OpenLayers/Lang/fr.js ../lib)
  rm -rf lib_tmp

  # To develop OpenLayers, you may want to get the sources (not committed for size).
  wget http://openlayers.org/download/OpenLayers-2.11-rc1.zip
  unzip OpenLayers-2.11-rc1.zip
  mv OpenLayers-2.11-rc1 OpenLayers_full

OpenLayers_maptypepanel:
  The following files were retrieved From the maptypepanel camptocamp OpenLayers sandbox:
  http://svn.openlayers.org/sandbox/camptocamp/maptypepanel/openlayers/lib/OpenLayers/Control/MapTypePanel.js@10557
  http://svn.openlayers.org/sandbox/camptocamp/maptypepanel/openlayers/lib/OpenLayers/Control/MapType.js@10557
  Patch in OpenLayers_maptypepanel/patch.diff applied to make it work with OpenLayers 2.11.

jquery.tmpl.beta1.min.js:
jquery.tmpl.beta1.js:
  Version: beta 1
  http://ajax.microsoft.com/ajax/jquery.templates/beta1/jquery.tmpl.min.js
  http://ajax.microsoft.com/ajax/jquery.templates/beta1/jquery.tmpl.js

underscore-1.1.6.min.js:
underscore-1.1.6.js:
  Version: 1.1.6
  http://documentcloud.github.com/underscore/underscore-min.js
  http://documentcloud.github.com/underscore/underscore.js

backbone-0.5.1.min.js:
backbone-0.5.1.js:
  Version: 0.5.1
  http://documentcloud.github.com/backbone/backbone-min.js
  http://documentcloud.github.com/backbone/backbone.js
