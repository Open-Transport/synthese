(function() {
if (!window.requireJSConfig)
  window.requireJSConfig = {};

function getVersion() {
  var scripts = document.getElementsByTagName("script");
  for (var i = 0; i < scripts.length; i++) {
    var script = scripts[i];
    var src = scripts[i].getAttribute("src");
    if (!src || src.search(/\/init.js$/) == -1)
      continue;
    return script.getAttribute("data-version");
  }
}

requireJSConfig.paths = {
  "text": "core/vendor/requirejs/text",
  "order": "core/vendor/requirejs/order"
};
requireJSConfig.baseUrl = requireJSConfig.baseUrl || "/";

var version = getVersion();
if (version)
  requireJSConfig.urlArgs = "v=" + version;

var locStorage = window.localStorage || {};
if (locStorage.debug) {
  requireJSConfig.urlArgs = "bust=" + (new Date()).getTime();
}

require.config(requireJSConfig);

})();
