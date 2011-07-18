var RoutePlannerView = Backbone.View.extend({

  initialize: function(options) {
    var formName = options.formName || "ri"

    this.$form = $("form[name=" + formName + "]");
    this.departure = {
      city: this.$form.find("#origin_city_txt"),
      place: this.$form.find("#origin_place_txt"),
    };
    this.arrival = {
      city: this.$form.find("#destination_city_txt"),
      place: this.$form.find("#destination_place_txt"),
    };

    var self = this;
    $(".mapLink").show().click(function(event) {
      event.stopPropagation();
      event.preventDefault();
      self.trigger("mapLinkClick", $(event.target).parents(".departure").length > 0);
    });

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

});
