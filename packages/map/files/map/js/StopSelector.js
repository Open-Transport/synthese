var StopSelector = Backbone.View.extend({

  // TODO: i18n
  template: $.template(null, [
    '<h1>',
    '  <a href="#" class="close">fermer</a>',
    '  Sélectionnez un arrêt',
    '</h1>',
    '<div class="cityBrowser"></div>'
  ].join('\n')),

  events: {
    "click .close": "close",
  },

  initialize: function(options) {

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

  render: function() {
    $(this.el).empty().addClass("stopSelector");
    $.tmpl(this.template, {}).appendTo(this.el);

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
