define([
  "map/js/SyntheseMap",
  "backbone"
], function(SyntheseMap) {


/**
 * Extension of SyntheseMap which will automatically fetch the stops under a
 * given zoom level.
 */
var CityBrowserMap = OpenLayers.Class(SyntheseMap, {

  initialize: function(mapId, options) {
    _.extend(this, Backbone.Events);
    SyntheseMap.prototype.initialize.apply(this, arguments);

    var locStorage = window.localStorage || {};
    if (this.urlOptions.debug || locStorage.debug)
      $(".debug").show();
  },

  /**
   * Return the zoom level from which (included) the stops are fetched. 
   */
  getMinimumFetchZoomLevel: function() {
    // Use a larger value with IE7 which has performance issues when there are
    // many vector objects.
    if (jQuery.browser.msie && parseInt(jQuery.browser.version) <= 7)
      return 14;
    return 11;
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
    var minimumFetchZoomLevel = this.getMinimumFetchZoomLevel();

    this.stopsLayer.styleMap.styles["default"].context.getDisplay = function(feature) {
      var zoom = feature.layer.map.getZoom();
      if (zoom <= minimumFetchZoomLevel)
        return "none";
      return "";
    }

    var strategy = new MinZoomBBOXStrategy({
      minZoom: minimumFetchZoomLevel
    });
    this.stopsLayer.strategies = [strategy];
    strategy.setLayer(this.stopsLayer);
    strategy.activate();
  },

  onStopSelected: function(stopFeature) {
    this.trigger("stopSelected", stopFeature);
  }

});

return CityBrowserMap;

});
