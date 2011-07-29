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
      place: this.$("#origin_place_txt")
    };
    this.arrival = {
      city: this.$("#destination_city_txt"),
      place: this.$("#destination_place_txt")
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
