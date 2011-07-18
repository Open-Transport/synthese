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

  callService: function(serviceName, args, dataType) {
    console.log("Synthese.callService", args);
    args["SERVICE"] = serviceName;

    // TODO: make json the default?
    var params = {
      url: this.URL,
      data: args,
      dataType: dataType,
    };
    
    return $.ajax(params);
  },
};
