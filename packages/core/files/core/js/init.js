var requireJSConfig = {
  paths: {
    "text": "/core/vendor/requirejs/text",
    "order": "/core/vendor/requirejs/order"
  }
};
var locStorage = window.localStorage || {};
if (locStorage.debug) {
  requireJSConfig.urlArgs = "bust=" + (new Date()).getTime();
}
require.config(requireJSConfig);
