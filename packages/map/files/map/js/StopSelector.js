define([
  "map/js/CityBrowser",
  "backbone"
], function(CityBrowser) {

var StopSelectorPopup = Backbone.View.extend({

  template: $.template(null, [
    '<h1>',
    '  <a href="#" class="close">fermer</a>',
    '  <span class="selectStop"></span>',
    '</h1>',
    '<div class="cityBrowser"></div>'
  ].join('\n')),

  events: {
    "click .close": "close"
  },

  initialize: function(options) {
    this.initTranslations();
    this.cityBrowserView =  new CityBrowser(_.defaults({
      noMapAutoRender: true
    }, this.options.cityBrowserOptions));

    var self = this;
    this.cityBrowserView.syntheseMap.bind("all", function(eventName, arg) {
      self.trigger(eventName, arg);
    });

    this.render();
  },

  initTranslations: function() {

    // English
    OpenLayers.Util.applyDefaults(OpenLayers.Lang.en, {
      'selectAStop': 'Select a stop'
    });

    // French
    OpenLayers.Util.applyDefaults(OpenLayers.Lang.fr, {
      'selectAStop': 'Sélectionnez un arrêt'
    });
  },

  render: function() {
    $(this.el).empty().addClass("stopSelector");
    $.tmpl(this.template, {}).appendTo(this.el);

    this.$(".selectStop").text(OpenLayers.i18n("selectAStop"));

    this.$(".cityBrowser").replaceWith(this.cityBrowserView.el);
    this.cityBrowserView.renderMap();
  },

  close: function(event) {
    if (event) {
      event.stopPropagation();
      event.preventDefault();
    }
    this.trigger("close");
    $(this.el).hide();
  },

  show: function() {
    $(this.el).show();
  }

});

// TODO: decouple class from routePlanner implementation.
var StopSelector = Backbone.View.extend({
  initialize: function(options) {
    _.bindAll(this, "mapLinkClick", "stopSelected");

    this.routePlanner = options.routePlanner;
    if (!this.routePlanner) {
      alert("Missing routePlanner parameter");
      // TODO: decouple.
      //this.routePlanner = new RoutePlannerView(options.routePlannerOptions || {});
    }
    this.routePlanner.bind("mapLinkClick", this.mapLinkClick);

    if (options.popupEl) {
      this.popupEl = options.popupEl;
    } else {
      this.popupEl = document.createElement("div");
      document.body.appendChild(this.popupEl);
    }
    $(this.popupEl).addClass("stopSelectorPopup");
  },

  mapLinkClick: function(departure) {
    this.ensureSelectorPopup();
    $(this.selectorPopup).show();

    var city = this.routePlanner.getCity(departure);

    this.selectorPopup.cityBrowserView.setActiveCity(city);
    this.currentDirection = departure;
  },

  ensureSelectorPopup: function() {
    $(this.popupEl).show();
    if (this.selectorPopup)
      return;
    this.selectorPopup = new StopSelectorPopup(_.defaults({
      el: this.popupEl
    }, this.options.popupOptions));
    this.selectorPopup.bind("stopSelected", this.stopSelected);
  },

  stopSelected: function(stopFeature) {
    if (legacyRoutePlannerConfig.routePlannerFormOneField) {
      this.routePlanner.setPlace(
        this.currentDirection, stopFeature.data.city_name + " " + stopFeature.data.stop_name);
      this.routePlanner.setClass(
        this.currentDirection, "stop");
        
      var self = this;
      $(this.selectorPopup.el).fadeOut(null, function() {
        self.selectorPopup.close();
      });
    }
    else {
      this.routePlanner.setCity(
        this.currentDirection, stopFeature.data.city_name);
      this.routePlanner.setPlace(
        this.currentDirection, stopFeature.data.stop_name);

      var self = this;
      $(this.selectorPopup.el).fadeOut(null, function() {
        self.selectorPopup.close();
      });
    }
  }
});

return StopSelector;

});
