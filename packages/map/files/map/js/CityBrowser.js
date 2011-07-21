/**
 * TODO: doc.
 */

(function() {

})();

// TODO: put in anonymous namespace and export mininum stuff.

// === Models ===

var City = Backbone.Model.extend({
});


var CityCollection = Backbone.Collection.extend({
  model: City,
});


var CitySelectorModel = Backbone.Model.extend({
  initialize: function() {
    this.cities = new CityCollection();
    this.cities.selector = this;

    this.allCities = null;
    this.fixedUpCities = {};
  },

  fetchStartingWith: function(prefix) {

    var prefix = prefix.toLowerCase();
    this.set({prefix: prefix});

    var self = this;

    // TODO: implement a service to return only the cities starting with the letter.
    // Until then, we retrieve all cities and filter from that.

    return $.Deferred(function(dfd) {

      if (self.allCities)
        return dfd.resolve(self.allCities);

      console.log("Fetching all cities");

      Synthese.callService("lc", {
        n: 9999,
        at_least_a_stop: 1,
        si: Synthese.siteId,
        output_format: "json",
      }).then(function(json) {

        // TODO: change once https://188.165.247.81/issues/10786 is fixed.
        self.allCities = json.cities.city;
        dfd.resolve(self.allCities);
      }, dfd.reject);

    }).pipe(function(cities) {
      return _(cities).chain().select(function(city) {
        return city.city_name.toLowerCase().indexOf(prefix) === 0;
      }).value();
    }).done(function(cities) {
      self.cities.reset(_(cities).map(function(city) {
        return {
          id: city.city_id,
          name: city.city_name,
        };
      }));
    }).fail(function() {
      console.warn("Error while fetching cities", arguments);
      self.trigger("error", "Error while fetching cities");
    });
  },

  selectByName: function(cityName) {

    var self = this;

    // Call the Cities List service to get the city name from the string the
    // user may have typed.
    function fixUpCityName(cityName) {
      if (self.fixedUpCities[cityName] !== undefined) {
        return $.Deferred(function(dfd) {
          dfd.resolve(self.fixedUpCities[cityName]);
        }).promise();
      }

      // TODO: maybe factorize on Synthese.
      return Synthese.callService("lc", {
        // FIXME: there is a bug in the lexical matcher service, where the first results are not returned unless
        // we ask for a larger number of results.
        n: 10,
        at_least_a_stop: 1,
        si: Synthese.siteId,
        t: cityName,
        output_format: "json"
      }).pipe(function(json) {
        var fixedUpCity;
        if (!json.cities.city || !json.cities.city.city_name) {
          fixedUpCity = null;
        } else {
          fixedUpCity = json.cities.city.city_name;
        }
        return self.fixedUpCities[cityName] = fixedUpCity
      });
    }

    var realCityName;
    fixUpCityName(cityName).pipe(function(cityName) {
      if (!cityName)
        return null;
      realCityName = cityName;
      return self.fetchStartingWith(realCityName[0]);

    }, function() {
      self.trigger("error", "Error while fetching city");
    }).pipe(function() {
      if (!realCityName) {
        self.trigger("cityNotFound");
        return;
      }

      var city = self.cities.detect(function(city) {
        return city.get('name') === realCityName;
      });
      self.select(city);
      city.trigger("ensureVisible");
    });
  },

  select: function(city) {
    this.cities.each(function(c) {
      c.set({"selected": c === city});
    });
    this.trigger("citySelected", city);
  },

  // TODO
  // getSelectedCity
});


//=== Views ===

var CityView = Backbone.View.extend({
  tagName: "li",

  events: {
    "click": "select",
  },

  initialize: function() {
    _.bindAll(this, "render", "updateSelected", "select");
    this.model.bind("change:selected", this.updateSelected);

    this.model.bind("ensureVisible", _.bind(function() {
      this.el.scrollIntoView();
    }, this));
  },

  render: function() {
    $(this.el).text(this.model.get("name"));
    this.updateSelected();
    return this;
  },

  updateSelected: function() {
    var selected = this.model.get("selected");
    $(this.el)[selected ? "addClass" : "removeClass"]("selected");
    return this;
  },

  select: function() {
    console.log("select", this);
    this.model.collection.selector.select(this.model);
  }
});

var CityListView = Backbone.View.extend({

  tagName: "ul",
  className: "cityList",

  initialize: function() {
    _.bindAll(this, "render", "addOne", "addAll");

    this.collection.bind("add", this.addOne);
    this.collection.bind("reset", this.addAll);

    this.render();
  },

  render: function() {
    if (this.collection.length == 0)
      $
    return this;
  },

  addOne: function(city) {
    $(this.el).append(new CityView({model: city}).render().el);
  },

  addAll: function() {
    $(this.el).empty();
    this.collection.each(this.addOne);
  },
});

var CitySelectorView = Backbone.View.extend({

  className: 'citySelector',

  template: $.template(null, [
    '<ul class="letters">',
    '  {{each letters}}',
    '    <li data-prefix="${$value}">${$value}</li>',
    '  {{/each}}',
    '</ul>',
    '<div class="cities">',
    '  <div class="citiesLabel"></div>',
    '  <div class="noCities"></div>',
    '  <ul class="cityList">',
    '  </ul>',
    '</div>'
  ].join('\n')),

  events: {
    "click .letters li": "letterClick",
  },

  initialize: function() {
    _.bindAll(this, "render", "prefixUpdated", "updateEmptyMessage");

    this.cityList = new CityListView({collection: this.model.cities});
    this.model.bind("change:prefix", this.prefixUpdated);
    this.model.cities.bind("reset", this.updateEmptyMessage);
    this.render();
  },

  render: function() {
    var letters = _.map(_.range("A".charCodeAt(0), "Z".charCodeAt(0) + 1),
      function (charCode) {
        return String.fromCharCode(charCode);
      }
    );
    $.tmpl(this.template, {
      letters: letters,
    }).appendTo(this.el);

    this.$('.cityList').replaceWith(this.cityList.el);
  },

  letterClick: function(event) {
    event.stopPropagation();
    event.preventDefault();
    this.model.fetchStartingWith($(event.target).text().toLowerCase());
  },

  prefixUpdated: function() {
    this.$(".letters .selected").removeClass("selected");
    var prefix = this.model.get('prefix');
    this.$(".letters [data-prefix=" + prefix.toUpperCase() + "]").addClass("selected");
  },

  updateEmptyMessage: function() {
    if (this.model.cities.length == 0) {
      this.$(".noCities").show().html(OpenLayers.i18n("noCities"));
    } else {
      this.$(".noCities").hide();
    }
  },
});

var CityBrowserMap = OpenLayers.Class(SyntheseMap, {

  initialize: function(mapId, options) {
    _.extend(this, Backbone.Events);
    SyntheseMap.prototype.initialize.apply(this, arguments);

    var localStorage = window.localStorage || {};
    if (this.urlOptions.debug || localStorage.debug)
      $(".debug").show();
  },

  afterMapInit: function() {
    var self = this;

    // A protocol for fetching the stops on a given bbox.
    var StopsProtocol = OpenLayers.Class(OpenLayers.Protocol, {
      read: function(options) {
        console.log("StopsProtocol read", options);
        var bounds = options.filter.value;

        self.fetchStops({bounds: bounds}, true).
          then(function(stopFeatures) {
            var resp = new OpenLayers.Protocol.Response({requestType: "read"});
            resp.features = stopFeatures;
            options.callback.call(options.scope, resp);
        });
      }
    });

    // Extension of BBOX strategy to avoid fetching features if the zoom level
    // is not at least a given value. It also fires events on the map to
    // announce when zoom in is required to fetch the features.
    var MinZoomBBOXStrategy = OpenLayers.Class(OpenLayers.Strategy.BBOX, {
      invalidBounds: function(mapBounds) {
        console.log("invalidBounds called", mapBounds);
        var invalid = OpenLayers.Strategy.BBOX.prototype.invalidBounds.apply(this, arguments);
        // Our data is still in the requested bounds, nothing to do.
        if (!invalid) {
          self.trigger("zoomRequired", false);
          return false;
        }

        var zoom = self.map.getZoomForExtent(mapBounds);
        // Bounds are invalid but we have an acceptable zoom level.
        if (zoom >= this.minZoom) {
          self.trigger("zoomRequired", false);
          return true;
        }

        // Zooming is required, don't fetch the data yet.
        self.trigger("zoomRequired", true);
        return false;
      }
    });

    this.stopsLayer.protocol = new StopsProtocol();

    var MIN_ZOOM = 11;

    var strategy = new MinZoomBBOXStrategy({
      minZoom: MIN_ZOOM,
    });
    this.stopsLayer.strategies = [strategy];
    strategy.setLayer(this.stopsLayer);
    strategy.activate();
  },

  onStopSelected: function(stopFeature) {
    this.trigger("stopSelected", stopFeature);
  },

});


var CityBrowser = Backbone.View.extend({

  template: $.template(null, [
    '<div class="citySelector"></div>',
    '<div class="message loadingMessage"></div>',
    '<div class="message errorMessage">',
    '  <div></div>',
    '  <div class="detail"></div>',
    '</div>',
    '<div class="map">',
    '  <div class="zoomRequiredMessage"></div>',
    '</div>'
  ].join('\n')),

  initialize: function(options) {
    _.bindAll(this, "zoomRequired", "citySelected", "cityNotFound", "showMessage", "onError");

    this.cityExtentCache = {};

    this.initTranslations();

    this.citySelectorModel = new CitySelectorModel();
    this.citySelectorView = new CitySelectorView({model: this.citySelectorModel});

    this.citySelectorModel.bind("citySelected", this.citySelected);
    this.citySelectorModel.bind("cityNotFound", this.cityNotFound);
    this.citySelectorModel.bind("error", this.onError);
    this.citySelectorModel.bind("message", this.showMessage);

    var mapClass = options.mapClass || CityBrowserMap;
    this.syntheseMap = new mapClass(null, options.mapOptions);
    this.syntheseMap.onError = _.bind(this.onError, this);
    this.syntheseMap.bind("zoomRequired", this.zoomRequired)

    this.render();

    this.resetState();
  },

  initTranslations: function() {

    // English
    OpenLayers.Util.applyDefaults(OpenLayers.Lang.en, {
      "error": "An error occurred :(",
      "cities": "Cities:",
      "processing": "Loading...",
      "zoomRequired": "Zoom or select a city to see all the stops",
      "noCities": "no cities",
      "cityNotFound": "City not found",
    });

    // French
    OpenLayers.Util.applyDefaults(OpenLayers.Lang.fr, {
      "error": "Une erreur s'est produite :(",
      "cities": "Villes:",
      "processing": "Chargement en cours ...",
      "zoomRequired": "Zoomez ou sélectionnez une ville pour voir tous les arrêts",
      "noCities": "Pas de villes",
      "cityNotFound": "Ville non trouvée",
    });
  },

  resetState: function() {
    this.citySelectorModel.fetchStartingWith("a");
    if (this.syntheseMap.map)
      this.syntheseMap.zoomToInitialPosition();
  },

  zoomRequired: function(required) {
    this.$(".zoomRequiredMessage")[required ? "fadeIn" : "fadeOut"]();
  },

  getCityExtent: function(cityId) {
    var self = this;

    if (this.cityExtentCache[cityId]) {
      return $.Deferred(function(dfd) {
        dfd.resolve(self.cityExtentCache[cityId]);
      }).promise();
    }

    return this.syntheseMap.fetchStops({cityId: cityId}).pipe(function(stopFeatures) {
      if (stopFeatures.length == 0)
        return null;

      var extent = new OpenLayers.Bounds();
      _.each(stopFeatures, function(stopFeature) {
        extent.extend(stopFeature.geometry);
      });

      return self.cityExtentCache[cityId] = extent;
      // for testing loading message.
      /*
      return $.Deferred(function(dfd) {
        setTimeout(function() {
          dfd.resolveWith(self, [extent]);
        }, 10000);
      });
      */
    });
  },

  showMessage: function(message) {
    this.$(".loadingMessage").text(OpenLayers.i18n(message)).fadeIn();
  },

  showPendingMessage: function(dfd, message) {

    this.showMessage(message);
    var self = this;
    dfd.always(function() {
      self.$(".loadingMessage").hide();
    });
    dfd.fail(function(message) {
      self.onError(message);
    });
  },

  citySelected: function(city) {
    var self = this;

    var dfd = this.getCityExtent(city.get("id"));
    this.showPendingMessage(dfd, "processing");
    dfd.done(function(extent) {
      if (extent)
        self.syntheseMap.map.zoomToExtent(extent);
    });
  },

  cityNotFound: function() {
    this.resetState();
    this.showMessage("cityNotFound");
  },

  renderMap: function() {
    if (this.syntheseMap.mapId)
      return;
    this.syntheseMap.setMapId(this.$(".map").get(0));
  },

  render: function() {
    $(this.el).empty().addClass("cityBrowser");
    $.tmpl(this.template, {}).appendTo(this.el);

    this.$(".citySelector").replaceWith(this.citySelectorView.el);

    this.$(".zoomRequiredMessage").text(OpenLayers.i18n("zoomRequired"));
    this.$(".citiesLabel").text(OpenLayers.i18n("cities"));

    if (!this.options.noMapAutoRender)
      this.renderMap();
  },

  onError: function(message) {
    this.$(".errorMessage div:first-child").text(OpenLayers.i18n("error")).show();
    this.$(".errorMessage").click(function() {
      // XXX why doesn't toggleClass work here?
      $(this).addClass("showDetail");
    });
    var detail = this.$(".errorMessage .detail");
    detail.text(detail.text() + "," + (message && message.toString()));
  },

  setActiveCity: function(cityName) {
    if (cityName) {
      this.citySelectorModel.selectByName(cityName);
      return;
    }

    this.resetState();
  },
});
