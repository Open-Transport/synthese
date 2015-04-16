define([
  "core/js/Synthese",
  "map/js/CityBrowserMap"
], function(Synthese, CityBrowserMap) {

// === Models ===

var City = Backbone.Model.extend({
});


var CityCollection = Backbone.Collection.extend({
  model: City,

  comparator: function(city) {
    return city.get("sortName");
  }
});


var CitySelectorModel = Backbone.Model.extend({
  initialize: function(options) {
    this.mapSRID = options.mapSRID;
    this.cities = new CityCollection();
    this.cities.selector = this;

    this.fixedUpCities = {};
  },

  showStartingWith: function(prefix) {
    var prefix = prefix.toLowerCase();
    this.set({prefix: prefix});

    var self = this;

    // TODO: implement a service to return only the cities starting with the letter.
    // Until then, we retrieve all cities and filter from that.

    function removeAccents(string) {
      return string.replace(/[àâä]/gi, "a")
        .replace(/[éèêë]/gi, "e")
        .replace(/[îï]/gi, "i")
        .replace(/[ôö]/gi, "o")
        .replace(/[ùûü]/gi, "u")
    }

    // Returns the name of the city used for sorting and filtering.
    function getCitySortName(cityName) {
      var sortName = removeAccents(cityName).toLowerCase();
      sortName = sortName.replace(/^(la |le |les |l')/, "");
      var firstCharCode = sortName.toLowerCase().charCodeAt(0);
      var aCharCode = "a".charCodeAt(0);
      var zCharCode = "z".charCodeAt(0);
      if (firstCharCode < aCharCode || firstCharCode > zCharCode) {
        console.warn("Special char code", firstCharCode, "for city", cityName);
      }
      return sortName;
    }

    function getCities() {
      if (self.cities.length > 0)
        return self.cities;

      return Synthese.callService("lc", {
        n: 9999,
        at_least_a_stop: 1,
        si: Synthese.siteId,
        output_format: "json",
        srid: self.mapSRID
      }).then(function(json) {
        // Result can be an array or object due to this issue:
        // https://188.165.247.81/issues/10786
        if (!_.isArray(json.cities.city))
          json.cities.city = [json.cities.city];

        // TODO: change once https://188.165.247.81/issues/10786 is fixed.
        self.cities.reset(_.map(json.cities.city, function(jsonCity) {
          return {
            id: jsonCity.city_id,
            name: jsonCity.city_name,
            x: parseFloat(jsonCity.city_x),
            y: parseFloat(jsonCity.city_y),
            sortName: getCitySortName(jsonCity.city_name)
          };
        }));
      });
    }

    $.when(getCities()).done(function() {
      self.cities.each(function(city) {
        city.set({"visible": city.get("sortName").indexOf(prefix) === 0});
      });
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
        json.cities.city
        if (!json.cities.city)
          return self.fixedUpCities[cityName] = null;

        // Result can be an array or object due to this issue:
        // https://188.165.247.81/issues/10786
        var cityObj = json.cities.city;
        if (_.isArray(cityObj))
          cityObj = cityObj[0];

        if (!cityObj.city_name)
          return self.fixedUpCities[cityName] = null;

        return self.fixedUpCities[cityName] = cityObj.city_name;
      });
    }

    var realCityName;
    fixUpCityName(cityName).pipe(function(cityName) {
      if (!cityName)
        return null;
      realCityName = cityName;
      return self.showStartingWith(realCityName.charAt(0));

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
  }
});


// === Views ===

var CityView = Backbone.View.extend({
  tagName: "li",

  events: {
    "click": "select"
  },

  initialize: function() {
    _.bindAll(this, "render", "updateSelected", "updateVisible", "select");
    this.model.bind("change:selected", this.updateSelected);
    this.model.bind("change:visible", this.updateVisible);

    this.model.bind("ensureVisible", _.bind(function() {
      this.el.scrollIntoView();
    }, this));
  },

  render: function() {
    $(this.el).text(this.model.get("name"));
    $(this.el)[this.model.get("visible") ? "show" : "hide"]();
    this.updateSelected();
    return this;
  },

  updateSelected: function() {
    var selected = this.model.get("selected");
    $(this.el)[selected ? "addClass" : "removeClass"]("selected");
    return this;
  },

  updateVisible: function() {
    var visible = this.model.get("visible");
    $(this.el)[visible ? "show" : "hide"]();
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

  addOne: function(city) {
    $(this.el).append(new CityView({model: city}).render().el);
  },

  addAll: function() {
    $(this.el).empty();
    this.collection.each(this.addOne);
  }
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
    "click .letters li": "letterClick"
  },

  initialize: function() {
    _.bindAll(this, "render", "prefixUpdated", "updateEmptyMessage",
        "setLettersSize");

    this.cityList = new CityListView({collection: this.model.cities});
    this.model.bind("change:prefix", this.prefixUpdated);
    this.model.cities.bind("reset", this.updateEmptyMessage);
    this.letterCodes = _.range("A".charCodeAt(0), "Z".charCodeAt(0) + 1);
    this.render();
  },

  render: function() {
    var letters = _.map(this.letterCodes,
      function (charCode) {
        return String.fromCharCode(charCode);
      }
    );
    $.tmpl(this.template, {
      letters: letters
    }).appendTo(this.el);

    this.$('.cityList').replaceWith(this.cityList.el);

    this.setLettersSize();
    $(window).resize(this.setLettersSize);
  },

  setLettersSize: function() {
    var height = $(this.el).height();

    // If not visible or not rendered yet, wait for a bit.
    var self = this;
    if (height == 0 || height == 100) {
      setTimeout(function() {
        self.setLettersSize();
      }, 500);
      return;
    }

    this.$(".letters").show().css("height", height);
    // Without the "+ 1", the last letter is not visible.
    var lineHeight = Math.floor(height / (this.letterCodes.length + 1));
    this.$(".letters li").css("line-height", lineHeight + "px");
  },

  letterClick: function(event) {
    event.stopPropagation();
    event.preventDefault();
    this.model.showStartingWith($(event.target).text().toLowerCase());
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
  }
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

    this.initTranslations();

    var mapClass = options.mapClass || CityBrowserMap;
    this.syntheseMap = new mapClass(null, options.mapOptions);
    this.syntheseMap.onError = _.bind(this.onError, this);
    this.syntheseMap.bind("zoomRequired", this.zoomRequired)

    this.citySelectorModel = new CitySelectorModel({mapSRID: this.syntheseMap.MAP_SRID});
    this.citySelectorView = new CitySelectorView({model: this.citySelectorModel});

    this.citySelectorModel.bind("citySelected", this.citySelected);
    this.citySelectorModel.bind("cityNotFound", this.cityNotFound);
    this.citySelectorModel.bind("error", this.onError);
    this.citySelectorModel.bind("message", this.showMessage);

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
      "cityNotFound": "City not found"
    });

    // French
    OpenLayers.Util.applyDefaults(OpenLayers.Lang.fr, {
      "error": "Une erreur s'est produite :(",
      "cities": "Villes:",
      "processing": "Chargement en cours ...",
      "zoomRequired": "Zoomez ou sélectionnez une ville pour voir tous les arrêts",
      "noCities": "Pas de villes",
      "cityNotFound": "Ville non trouvée"
    });
  },

  resetState: function() {
    this.citySelectorModel.showStartingWith("a");
    if (this.syntheseMap.map)
      this.syntheseMap.zoomToInitialPosition();
  },

  zoomRequired: function(required) {
    this.$(".zoomRequiredMessage")[required ? "fadeIn" : "fadeOut"]();
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

    var map = this.syntheseMap.map;
    var center = new OpenLayers.LonLat(city.get("x"), city.get("y"));
    map.setCenter(center, this.options.cityRecenterZoomLevel || 15);
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
  }
});

return CityBrowser;

});
