define([
  "core/js/Synthese",
  "backbone",
  "jquery.placeholder"
], function(Synthese, __Backbone, __$) {

$(function() {
  $('input, textarea').placeholder();
});

// XXX rename to RoutePlannerFormView in order to have other objects not related to the form?
var RoutePlannerView = Backbone.View.extend({

  events: {
    "click .mapLink": "mapLinkClick",
    "change #noTransfer": "updateMaxDepth"
  },

  initialize: function(options) {
    _.bindAll(this, "mapLinkClick", "updateMaxDepth");

    this.departure = {
      city: this.$("#origin_city_txt"),
      place: this.$("#origin_place_txt"),
      class_: this.$("#origin_class_txt")
    };
    this.arrival = {
      city: this.$("#destination_city_txt"),
      place: this.$("#destination_place_txt"),
      class_: this.$("#destination_class_txt")
    };

    this.$(".mapLink").show();

    // Init form values
    // FIXME: this won't work if the form is posted.
    this.$("input#noTransfer").prop(
        "checked", Synthese.queryStringOptions.md == "0");
  },

  getDirectionObj: function(departure) {
    return this[departure ? "departure" : "arrival"];
  },
  getCity: function(departure) {
    return this.getDirectionObj(departure).city.val();
  },
  setCity: function(departure, cityName) {
    return this.getDirectionObj(departure).city.val(cityName);
  },
  getPlace: function(departure) {
    return this.getDirectionObj(departure).city.val();
  },
  setPlace: function(departure, placeName) {
    return this.getDirectionObj(departure).place.val(placeName);
  },
  getClass: function(departure) {
    return this.getDirectionObj(departure).class_.val();
  },
  setClass: function(departure, className) {
    return this.getDirectionObj(departure).class_.val(className);
  },
  
  mapLinkClick: function(event) {
    event.stopPropagation();
    event.preventDefault();
    this.trigger("mapLinkClick", $(event.target).parents(".departure").length > 0);
  },

  updateMaxDepth: function(event) {
    var noTransfer = this.$("input#noTransfer").prop("checked");
    this.$("input[name=md]").val(noTransfer ? "0" : "9999");
  }

});

// Legacy interface -----------------------------------------------------------

/**
 * Initialize the route planner form to enable auto completion on the inputs.
 * Synthese.init(SITE_ID) needs to be called first in order to set the site id.
 */
function initAutoCompletions() {
  if (document.getElementsByTagName) {
    var inputElements = document.getElementsByTagName("input");
    for (i = 0; inputElements[i]; i++)
      inputElements[i].setAttribute("autocomplete", "off");
  }

  initAutoCompleteForm(
    document.forms[legacyRoutePlannerConfig.routePlannerFormName],
    document.getElementById("submitButton"));
  if(!legacyRoutePlannerConfig.routePlannerFormOneField) {
    initAutoCompleteField(0, document.getElementById("origin_city_txt"), null, function (city, place) {
      return Synthese.URL + '?fonction=lc&n=10&at_least_a_stop=1&si=' + Synthese.siteId + '&t=' + city + '';
    }, document.getElementById("origin_place_txt"), null, null);
    initAutoCompleteField(1, document.getElementById("origin_place_txt"), document.getElementById("origin_city_txt"), function (city, place) {
      return Synthese.URL + '?ct=' + city + '&fonction=lp&n=10&si=' + Synthese.siteId + '&t=' + place + '';
    }, null, null, null);
    initAutoCompleteField(2, document.getElementById("destination_city_txt"), null, function (city, place) {
      return Synthese.URL + '?fonction=lc&n=10&at_least_a_stop=1&si=' + Synthese.siteId + '&t=' + city + '';
    }, document.getElementById("destination_place_txt"), null, null);
    initAutoCompleteField(3, document.getElementById("destination_place_txt"), document.getElementById("destination_city_txt"), function (city, place) {
      return Synthese.URL + '?ct=' + city + '&fonction=lp&n=10&si=' + Synthese.siteId + '&t=' + place + '';
    }, null, null, null);
    document.getElementById("origin_city_txt").focus();
  }
  else {
    initAutoCompleteField(1, document.getElementById("origin_place_txt"), document.getElementById("origin_class_txt"), function (place, place2) {
      return Synthese.URL + '?fonction=places_list&number=3&si=' + Synthese.siteId + '&text=' + place + '';
    }, null, null, null);
    initAutoCompleteField(3, document.getElementById("destination_place_txt"), document.getElementById("destination_class_txt"), function (place, place2) {
      return Synthese.URL + '?fonction=places_list&number=3&si=' + Synthese.siteId + '&text=' + place + '';
    }, null, null, null);
    document.getElementById("origin_place_txt").focus();
  }
  if (document.getElementById("fh")) {
    addEvent(document.getElementById("fh"), "mouseover", mouse_event_handler);
    addEvent(document.getElementById("fh"), "click", mouse_click_handler);
  }
}

return {
  RoutePlannerView: RoutePlannerView,
  initAutoCompletions: initAutoCompletions
};

});
