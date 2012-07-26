define([
  "jquery"
], function() {

// Dummy console.log for browsers without a console API.
try {
  console.log("");
} catch(e) {
  window.console = {
    log: function() {},
    warn: function() {}
  };
}

// TODO: refactor with SyntheseMap
var Synthese = {
  URL: "/synthese",

  init: function(siteId) {
    function parseQueryString(str) {
      var paramArray = str.split("&");
      var regex = /^([^=]+)=(.*)$/;
      var params = {};
      for (var i = 0, sz = paramArray.length; i < sz; i++) {
        var match = regex.exec(paramArray[i]);
        if (!match)
          continue;
        try {
          params[decodeURIComponent(match[1])] = decodeURIComponent(match[2]);
        } catch(e) {
          continue;
        }
      }
      return params;
    }

    this.queryStringOptions = parseQueryString(location.search.slice(1));
    this.siteId = siteId || this.queryStringOptions.siteId;
    if (!this.siteId) {
      console.warn("Missing siteId parameter");
      alert("Missing siteId parameter");
    }
  },

  callService: function(serviceName, args, dataType) {
    console.log("Synthese.callService", args);
    args["SERVICE"] = serviceName;

    // TODO: make json the default?
    var params = {
      url: this.URL,
      data: args,
      dataType: dataType
    };

    return $.ajax(params);
  },

  ieFix: function() {
    // Workaround for IE 7 and below.
    // It does two things:
    // 1) Fix IE overflow (Based on script from Remy Sharp http://remysharp.com/2008/01/21/fixing-ie-overflow-problem/)
    // 2) Adds a class of "ie7OrLess" to the body so that we can hide the tables used
    //    for keeping the header column fixed on the left. (because IE7 doesn't support
    //    setting cell width unless the table is in fixed layout mode).
    if (!window.attachEvent || (document.documentMode && document.documentMode > 7))
      return;

    document.body.className = (document.body.className ? " " : "") + "ie7OrLess";

    // find every element to test
    var all = document.getElementsByTagName('*'), i = all.length;
    
    // fast reverse loop
    while (i--) {
      var el = all[i];
      // if the scrollWidth (the real width) is greater than
      // the visible width, then apply style changes
      if (el.scrollWidth > el.offsetWidth) {
        el.style['paddingBottom'] = '20px';
        el.style['overflowY'] = 'hidden';
      }
    }
  }

};

return Synthese;

});
