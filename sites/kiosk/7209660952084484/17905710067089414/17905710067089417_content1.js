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
  "text": "core/vendor/requirejs/text",
  "jquery": "/kiosk/js/jquery-1.7.1.min",
  "jquery.tmpl": "/kiosk/js/jquery.tmpl.beta1.min",
  "jquery.placeholder": "core/vendor/jquery.placeholder.min",
  "underscore": "/kiosk/js/underscore-1.1.6.min",
  "backbone": "/kiosk/js/backbone-0.5.1.min",
  "bootstrap": "core/vendor/bootstrap/js/bootstrap.min",

  // Debug versions:
  //"jquery": "core/vendor/jquery-1.7.1",
  //"jquery.tmpl": "core/vendor/jquery.tmpl.beta1",
  //"underscore": "core/vendor/underscore-1.1.6",
  //"backbone": "core/vendor/backbone-0.5.1"

  "OpenLayers": "map/vendor/OpenLayers/OpenLayers",
  // Debug version:
  // To load the debug version of OpenLayers, comment the line above and uncomment the one below.
  // Then, put the following code in the html:.
  // <script src="/map/vendor/OpenLayers_full/lib/OpenLayers.js"></script>
  //"OpenLayers": "core/js/dummy",

  "OpenLayers_fr": "map/vendor/OpenLayers/lib/OpenLayers/Lang/fr",
  "OpenLayers_maptypepanel": "map/vendor/OpenLayers_maptypepanel/MapType"
};
requireJSConfig.baseUrl = requireJSConfig.baseUrl || "/";

requireJSConfig.shim = {
  "map/vendor/OpenLayers_maptypepanel/MapType": ["OpenLayers"],
  "map/vendor/OpenLayers_maptypepanel/MapTypePanel": ["OpenLayers"],
  "OpenLayers_fr": ["OpenLayers"],

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