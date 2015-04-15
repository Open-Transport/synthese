(function() {
if (!window.requireJSConfig)
  window.requireJSConfig = {};

function getVersion() {
  var scripts = document.getElementsByTagName("script");
  for (var i = 0; i < scripts.length; i++) {
    var script = scripts[i];
    var src = scripts[i].getAttribute("src");
    if (!src || src.search(RegExp("/init\.js$")) == -1)
      continue;
    return script.getAttribute("data-version");
  }
}

requireJSConfig.paths = {
  "text": "lib/requirejs/text",
  "jquery": "lib/jquery/jquery-1.7.1.min",
  "jquery.tmpl": "lib/jquery/jquery.tmpl.beta1.min",
  "jquery.placeholder": "lib/jquery/jquery.placeholder.min",
  "underscore": "lib/underscore/underscore-1.1.6.min",
  "backbone": "lib/backbone/backbone-0.5.1.min",
  "bootstrap": "core/vendor/bootstrap/js/bootstrap.min",

  // Debug versions:
  //"jquery": "lib/jquery/jquery-1.7.1",
  //"jquery.tmpl": "core/vendor/jquery.tmpl.beta1",
  //"underscore": "core/vendor/underscore-1.1.6",
  //"backbone": "core/vendor/backbone-0.5.1"

  "OpenLayers": "lib/openlayers/OpenLayers",
  // Debug version:
  // To load the debug version of OpenLayers, comment the line above and uncomment the one below.
  // Then, put the following code in the html:.
  // <script src="/map/vendor/OpenLayers_full/lib/OpenLayers.js"></script>
  //"OpenLayers": "core/js/dummy",

  "OpenLayers_fr": "lib/openlayers/lib/OpenLayers/Lang/fr",
  "OpenLayers_maptype": "lib/openlayers_maptypepanel/MapType",
  "OpenLayers_maptypepanel": "lib/openlayers_maptypepanel/MapTypePanel"
};
requireJSConfig.baseUrl = requireJSConfig.baseUrl || "/";

requireJSConfig.shim = {
  "OpenLayers_fr": ["OpenLayers"],
  "OpenLayers_maptype": ["OpenLayers"],
  "OpenLayers_maptypepanel": ["OpenLayers"],

  "jquery.placeholder": ["jquery"],
  "jquery.tmpl": ["jquery"],
  "backbone": {
    deps: ["underscore", "jquery.tmpl"],
    exports: "Backbone"
  },
  "bootstrap": ["jquery"]
};

var version = getVersion();
if (version)
  requireJSConfig.urlArgs = "v=" + version;

var locStorage = window.localStorage || {};
if (locStorage.debug) {
  requireJSConfig.urlArgs = "bust=" + (new Date()).getTime();
}

require.config(requireJSConfig);

})();