if (!window.requireJSConfig)
  window.requireJSConfig = {};

requireJSConfig.paths = {
  "text": "core/vendor/requirejs/text",
  "order": "core/vendor/requirejs/order"
};
requireJSConfig.baseUrl = requireJSConfig.baseUrl || "/";

var locStorage = window.localStorage || {};
if (locStorage.debug) {
  requireJSConfig.urlArgs = "bust=" + (new Date()).getTime();
}

require.config(requireJSConfig);
