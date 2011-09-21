OpenLayers:
  Version: 2.11
  http://openlayers.org/download/OpenLayers-2.11.zip

  Removed unneeded files:
  cd OpenLayers
  rm -rf apidoc_config art build doc doc_config examples license.txt readme.txt release-license.txt repository-license.txt tests tools
  mv lib lib_tmp
  mkdir lib
  (cd lib_tmp; cp --parents OpenLayers/Lang/en.js OpenLayers/Lang/fr.js ../lib)
  rm -rf lib_tmp

  # To develop OpenLayers, you may want to get the sources (not committed for size).
  wget http://openlayers.org/download/OpenLayers-2.11.zip
  unzip OpenLayers-2.11.zip
  mv OpenLayers-2.11 OpenLayers_full

OpenLayers_maptypepanel:
  The following files were retrieved From the maptypepanel camptocamp OpenLayers sandbox:
  http://svn.openlayers.org/sandbox/camptocamp/maptypepanel/openlayers/lib/OpenLayers/Control/MapTypePanel.js@10557
  http://svn.openlayers.org/sandbox/camptocamp/maptypepanel/openlayers/lib/OpenLayers/Control/MapType.js@10557
  Patch in OpenLayers_maptypepanel/patch.diff applied to make it work with OpenLayers 2.11.
