define([
  "backbone"
], function() {

// Utils

var FrameView = Backbone.View.extend({
  FRAME_LOAD_TIMEOUT_S: 20,

  initialize: function(options) {
    this.kv = options.kioskView;
    this.name = options.name;
    this.el = document.createElement("iframe");
    options.container.appendChild(this.el);
  },

  onFrameLoaded: function(dfd) {
    var html = this.html();

    if (/service.*unavailable/i.test(html) ||
        /failed to connect/i.test(html) ||
        /404 Not Found/i.test(html) ||
        // Polipo error message
        /Connection refused/i.test(html) ||
        /502 disconnected operation/i.test(html)) {
      console.log("Detected page with failure: " + this.url);
      this.kv.callKiosk("force_offline", this.url);
      dfd.reject();
    } else {
      var nobreak =  html.replace(/(\r\n|\n|\r)/gm, " ");
      var message = ("debug" + ":" + nobreak.substr(0, 100) + "....." + nobreak.substr(nobreak.length-100));
      this.kv.callKiosk("message", message);
    }
    dfd.resolve();
  },

  loadUrl: function(url) {
    this.url = url;
    console.log("loadUrl " + url);
    var dfd = $.Deferred();
    var self = this;

    var loadTimeout = setTimeout(function() {
      console.log("timeout when loading url");
      $(self.el).unbind("load");
      dfd.reject();
    }, this.FRAME_LOAD_TIMEOUT_S * 1000);

    $(this.el).one("load", function() {
      clearTimeout(loadTimeout);
      self.onFrameLoaded(dfd);
    });

    this.el.src = url;
    return dfd;
  },

  refresh: function() {
    return this.loadUrl(this.url);
  },

  html: function() {
    try {
      return this.el.contentDocument.documentElement.innerHTML;
    } catch (e) {
      // Document access might be denied if not on the same domain.
      return Date.now();
    }
  },

});


var KioskView = Backbone.View.extend({

  events: {
    "click .refresh-now": "refresh",
  },

  initialize: function(options) {
    var self = this;
    var paramString = decodeURIComponent(location.hash.substring(1));
    try {
      params = JSON.parse(paramString);
      if(this.kioskName != undefined) {
        this.kioskName = params.kiosk;
      } else {
        this.kioskName = "UNDEFINED";
      }
      this.targetUrl = params.target_url;
      this.refreshTimeout = params.refresh_timeout;
      this.stopped = false;
    } catch (e) {
      this.stopped = true;
      this.error("Unable to parse params: " + e);
      return;
    }
    this.initFrames();
    this.loadConfig();
  },

  updateStatus: function() {
    var status = this.$(".status");
    if (!status.hasClass("visible"))
      return;

    this.$(".status-config").text(JSON.stringify(this.config));
  },

  toggleStatus: function() {
    var status = this.$(".status");
    status.toggleClass("visible");

    this.updateStatus();
  },

  clearTimeouts: function() {
    if (this.refreshTimeoutID)
      clearTimeout(this.refreshTimeoutID);
  },

  loadConfig: function() {
    var self = this;

    loadComplete = $.when(
          self.getFrame("page0").loadUrl(self.targetUrl),
          self.getFrame("page1").loadUrl(self.targetUrl));

    loadComplete.always(function() {
        self.refresh();
      });

    return;
  },

  initFrames: function() {
    this.frames = {};
    ["page0", "page1"].forEach(function(frameName) {
      this.frames[frameName] = new FrameView({
        container: document.getElementById("framesContainer"),
        name: frameName,
        kioskView: this,
      });

    }, this);
  },

  error: function(msg) {
    this.$(".errorMsg").text(msg)
    this.$(".error").show();
  },

  getFrame: function(frameName) {
    if (this.frames[frameName])
      return this.frames[frameName];

    if (frameName == "activePage") {
      return this.getFrame(this.currentFrame);
    }
    if (frameName == "hiddenPage") {
      if (this.currentFrame == "page0")
        return this.getFrame("page1");
      return this.getFrame("page0");
    }
    return null;
  },

  showFrame: function(frameName) {
    if (this.currentFrame == frameName)
      return;
    this.currentFrame = frameName;
    $("#framesContainer > *:not(:first-child)").hide();
    $(this.getFrame(frameName).el).show();
  },

  callKiosk: function(method, args) {
    $.ajax({
      url: "http://localhost:5000/display_rpc",
      type: "POST",
      data: {
        display: this.kioskName,
        method: method,
        args: args,
      }
    });
  },

  refresh: function() {
    var self = this;
    console.log("__Refreshing frame");

    var hiddenPage = this.getFrame("hiddenPage");
    var activePage = this.getFrame("activePage");

    hiddenPage.refresh().then(
      function() {
        console.log("refresh");
        if (!activePage) {
          console.log("no active page, showing hidden page");
          self.showFrame(hiddenPage.name);
        } else if (activePage.html() != hiddenPage.html()) {
          console.log("detected page change, showing hidden page");
          self.showFrame(hiddenPage.name);
        } else {
          console.log("no change");
        }

      }, function() {

      }).always(function() {
        if (self.stopped)
          return;

        console.log("scheduling next refresh in " + self.refreshTimeout + "s");
        if (self.refreshTimeoutID)
          clearTimeout(self.refreshTimeoutID);
        self.refreshTimeoutID = setTimeout(function() {
          self.refresh()
        }, self.refreshTimeout * 1000)
      });
  },
});

var kv = new KioskView({
  el: document.body,
});

window.onerror = function(e) {
  kv.error("JS error detected" + e);
}

});