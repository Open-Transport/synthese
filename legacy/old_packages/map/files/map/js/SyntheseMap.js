define([
  "core/js/Synthese",
  "underscore",
  "OpenLayers_fr",
  "map/vendor/OpenLayers_maptypepanel/MapType",
  "map/vendor/OpenLayers_maptypepanel/MapTypePanel"
], function(Synthese) {

/**
 * Class that manages an OpenLayer map to display public transportation objects
 * (lines and stops).
 */
var SyntheseMap = OpenLayers.Class({
  MAP_SRID: "900913",

  map: null,
  linesLayer: null,
  stopsLayer: null,
  layerSwitcher: null,
  // TODO: use Synthese object instead.
  urlOptions: null,
  mapOptions: {},

  /**
   * Option where to recenter the map on the first display.
   * Array of two or three strings: x, y, [zoom level].
   * Coordinates are in lat/lon.
   */
  center: null,
  /**
   * Visibility of the layer switcher for stops and lines.
   */
  showLayerSwitcher: true,

  /**
   * Default language to use.
   */
  defaultLanguage: "fr",
  /**
   * Id of the network to display stops and lines from. Required in order
   * to draw lines and stops, unless you override the fetch of these objects.
   */
  networkId: null,
  /**
   * Optional filter on this city id: shows only the stops on that city
   * and the lines that crosses it.
   */
  cityIdFilter: null,

  initialize: function(mapId, options) {
    OpenLayers.Util.extend(this, options);
    if (this.defaultLanguage)
      OpenLayers.Lang.setCode(this.defaultLanguage);

    this.urlOptions = OpenLayers.Util.getParameters(window.location.href);

    this.initTranslations();
    if (mapId)
      this.setMapId(mapId);
  },

  setMapId: function(mapId) {

    var mapDiv;

    if (typeof(mapId) == "string") {
      mapDiv = document.getElementById(mapId);
    } else {
      // The doc says we can pass a DOM node, but that's not true :/
      // Construct an id in this case.
      mapDiv = mapId;
      mapId = OpenLayers.Util.createUniqueID("OpenLayers.Map_");
      mapDiv.setAttribute("id", mapId);
    }

    // Hack for IE7: The map is not shown if the map div has an absolute
    // position and no explicit height.
    // Setting the height "manually" fixes the issues. It should be harmless
    // to other browsers.
    // (See http://lists.osgeo.org/pipermail/openlayers-users/2010-July/018686.html)
    mapDiv.style.height = $(mapDiv).height();

    this.beforeMapInit();

    this.map = new OpenLayers.Map(mapId, OpenLayers.Util.extend({
      projection: "EPSG:" + this.MAP_SRID
    }, this.mapOptions));

    this.linesLayer = new OpenLayers.Layer.Vector("lines", {
      layerId: "lines",
      styleMap: this.getLinesStyleMap(),
      rendererOptions: {zIndexing: true}
    });

    this.stopsLayer = new OpenLayers.Layer.Vector("stops", {
      layerId: "stops",
      styleMap: this.getStopsStyleMap()
    });

    this.map.addLayers(this.getBackgroundLayers().concat([this.linesLayer, this.stopsLayer]));

    this.map.addControl(new OpenLayers.Control.MapTypePanel());

    var self = this;

    var select = new OpenLayers.Control.SelectFeature(this.stopsLayer, OpenLayers.Util.extend({
      hover: true,
      highlightOnly: true,
      onSelect: function() {
        self.onStopSelected.apply(self, arguments);
      },
      // Hack to get the onSelect callback to be called even if hover is true.
      clickFeature: function() {
        var oldHover = this.hover;
        this.hover = false;
        OpenLayers.Control.SelectFeature.prototype.clickFeature.apply(this, arguments);
        this.hover = oldHover;
      }
    }, this.selectFeatureOptions || {}));
    this.map.addControl(select);
    select.activate();

    if (this.showLayerSwitcher)
      this.initPTLayerSelector();

    if (this.urlOptions.showLines == "0")
      this.linesLayer.setVisibility(false);

    // Debugging controls
    var el;
    if (el = document.getElementById("mousePosLatLon"))
      this.map.addControl(new OpenLayers.Control.MousePosition({
        element: el,
        numDigits: 3,
        displayProjection: new OpenLayers.Projection("EPSG:4326")
      }));

    if (el = document.getElementById("mousePosSphericalMercator"))
      this.map.addControl(new OpenLayers.Control.MousePosition({
        element: el,
        numDigits: 1,
        displayProjection: new OpenLayers.Projection("EPSG:900913")
      }));

    if (document.getElementById("zoomLevel")) {
      var control = new OpenLayers.Control();
      OpenLayers.Util.extend(control, {
        autoActivate: true,
        activate: function() {
          if (OpenLayers.Control.prototype.activate.apply(this, arguments)) {
            this.map.events.register("zoomend", this, this.redraw);
            this.redraw();
            return true;
          }
          return false;
        },
        deactivate: function() {
          if (OpenLayers.Control.prototype.deactivate.apply(this, arguments)) {
            this.map.events.unregister("zoomend", this, this.redraw);
            return true;
          }
          return false;
        },
        redraw: function() {
          $("#zoomLevel").text(this.map.zoom);
        }
      });
      this.map.addControl(control);
    }

    this.zoomToInitialPosition();
    this.afterMapInit();

    this.networkId = this.networkId || this.urlOptions.networkId;
    if (this.networkId)
      this.addPTFeatures();
  },

  /**
   * Zoom to a point and zoom given in the center map parameter or query string.
   * See documentation for the center attribute.
   */
  zoomToInitialPosition: function() {
    var center = this.center || this.urlOptions.center
    if (!center)
      return;
    var zoom = center[2] && parseFloat(center[2]);
    this.map.setCenter(
      new OpenLayers.LonLat(parseFloat(center[0]), parseFloat(center[1])).
        transform(new OpenLayers.Projection("EPSG:4326"),
          this.map.getProjectionObject()),
      zoom);
  },

  /**
   * Override this method to do operations before the map is initialized.
   * You should do work here instead of in initialize if you need to use
   * OpenLayers.i18n() for instance.
   */
  beforeMapInit: function() {
  },


  /**
   * Return an array of layers used for the background.
   */
  getBackgroundLayers: function() {
    var backgroundLayers = []

    backgroundLayers.push(new OpenLayers.Layer.OSM(OpenLayers.i18n("OSM")));

    var GOOGLE_NUM_ZOOM_LEVELS = 22;
    backgroundLayers.push(
      new OpenLayers.Layer.Google(
        OpenLayers.i18n("googleSatelliteLayer"), {
          type: google.maps.MapTypeId.SATELLITE,
          numZoomLevels: GOOGLE_NUM_ZOOM_LEVELS
        }
      )
    );

    backgroundLayers.push(
      new OpenLayers.Layer.Google(
        OpenLayers.i18n("googleDefaultLayer"), {
          numZoomLevels: GOOGLE_NUM_ZOOM_LEVELS
        }
      )
    );

    return backgroundLayers;
  },

  /**
   * Override to add your application specific translations.
   */
  initTranslations: function() {
    // English
    OpenLayers.Util.applyDefaults(OpenLayers.Lang.en, {
      "googleSatelliteLayer": "Earth",
      "googleDefaultLayer": "Google"
    });

    // French
    OpenLayers.Util.applyDefaults(OpenLayers.Lang.fr, {
      "googleSatelliteLayer": "Photo",
      "googleDefaultLayer": "Google",
      "lines": "Lignes",
      "stops": "Arrêts"
    });
  },

  getLinesStyleMap: function() {
    // Doc:
    // http://trac.osgeo.org/openlayers/wiki/Styles
    // http://docs.openlayers.org/library/feature_styling.html
    // http://osgeo-org.1803224.n2.nabble.com/Vector-labels-at-different-zoom-levels-td4892404.html

    var context = {
      getStrokeWidth: function(feature) {
        var zoom = feature.layer.map.getZoom();
        var width;
        if (zoom <= 12) {
          width = 2;
        } else if (zoom <= 14) {
          width = 4;
        } else {
          width = 6;
        }
        if (feature.attributes.background)
          width += 2;
        return width;
      },
      getStrokeColor: function(feature) {
        var attrs = feature.attributes;
        if (attrs.background)
          return "black";
        // default fall back color
        if (!attrs.line_color)
          return "gray";
        return "rgb" + attrs.line_color;
      }
    };

    var style = new OpenLayers.Style({
      strokeColor: "${getStrokeColor}",
      strokeWidth: "${getStrokeWidth}",
      graphicZIndex: 5
    }, {
      context: context,
      rules: [
        new OpenLayers.Rule({
          filter: new OpenLayers.Filter.Comparison({
            type: OpenLayers.Filter.Comparison.EQUAL_TO,
            property: "color",
            value: "0"
          }),
          symbolizer: {
            display: "none"
          }
        }),
        new OpenLayers.Rule({
          filter: new OpenLayers.Filter.Comparison({
            type: OpenLayers.Filter.Comparison.EQUAL_TO,
            property: "background",
            value: true
          }),
          symbolizer: {
            graphicZIndex: 5
          }
        }),
        new OpenLayers.Rule({
          // apply this rule if no others apply
          elseFilter: true,
          symbolizer: {
          }
        })
      ]
    });
    return new OpenLayers.StyleMap(style);
  },

  getStopsStyleMap: function() {
    var defaultContext = {
      getPointRadius: function(feature) {
        var zoom = feature.layer.map.getZoom();
        if (zoom <= 12)
          return 3;
        if (zoom <= 14)
          return 4;
        return 6;
      },
      getLabel: function(feature) {
        var zoom = feature.layer.map.getZoom();
        if (zoom <= 14)
          return "";
        return feature.attributes.name;
      },
      getDisplay: function(feature) {
        var zoom = feature.layer.map.getZoom();
        if (zoom <= 11)
          return "none";
        return "";
      }
    };

    var defaultStyle = new OpenLayers.Style({
      strokeColor: "red",
      strokeOpacity: 0.5,
      strokeWidth: 2,
      fillColor: "blue",
      fillOpacity: 0.9,
      pointRadius: "${getPointRadius}",
      fontWeight: "bold",
      label: "${getLabel}",
      labelAlign: "lt",
      labelXOffset: "5",
      labelYOffset: "-5",
      display: "${getDisplay}",
      fontSize: "small"
    }, {
      context: defaultContext
    });

    var selectContext = OpenLayers.Util.extend({}, defaultContext);
    OpenLayers.Util.extend(selectContext, {
      getLabel: function(feature) {
        return feature.attributes.name;
      }
    });

    var selectStyle = OpenLayers.Util.extend({}, defaultStyle.defaultStyle);
    OpenLayers.Util.extend(selectStyle, {
      display: "",
      pointRadius: 6,
      fillColor: "red",
      strokeColor: "black",
      labelXOffset: "15",
      fontColor: "#222",
      fontSize: "18px"
    });

    selectStyle = new OpenLayers.Style(selectStyle, {
      context: selectContext
    });

    return new OpenLayers.StyleMap({
      'default': defaultStyle,
      'select': selectStyle
    });
  },

  /**
   * Simple checkbox based layer selector for stops and lines.
   */
  initPTLayerSelector: function() {

    var layerSelectorHtml = [
      '<div class="ptLayerSelector">',
      '  <div>',
      '    <label><input checked type="checkbox" data-layername="stops">',
      '      ' + OpenLayers.i18n('stops'),
      '    </label>',
      '    <label><input checked type="checkbox" data-layername="lines">',
      '      ' + OpenLayers.i18n('lines'),
      '    </label>',
      '  </div>',
      '</div>'
    ].join("\n");

    $(layerSelectorHtml).appendTo(this.map.div);

    var self = this;

    this.map.events.on({
      "changelayer": function() {
        _.each(self.map.layers, function(layer) {
          $(".ptLayerSelector input[data-layername=" + layer.name + "]",
              self.map.div).prop("checked", layer.getVisibility());
        });
      }
    });

    $(".ptLayerSelector", this.map.div).change(function() {
      $(this).find('input').each(function() {
        var layerName = $(this).data("layername");
        var layers = self.map.getLayersByName(layerName);
        if (layers.length != 1)
          return;
        layers[0].setVisibility($(this).prop("checked"));
      });
    });
  },


  /**
   * Override this method to alter the map after it is initialized. For instance
   * to recenter it to a region.
   */
  afterMapInit: function() {
  },


 /**
  * Override this to do something when a stop is selected.
  */
  onStopSelected: function(feature) {
    console.log("onStopSelected", feature);
  },

  /**
   * Override this to display error messages.
   */
  onError: function(message, arguments) {
    console.warn("Map error", message, arguments);
  },

  /**
   * Helper to call a Synthese service. It returns a deferred
   */
  callSynthese: function(serviceName, args) {
    var self = this;
    return Synthese.callService(serviceName, args).pipe(null, function(res) {
      self.onError("Ajax error while calling Synthese service", arguments);
      return res;
    });
  },

  /**
   * Copy a DOM node attributes to the given JS object.
   */
  copyAttributes: function(node, target) {
    $.each($(node).get(0).attributes, function(index, a) {
      target[a.name] = a.value;
    });
  },

  /**
   * Fetch line features. Returns a Deferred.
   * 
   * filter could contain a rollingStockFilterId 
   */
  fetchLines: function(filter, withStops) {

    var args = {
      srid: this.MAP_SRID,
      of: "xml",
      ni: this.networkId,
      og: "wkt"
    };
    // Coming soon for LinesListFunction2 on the server
    if (filter.rollingStockFilterId)
      args.rsf = filter.rollingStockFilterId;
    if (withStops)
      args.os = 1;  

    var linesXHR = this.callSynthese("LinesListFunction2", args);

    var self = this;
    return linesXHR.pipe(function(linesDocument) {

      var lineFeatures = [];
      var wktFormat = new OpenLayers.Format.WKT();

      $("line", linesDocument).each(function(index, line) {
      
        // TODO: Delete once rollingStockFilterId implemented on server
        if (filter.rollingStockId && $("transportMode", line).attr("id") == filter.rollingStockId)
          return;
        
        // Skip lines that have no stop in the active city
        if (self.cityIdFilter &&
            !$("stopArea[city_id=" + self.cityIdFilter + "]", line).length)
          return;

        var wkt = $("geometry", line).attr("wkt");
        var features = wktFormat.read(wkt);
        if (features === undefined)
          return;

        var attributes = {};
        self.copyAttributes(line, attributes);

        // Add a background line for each line that will be styled separately.
        var bgFeatures = [];

        features = $.map(features,
          function(f) {
            if (f === undefined)
              return null;
            f.attributes = attributes;
            var bgFeature = f.clone();
            bgFeature.attributes.background = true;
            bgFeatures.push(bgFeature);
            return f;
          }
        );

        lineFeatures.push.apply(lineFeatures, bgFeatures);
        lineFeatures.push.apply(lineFeatures, features);
      });

       return lineFeatures;
     }).promise();
  },

  addLines: function() {
    var self = this;

    return this.fetchLines({}, true).
      pipe(function(lineFeatures) {
        self.linesLayer.addFeatures(lineFeatures);
      }
    );
  },

  /**
   * Fetch stop features. Returns a Deferred.
   *
   * filter should contains at least a cityId (integer) or
   * bounds (OpenLayers.Bounds) property.
   */
  fetchStops: function(filter, withLines) {

    var args = {
      srid: this.MAP_SRID
    };
    if (filter.cityId)
      args.roid = filter.cityId;
    if (filter.bounds)
      args.bbox = filter.bounds.toBBOX();
    if (withLines)
      args.ol = 1;

    var stopsXHR = this.callSynthese("StopAreasListFunction", args);
    var self = this;

    return stopsXHR.pipe(function(stopsDocument) {
      var stopFeatures = [];

      // Ignore stops which are in the middle of the sea. This can happen when
      // importing data from Lambert93 with zero coordinates.
      var latLonProj = new OpenLayers.Projection("EPSG:4326");
      var invalidBounds = new OpenLayers.Bounds(-15, -6, 7, 2);

      $("stopArea", stopsDocument).each(function(index, stopArea) {
        var attributes = {};
        self.copyAttributes(stopArea, attributes);

        if (attributes.x === undefined || attributes.y === undefined) {
          return;
        }

        var point = new OpenLayers.Geometry.Point(
          parseFloat(attributes.x),
          parseFloat(attributes.y)
        );

        var pointLatLon = OpenLayers.Projection.transform(
          point.clone(), self.map.getProjectionObject(), latLonProj);

        if (invalidBounds.contains(pointLatLon.x, pointLatLon.y)) {
          console.warn("Ignoring stops with invalid coordinates", attributes);
          return;
        }

        if (withLines) {
          attributes.lines = [];
          $("line", stopArea).each(function(index, line) {
            var lineAttributes = {};
            self.copyAttributes(line, lineAttributes);
            attributes.lines.push(lineAttributes);
          });
        }

        stopFeatures.push(
          new OpenLayers.Feature.Vector(
            point, attributes
          )
        );
      });

      return stopFeatures;
    }).promise();
  },

  addStops: function() {
    var self = this;

    return this.fetchStops({cityId: this.cityIdFilter}, true).
      pipe(function(stopFeatures) {
        self.stopsLayer.addFeatures(stopFeatures);

        if (self.cityIdFilter && self.stopsLayer.features.length > 0) {
          // FIXME: a bug happens sometimes where the extent is wrong and we
          // recenter on Africa/Europe.
          console.log(
            "zooming to stopslayer extent: ",
            self.stopsLayer.getDataExtent().toString(),
            "n features", self.stopsLayer.features.length);
          self.map.zoomToExtent(self.stopsLayer.getDataExtent());
        }
      }
    );
  },

  /**
   * Override this method to add the stops and lines features differently.
   */
  addPTFeatures: function() {
    // TODO: these methods should only build the features, so that they can
    // be overridden without them knowing about the specifics for putting the
    // features into the lines/stops layers.
    // FIXME: temporary until https://188.165.247.81/issues/11210 is fixed
    var self = this;
    $.when(
      this.addLines()
    ).pipe(function() {
      return self.addStops();
    }).done($.proxy(this.afterPTFeaturesAdded, this));
  },

  /**
   * Called after the lines and stops features have been added to their
   * respective layers.
   */
  afterPTFeaturesAdded: function() {
    console.log("afterPTFeaturesAdded");
  }

});

return SyntheseMap;

});