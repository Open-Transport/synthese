define([
  "core/js/backbone"
], function() {


var FrameView = Backbone.View.extend({
  FRAME_LOAD_TIMEOUT_S: 20,

  initialize: function(options) {
    this.kv = options.kioskView;
    this.kv.log("init frame", options);
    this.name = options.name;
    this.el = document.createElement("iframe");
    options.container.appendChild(this.el);
  },

  onFrameLoaded: function(dfd) {
    var html = this.html();

    if (/service.*unavailable/i.test(html) ||
        /failed to connect/i.test(html) ||
        // Polipo error messages
        /Connection refused/i.test(html) ||
        /502 disconnected operation/i.test(html)) {
      this.kv.warn("Detected page with failure: " + this.url);
      dfd.reject();
    }
    dfd.resolve();
  },

  loadUrl: function(url, cacheBust) {
    this.url = url;
    var dfd = $.Deferred();
    var self = this;

    var loadTimeout = setTimeout(function() {
      self.kv.warn("timeout when loading frame");
      $(self.el).unbind("load");
      dfd.reject();
    }, this.FRAME_LOAD_TIMEOUT_S * 1000);

    $(this.el).one("load", function() {
      clearTimeout(loadTimeout);
      self.onFrameLoaded(dfd);
    });

    if (cacheBust)
      url += "?" + Date.now();
    this.el.src = url;
    return dfd;
  },


  refresh: function() {
    return this.loadUrl(this.url, true);
  },

  html: function() {
    try {
      return this.el.contentDocument.documentElement.innerHTML;
    } catch (e) {
      // Document access might be denied if not on the same domain.
      return Date.now();
    }
  },

})

var KioskView = Backbone.View.extend({

  events: {
    "click .refresh-now": "refresh",
  },

  initialize: function(options) {
    this.params = JSON.parse(unescape(location.hash.substring(1)));
    this.debug = this.params.debug && this.params.debug != "0";
    if (this.debug) {
      $("body").addClass("debug");
    }
    if (!this.params.display) {
      this.error("Missing display variable");
      return;
    }

    this.loadConfig();

    var self = this;
    self.stopped = false;
    var H_KEY_CODE = "h".charCodeAt(0);
    var S_KEY_CODE = "s".charCodeAt(0);
    $(window).keypress(function(event) {
      if (event.which == H_KEY_CODE) {
        self.stopped = true;
        self.error("Stop key pressed: disabling refresh.")
      }
      if (event.which == S_KEY_CODE) {
        self.toggleStatus();
      }
    });
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

  loadConfig: function() {
    var self = this;

    function _parseConfig(config) {
      self.log("got config", config);

      self.config = config.defaults;
      if (!config.displays[self.params.display]) {
        self.error("Display '" + self.params.display + "' not found in config");
        return;
      }
      $.extend(self.config, config.displays[self.params.display]);
      self.log("display config", self.config);

      self.initFrames();
    }

    $.ajax({
      url: "/kiosk/config.json",
      dataType: "json",
    }).error(function() {
      self.error("Error fetching or parsing config")
    }).success(function(config) {
      // Try to load it again with cache: false, in order to have the latest
      // version. Might fail if offline.
      $.ajax({
        url: "/kiosk/config.json",
        dataType: "json",
        cache: false,
      }).error(function() {
        _parseConfig(config);
      }).success(function(config) {
        _parseConfig(config);
      });
    });
  },

  initFrames: function() {
    this.frames = {};
    ["page0", "page1", "fallback"].forEach(function(frameName) {
      this.frames[frameName] = new FrameView({
        container: document.getElementById("framesContainer"),
        name: frameName,
        kioskView: this,
      });

    }, this);

    this.lastSuccess = Date.now();
    var self = this;

    $.when(
      this.getFrame("fallback").loadUrl(this.config.fallbackUrl),
      this.getFrame("page0").loadUrl(this.config.url),
      this.getFrame("page1").loadUrl(this.config.url))
      .always(function() {
        self.refresh();
      });
    this.showFrame("fallback");
  },

  error: function(msg) {
    if (!this.debug) {
      this.fatal(msg);
      return;
    }
    this.$(".errorMsg").text(msg)
    this.$(".error").show();
  },


  getFrame: function(frameName) {
    if (this.frames[frameName])
      return this.frames[frameName];

    if (frameName == "activePage") {
      if (this.currentFrame == "fallback")
        return null;
      return this.getFrame(this.currentFrame);
    }
    if (frameName == "hiddenPage") {
      if (this.currentFrame == "fallback" || this.currentFrame == "page0")
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
        display: this.params.display,
        method: method,
        args: args,
      }
    });
  },

  _message: function(level, args_, consoleLevel) {
    var args = [];
    for (var i = 0; i < args_.length; i++) {
      args[i] = args_[i];
    }
    var message = (level + ":" + (this.params.display || "<no display>") + ": " +
      Array.prototype.join.call(args, " "));
    if (this.debug || level != "debug")
      this.callKiosk("message", message);

    if (!window.console)
      return;
    consoleLevel = consoleLevel || level;
    try {
      console[consoleLevel].apply("", args);
    } catch (e) {
      // IE/webkit throw when calling console.log.apply.
      console[consoleLevel](args.join(" "));
    }
  },

  log: function() {
    this._message("debug", arguments);
  },

  info: function() {
    this._message("info", arguments);
  },

  warn: function() {
    this._message("warn", arguments);
  },

  fatal: function() {
    this._message("fatal", arguments, "error");
  },

  refresh: function() {
    this.log("_refreshing");

    var hiddenPage = this.getFrame("hiddenPage"),
      activePage = this.getFrame("activePage");

    var self = this;
    hiddenPage.refresh().then(
      function() {
        self.lastSuccess = Date.now();
        self.log("refresh ok");
        if (!activePage) {
          self.log("no active page, showing hidden page");
          self.showFrame(hiddenPage.name);
        } else if (activePage.html() != hiddenPage.html()) {
          self.log("detected page change, showing hidden page");
          self.showFrame(hiddenPage.name);
        } else {
          self.log("no change");
        }

      }, function() {
        var failureDurationSeconds = (Date.now() - self.lastSuccess) / 1000;
        self.warn("refresh failed. Failure duration: ", failureDurationSeconds);
        if (failureDurationSeconds >= self.config.fallbackTimeout)
          self.showFrame("fallback");
      }).always(function() {
        if (self.stopped)
          return;

        self.log("scheduling next refresh in " + self.config.refreshTimeout + "s");
        if (self.refreshTimeout)
          clearTimeout(self.refreshTimeout);
        self.refreshTimeout = setTimeout(function() {
          self.refresh()
        }, self.config.refreshTimeout * 1000)
      });
  },
});

var kv = new KioskView({
  el: document.body,
});

window.onerror = function(e) {
  kw.error("JS error detected" + e);
}

});
