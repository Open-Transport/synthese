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
        params[decodeURIComponent(match[1])] = decodeURIComponent(match[2]);
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
  }
};
