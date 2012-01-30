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
  this.availableBrowsers = ['firefox', 'chrome'];
  this.browser = ko.observable();
  this.displayNames = ko.observable();
  this.debug = ko.observable();

  var self = this;

  this._loadConfigObj = function(config) {
    console.log("config", config);
    self.kioskName(config.kiosk_name);
    self.syntheseUrl(config.synthese_url);
    self.adminPassword(config.admin_password);
    self.secretKey(config.secret_key);
    self.browser(config.browser);
    self.displayNames(config.displays.join(","));
    self.debug(config.debug);
  };

  this.loadConfig = function() {

    $.getJSON('/get_config').then(
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
      displays: configObj.displayNames.split(","),
      debug: configObj.debug
    };
    $.post('/set_config', {
        data: JSON.stringify(config),
      }, 'json'
    ).then(function(config) {
      self._loadConfigObj(config);
    }, function() {
      alert("Failed to save config");
    });
  };
};


function StatusViewModel() {

  this.online = ko.observable('unknown');
  this.logs = ko.observable();

  var self = this;
  this.refresh = function() {
    console.log("refresh");
    $.getJSON("/get_status").then(function(status) {
      console.log("status", status);

      self.online(status.online ? 'online' : 'offline');

      self.logs(status.logs);

    }, function() {
      alert("Failed to get status");
    })

  };

};


var App = {
  init: function() {
    console.log("init");


    this.configViewModel = new ConfigViewModel();
    ko.applyBindings(this.configViewModel, $('#configuration').get(0));
    this.configViewModel.loadConfig();

    this.statusViewModel = new StatusViewModel();
    ko.applyBindings(this.statusViewModel, $('#status').get(0));
    this.statusViewModel.refresh();


  }
};

$(App.init());
