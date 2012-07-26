define([
  "backbone"
], function() {

// Utils

// from OpenLayers/Util.js
function urlAppend(url, paramStr) {
  var newUrl = url;
  if (paramStr) {
    var parts = (url + " ").split(/[?&]/);
    newUrl += (parts.pop() === " " ?
      paramStr :
      parts.length ? "&" + paramStr : "?" + paramStr);
  }
  return newUrl;
};


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
      url = urlAppend(url, Date.now());
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

});


var KioskView = Backbone.View.extend({

  events: {
    "click .refresh-now": "refresh",
  },

  GLOBAL_DEFAULTS: {
    refreshTimeout: 2 * 60,
    configRefreshTimeout: 5 * 60,
    fallbackTimeout: 15 * 60,
    fallbackUrl: "/kiosk/default_fallback.html",
    interactive: false,
  },

  initialize: function(options) {
    var paramString = unescape(location.hash.substring(1));
    try {
      this.params = JSON.parse(paramString);
    } catch (e) {
      this.params = {};
      this.error("Unable to parse params: " + e);
      return;
    }
    this.debug = this.params.debug && this.params.debug != "0";
    if (this.debug) {
      $("body").addClass("debug");
    }
    if (!this.params.display) {
      this.error("Missing display variable");
      return;
    }

    var self = this;
    self.stopped = false;
    var H_KEY_CODE = "h".charCodeAt(0);
    var S_KEY_CODE = "s".charCodeAt(0);
    $(window).keypress(function(event) {
      if (event.which == H_KEY_CODE) {
        self.stopped = true;
        self.clearTimeouts();
        self.error("Stop key pressed: disabling refresh.")
      }
      if (event.which == S_KEY_CODE) {
        self.toggleStatus();
      }
    });

    this.initFrames();
    this.loadConfig(true)
      .fail(function(message) {
        self.error(message);
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

  clearTimeouts: function() {
    if (this.refreshTimeout)
      clearTimeout(this.refreshTimeout);
    if (this.refreshConfigTimeout)
      clearTimeout(this.refreshConfigTimeout);
  },

  loadConfig: function(useCache) {
    var self = this;

    var dfd = $.ajax({
      url: "/kiosk_config.json",
      dataType: "json",
      cache: useCache,
    })

    var dfd2 = $.Deferred();
    dfd.then(function(config) {
      if (!useCache) {
        dfd2.resolve(config);
        return;
      }

      // Try to load it again without cache: false, in order to have the latest
      // version. Might fail if offline.
      return $.ajax({
        url: "/kiosk_config.json",
        dataType: "json",
        cache: false,
      }).fail(function() {
        self.log("Failed fetching config without cache, using cached version");
        dfd2.resolve(config);
      }).done(function(config) {
        dfd2.resolve(config);
      });
    }, function(jqXHR, textStatus, errorThrown) {
      dfd2.reject("Error fetching or parsing config: " + textStatus +
        " exception: " + errorThrown);
    });

    dfd2.done(function(config) {
      self.log("got config", config);

      self.config = {};
      $.extend(self.config, self.GLOBAL_DEFAULTS);
      $.extend(self.config, config.defaults);
      if (!config.displays[self.params.display]) {
        self.error("Display '" + self.params.display + "' not found in config");
        return;
      }
      $.extend(self.config, config.displays[self.params.display]);
      self.log("display config", self.config);
      if (JSON.stringify(config) == self.lastConfigString) {
        self.log("Config didn't change since last fetch. Not reloading");
        return;
      }

      self.lastConfigString = JSON.stringify(config);

      self.lastSuccess = Date.now();
      self.clearTimeouts();
      document.body.classList.remove("interactive");
      if (self.config.interactive)
        document.body.classList.add("interactive");

      var loadComplete;

      self.showFrame("fallback");

      if (self.config.interactive) {
        // Load the interactive page in a visible frame. Some applications
        // (e.g OpenLayers) don't like to be loaded in a hidden element.
        loadComplete = self.getFrame("fallback")
          .loadUrl(self.config.fallbackUrl)
          .pipe(function() {
            self.showFrame("page0");
            return self.getFrame("page0").loadUrl(self.config.url);
          });
      } else {
        loadComplete = $.when(
          self.getFrame("fallback").loadUrl(self.config.fallbackUrl),
          self.getFrame("page0").loadUrl(self.config.url),
          self.getFrame("page1").loadUrl(self.config.url));
      }

      loadComplete.always(function() {
        self.refresh();

        self.log("scheduling next config refresh in " + self.config.configRefreshTimeout + "s");
        if (self.refreshConfigTimeout)
          clearTimeout(self.refreshConfigTimeout);
        self.refreshConfigTimeout = setTimeout(function() {
          self.refreshConfig()
        }, self.config.configRefreshTimeout * 1000)
      });
    });

    return dfd2.promise();
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

  refreshConfig: function() {
    var self = this;

    self.log("____Refreshing config");

    if (self.stopped)
      return;

    var dd = this.loadConfig(false)
      .fail(function(message) {
        self.warn("Ignoring error while trying to refresh config: " + message);
      });

    self.log("scheduling next config refresh in " + self.config.configRefreshTimeout + "s");
    if (self.refreshConfigTimeout)
      clearTimeout(self.refreshConfigTimeout);
    self.refreshConfigTimeout = setTimeout(function() {
      self.refreshConfig();
    }, self.config.configRefreshTimeout * 1000)

  },

  refresh: function() {
    if (this.config.interactive)
      return;
    this.log("__Refreshing frame");

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
  kv.error("JS error detected" + e);
}

});
