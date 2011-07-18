var StopSelector = Backbone.View.extend({

  template: $.template(null, [
    '<h1>',
    '  <a href="#" class="close">fermer</a>',
    '  <span class="selectStop"></span>',
    '</h1>',
    '<div class="cityBrowser"></div>'
  ].join('\n')),

  events: {
    "click .close": "close",
  },

  initialize: function(options) {
    this.initTranslations();

    this.cityBrowserView =  new CityBrowser({
      mapOptions: options.mapOptions,
      noMapAutoRender: true,
    });

    var self = this;
    this.cityBrowserView.syntheseMap.bind("all", function(eventName, arg) {
      self.trigger(eventName, arg);
    });

    this.render();
  },

  initTranslations: function() {

    // English
    OpenLayers.Util.applyDefaults(OpenLayers.Lang.en, {
      'selectAStop': 'Select a stop',
    });

    // French
    OpenLayers.Util.applyDefaults(OpenLayers.Lang.fr, {
      'selectAStop': 'Sélectionnez un arrêt',
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
  },

});
