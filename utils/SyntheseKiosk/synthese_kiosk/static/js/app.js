/**
 * Synthese Kiosk Administration interface.
 *
 * @author Sylvain Pasche
 */


function ConfigViewModel() {
  this.kioskName = ko.observable();
  this.syntheseUrl = ko.observable();
  this.adminPassword = ko.observable();
  this.secretKey = ko.observable();
    this.availableBrowsers = ["firefox", "chrome", "opera", "xulrunner", "custom"];
  this.browser = ko.observable();
  this.browserPath = ko.observable();
  this.browserArgs = ko.observable();
  this.displayNames = ko.observable();
  this.offlineCacheDir = ko.observable();
  this.debug = ko.observable();

  var self = this;

  this.displayNamesArray = ko.computed(function() {
    if (!self.displayNames())
      return [];
    return self.displayNames().split(",").filter(function(display) {
      return display;
    }).map(function(displayName, index) {
      return {
        index: index,
        label: displayName
      }
    });
  });

  this._loadConfigObj = function(config) {
    console.log("config", config);
    self.kioskName(config.kiosk_name);
    self.syntheseUrl(config.synthese_url);
    self.adminPassword(config.admin_password);
    self.secretKey(config.secret_key);
    self.browser(config.browser);
    self.browserPath(config.browser_path);
    self.browserArgs(config.browser_args.join(","));
    self.displayNames(config.displays.join(","));
    self.offlineCacheDir(config.offline_cache_dir);
    self.debug(config.debug);
  };

  this.loadConfig = function() {

    $.getJSON("/get_config").then(
      function(config) {
        self._loadConfigObj(config);
      },
      function() {
        alert("Failed to fetch config");
      }
    );
  };

  this.saveConfig = function() {
    var configObj = ko.toJS(self);
    var config = {
      kiosk_name: configObj.kioskName,
      synthese_url: configObj.syntheseUrl,
      admin_password: configObj.adminPassword,
      secret_key: configObj.secretKey,
      browser: configObj.browser,
      browser_path: configObj.browserPath,
      browser_args: configObj.browserArgs.split(",").filter(function(d) {
        return d;
      }),
      displays: configObj.displayNames.split(",").filter(function(d) {
        return d;
      }),
      offline_cache_dir: configObj.offlineCacheDir,
      debug: configObj.debug
    };
    $.post("/set_config", {
        data: JSON.stringify(config),
      }, "json"
    ).then(function(config) {
      self._loadConfigObj(config);
    }, function() {
      alert("Failed to save config");
    });
  };
};


function StatusViewModel() {

  this.online = ko.observable("unknown");
  this.logs = ko.observable();

  var self = this;
  this.refresh = function() {
    console.log("refresh");
    $.getJSON("/get_status").then(function(status) {
      console.log("status", status);

      self.online(status.online ? "online" : "offline");

      self.logs(status.logs);

    }, function() {
      alert("Failed to get status");
    })

  };

};


function ViewModel() {
  this.config = new ConfigViewModel();
  this.status = new StatusViewModel();
}

var App = {
  init: function() {
    this.viewModel = new ViewModel();
    this.viewModel.config.loadConfig();
    this.viewModel.status.refresh();
    ko.applyBindings(this.viewModel);
  }
};

$(App.init());
