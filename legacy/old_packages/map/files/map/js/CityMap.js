define([
  "map/js/SyntheseMap",
  "jquery",
  "jquery.tmpl"
], function(SyntheseMap) {

/**
 * Extension of SyntheseMap which displays the stops and lines of a specific
 * city. The city should be passed in a "city" GET parameter.
 */
var CityMap = OpenLayers.Class(SyntheseMap, {

  initialize: function(mapId, options) {
    SyntheseMap.prototype.initialize.apply(this, arguments);

    var locStorage = window.localStorage || {};
    if (this.urlOptions.debug || locStorage.debug)
      $(".debug").show();
  },

  beforeMapInit: function() {
    this.networkId = this.urlOptions.networkId;

    this.cityIdFilter = this.urlOptions.cityId;
    if (this.urlOptions.cityName) {
      var cityTitle = OpenLayers.i18n("cityOf").replace("{name}", this.urlOptions.cityName)
      document.title = cityTitle;
      $("header h1").text(cityTitle);
    }
  },

  initTranslations: function() {
    SyntheseMap.prototype.initTranslations.apply(this, arguments);

    // English
    OpenLayers.Util.applyDefaults(OpenLayers.Lang.en, {
      "cityOf": "City of {name}"
    });

    // French
    OpenLayers.Util.applyDefaults(OpenLayers.Lang.fr, {
      "cityOf": "Commune de {name}"
    });
  },

  onError: function(message, arguments) {
    $("#errorMessage").text(message + " (" + arguments[2] + ")");
  },

  afterPTFeaturesAdded: function() {
    console.log("CityMap::afterPTFeaturesAdded");

    // Draw the list of lines on the left

    var idToLine = {};
    _.map(this.stopsLayer.features, function(feature) {
      _.map(feature.attributes.lines, function(line) {
        idToLine[line.id] = line;
      });
    });

    var linesList = _.values(idToLine);
    linesList = _(linesList).chain().map(function(line) {
      line.displayName = line.line_short_name;
      if (line.name)
        line.displayName += ", " + line.name;
      line.color =  line.line_color && ("rgb" + line.line_color) || "transparent";
      return line;
    }).sortBy(function(line) {
      return line.displayName;
    }).value();

    $("#linesTemplate").tmpl(linesList).appendTo("#linesList");

    // When a line is selected, update visibility of the lines and stops to
    // display active lines and only stops which belong to a least one selected
    // line respectively.

    var self = this;

    // TODO: use an attribute and update the StyleMap instead?
    function setFeatureVisible(feature, visible) {
      if (visible)
        delete feature.style;
      else
        feature.style = {display: "none"};
    }

    function updatePTFeatures() {
      var selectedLineIds = {};
      $("#linesList input:checked").each(function(index, checkbox) {
        // Warning: don't use jQuery.data to fetch the lineid attribute,
        // otherwise it will be converted to a number with its value truncated
        // to JavaScript number precision.
        selectedLineIds[$(checkbox).attr("data-lineid")] = true;
      });

      _.map(self.linesLayer.features, function(line) {
        setFeatureVisible(line, selectedLineIds[line.attributes.id]);
      });
      self.linesLayer.redraw();

      _.map(self.stopsLayer.features, function(stop) {
        var visible = _.any(stop.attributes.lines, function(line) {
          return selectedLineIds[line.id];
        });

        setFeatureVisible(stop, visible);
      });
      self.stopsLayer.redraw();
    }

    $("#linesList").change(updatePTFeatures);

    $(".checkUncheck").click(function(event) {
      event.stopPropagation();
      event.preventDefault();

      $("#linesList input[type=checkbox]").each(function(index, checkbox) {
        checkbox.checked = $(event.target).data("action") == "checkAll";
      });
      updatePTFeatures();
    });
  }
});

return CityMap;

});
