window.app.Gis = function(options){

    /**
     * Property: div
     * {String} ID of div in which we will create the map
     */
    this.div = options.div;

    /**
     * Property: type
     * {CONSTANT} kind of map we want here: one of app.Gis.type
     */
    this.type = options.type;

    /**
     * Property: id
     * {integer} routing solution ID
     */
    this.id = options.id;
    
    /**
     * Property: map
     * {OpenLayers.Map} map object
     */
    this.map = null;
    
    this.epsg900913 = new OpenLayers.Projection("EPSG:900913");
    this.epsg4326 = new OpenLayers.Projection("EPSG:4326");

    this.init();
};

window.app.Gis.type = {
    START: 0,
    ROUTES: 1,
    STOP: 2
};

window.app.Gis.prototype = {
    
    /**
     * Method: createMap
     * Creates the map
     */
    createMap: function() {
        
        var map = new OpenLayers.Map(this.div, {
            projection: this.epsg900913,
            displayProjection: this.epsg4326,
            units: "m",
            theme: null,
            numZoomLevels: 18,
            maxResolution: 156543.0339,
            controls: [
                new OpenLayers.Control.Navigation(),
                new OpenLayers.Control.PanZoom(),
                new OpenLayers.Control.Attribution()
            ],
            // restrictedExtent on Toulouse ?
            maxExtent: new OpenLayers.Bounds(-20037508.34, -20037508.34,
                                              20037508.34, 20037508.34)
        });
        
        return map;
    },
    
    /**
     * Method: createLayers
     *     creates all layers
     */
    createLayers: function() {
  
        var layers = [];
        
        var getTileURL = function(bounds) {
            var res = this.map.getResolution();
            var x = Math.round((bounds.left - this.maxExtent.left) / (res * this.tileSize.w));
            var y = Math.round((this.maxExtent.top - bounds.top) / (res * this.tileSize.h));
            var z = this.map.getZoom();
            var limit = Math.pow(2, z);
            if (y < 0 || y >= limit) {
                return OpenLayers.Util.getImagesLocation() + "blank.gif";
            } else {
                x = ((x % limit) + limit) % limit;
                var path = z + "/" + x + "/" + y + "." + this.type;
                if (this.url instanceof Array) {
                    return this.selectUrl(path, this.url) + path;
                } else {
                    return this.url + path;
                }
            }
        };
        
        layers.push(
            new OpenLayers.Layer.TMS(
                "OSM",
                [
                    "http://a.tile.openstreetmap.org/",
                    "http://b.tile.openstreetmap.org/",
                    "http://c.tile.openstreetmap.org/"
                ],
                {
                    type: 'png',
                    getURL: getTileURL,
                    displayOutsideMaxExtent: true,
                    //numZoomLevels: 19,
                    attribution: '<a href="http://www.openstreetmap.org/">OpenStreetMap</a>'
                }
            )
        );
        
        layers.push(
            new OpenLayers.Layer.Google("Photo", {
                sphericalMercator: true,
                type: G_SATELLITE_MAP
            })
        );
        
        layers.push(
            new OpenLayers.Layer.Google("Google", {
                sphericalMercator: true,
                type: G_NORMAL_MAP // [G_PHYSICAL_MAP | G_NORMAL_MAP | G_SATELLITE_MAP | G_HYBRID_MAP]
            })
        );

        layers.push(
            new OpenLayers.Layer.Vector("BlackRoutes", {
                styleMap: new OpenLayers.StyleMap({
                    "default": new OpenLayers.Style({
                        strokeColor: "#000000",
                        strokeOpacity: 1,
                        strokeWidth: 6,
                        strokeLinecap: "round", // [butt | round | square]
                        strokeDashstyle: "${strokeDashStyle}" // [dot | dash | dashdot | longdash | longdashdot | solid]
                    })
                })
            })
        );

        layers.push(
            new OpenLayers.Layer.Vector("Routes", {
                styleMap: new OpenLayers.StyleMap({
                    "default": new OpenLayers.Style({
                        strokeColor: "${color}",
                        strokeOpacity: 1,
                        strokeWidth: 4,
                        strokeLinecap: "round", // [butt | round | square]
                        strokeDashstyle: "${strokeDashStyle}" // [dot | dash | dashdot | longdash | longdashdot | solid]
                    })
                })
            })
        );
        
        layers.push( // icons for bus, metro lines ...
            new OpenLayers.Layer.Vector("Icons", {
                styleMap: new OpenLayers.StyleMap({
                    "default": new OpenLayers.Style({
                        externalGraphic: "${meanIcon}",
                        graphicXOffset: -27,
                        graphicYOffset: -9,
                        graphicWidth: 16,
                        graphicHeight: 16
                    })
                })
            })
        );
        
        layers.push(
            new OpenLayers.Layer.Vector("Labels", {
                styleMap: new OpenLayers.StyleMap({
                    "default": new OpenLayers.Style({
                        externalGraphic: "${icon}",
                        graphicXOffset: -21,
                        graphicYOffset: -9,
                        graphicWidth: 42,
                        graphicHeight: 18,
                        label : '${name}',
                        fontColor: "#ffffff", //"${color}",
                        fontSize: "13px",
                        fontFamily: "Arial, sans-serif", //"Courier New, Courier, monospace", 
                        fontWeight: "bold",
                        labelAlign: "cm" //  "lb" "cm" "rt"
                    })
                })
            })
        );

        layers.push(
            new OpenLayers.Layer.Vector("Points", {
                styleMap: new OpenLayers.StyleMap({
                    "default": new OpenLayers.Style({
                        externalGraphic: "${picto}",
                        graphicXOffset: -10.5,
                        graphicYOffset: -34,
                        graphicWidth: 21,
                        graphicHeight: 34,
                        cursor: 'pointer'
                    }),
                    "select": new OpenLayers.Style({
                        graphicWidth: 21,
                        graphicHeight: 36,
                        graphicXOffset: -10.5,
                        graphicYOffset: -36
                    })
                })
            })
        );
        
        return layers;
    },
    
    /**
     * Method: addDeferedControls
     *   Adds controls to the map object
     * 
     */
    addDeferedControls: function() {
        var map = this.map;
        
        map.addControl(new OpenLayers.Control.MapTypePanel());
        
        var control = new OpenLayers.Control.SelectFeature(
            this.getLayerNamed("Points"), {
                highlightOnly: true,
                hover: true,
                eventListeners: { 
                    "featurehighlighted": OpenLayers.Function.bind(function(options) {
                        var feature = options.feature;
                        if (feature.timer) {
                            clearTimeout(feature.timer);
                            feature.timer = null;
                        } else {
                            this.displayPopup(options.feature);
                        }
                    }, this),
                    "featureunhighlighted": OpenLayers.Function.bind(function(options) {
                        options.feature.timer = setTimeout(OpenLayers.Function.bind(this.destroyPopup, this), 500, options.feature);
                    }, this)
                }
            }
        );
        map.addControl(control);
        control.activate();
    },
    
    /**
     * Method: destroyPopup
     * removes and destroys popup attached to a feature
     */
    destroyPopup: function(feature) {
        if (feature && feature.popup) {
            feature.popup.destroy(); 
            feature.popup = null;
        }
        feature.timer = null;
    },
    
    /**
     * Method: displayPopup
     *     Displays a Popup on feature
     */
    displayPopup: function(feature) {
        
        var centroid = feature.geometry.getCentroid();
        
        var popup = new OpenLayers.Popup.AutoSizedFramedCloud("popup", 
            new OpenLayers.LonLat(centroid.x, centroid.y), 
            null,
            feature.attributes.name,
            null,
            false
        );
        feature.popup = popup;
        this.map.addPopup(popup);
        return popup;
    },
    
    /**
     * Method: refresh
     *   called when solution ID has changed
     *   and one wants to refresh the maps
     */
    refresh: function(id) {
        this.id = id;
        this.getLayerNamed("Icons").destroyFeatures();
        this.getLayerNamed("Labels").destroyFeatures();
        this.getLayerNamed("BlackRoutes").destroyFeatures();
        this.getLayerNamed("Routes").destroyFeatures();
        this.getLayerNamed("Points").destroyFeatures();
        this.displayRoutes();
    },
    
    /**
     * Method: displayRoutes
     *   Adds vector features to the map
     * 
     */
    displayRoutes: function() {
        var type = app.Gis.type;
        var points = app.bootstrap.routes[this.id].points;
        var nbpoints = points.length;
        
        // pre-processing:
        points[0].type = 'start';
        points[nbpoints-1].type = 'stop';
        
        // draw points and lines according to map type:
        switch (this.type) {
            case type.START:
                // just display start point
                this.drawPoint(points[0]); 
                break;
                
            case type.ROUTES:
                for (var len=nbpoints, i=len-2; i>=0; i--) {
                    this.goFromTo({
                        start: points[i],
                        stop: points[i+1],
                        displayFinalPoint: (i==len-2)
                    });
                }
                break;
                
            case type.STOP:
                // just display final point
                this.drawPoint(points[nbpoints-1]);
                break;
                
            default:
                app.displayError("[app.Gis.displayRoutes] unknown map type");
                break;
        }
        
        // Zoom to data extent + 15%
        var b = this.getLayerNamed("Points").getDataExtent();
        b.extend(this.getLayerNamed("Routes").getDataExtent());
        
        this.zoomToBounds(b);
    },
    
    /**
     * Method: goFromTo
     *   Adds vector features to the map, corresponding to one foot trip 
     *   between one point and another
     */
    goFromTo: function(options) {
    
        // display point A
        var p1 = this.drawPoint(options.start);
        
        // display point B if requested
        var p2;
        if (options.displayFinalPoint) {
            p2 = this.drawPoint(options.stop);
        } else {
            p2 = this.positionToLonLat(options.stop.position);
        }
        
        // display Line
        if(options.start.wkt)
          this.drawLine({wkt: options.start.wkt, lineRef: options.start.means});
        else
          this.drawLine({from: p1, to: p2, lineRef: options.start.means});
    },
    
    /**
     * Method: positionToLonLat
     *   converts a position string (lonlat 4326 string) into an OpenLayers.LonLat 900913
     */
    positionToLonLat: function(position) {
        var ll = OpenLayers.LonLat.fromString(position);
        ll.transform(this.epsg4326, this.epsg900913);
        return new OpenLayers.Geometry.Point(ll.lon, ll.lat);
    },
    
    /**
    * Method: zoomToBounds
    * sets map center and zooms to a BoundingBox
    */
    zoomToBounds: function(bounds) {
        if (bounds.getHeight()+bounds.getWidth() === 0) {
            this.map.setCenter(bounds.getCenterLonLat(), app.setup.zoomLevelForPoint);
        } else {
            this.map.zoomToExtent(bounds.scale(1.15));
        }
    },
    
    /**
    * Method: zoomToThisPlace
    * sets map center and zooms to a connection zone (for instance)
    * takes an array of "point strings" as arguments (with at least one),
    * eg: ['1.35475,43.62688','1.38702,43.64676'],...
    */
    zoomToThisPlace: function(points) {
        if (!points[0]) {
            return;
        }
        var b = OpenLayers.Bounds.fromString(points[0]+','+points[0]);
        b.transform(this.epsg4326, this.epsg900913);
        for (var i=1, len=points.length; i<len; i++) {
            b.extend(this.positionToLonLat(points[i]));
        }
        this.zoomToBounds(b);
    },
    
    /**
     * Method: drawPoint
     *   Adds one point vector features to the map
     *   TODO: add styling
     */
    drawPoint: function(point) {
    
        var layer = this.getLayerNamed("Points");
        
        var p = this.positionToLonLat(point.position);
        
        var fileName;
        var name = '<b>'+point.name+'</b>';
        
        switch (point.type) {
            case 'start':
                fileName = 'google_chart/flag.png';
                break;
            case 'stop':
                fileName = 'google_chart/location.png';
                break;
            default: // 'connection':
                fileName = 'google_chart/connection.png';
                //name = 'Correspondance à<br/><b>'+point.name+'</b>';
                break;
        }
        
        layer.addFeatures([
            new OpenLayers.Feature.Vector(p, {
                picto: 'img/'+fileName, 
                name: name
            })
        ]);
        
        return p;
    },
    
    /**
     * Method: drawLine
     *   Adds a linestring vector features to the map
     */
    drawLine: function(options) {
        var attributes = app.bootstrap.lines[options.lineRef];
    
        var layer = this.getLayerNamed("Routes");
        var blackLayer = this.getLayerNamed("BlackRoutes");
        var line;
        if(options.wkt)
        {
           var geom = OpenLayers.Geometry.fromWKT(options.wkt);
           var oldpoints = geom.getVertices();
           var points = [];
           for (var i = 0; i < oldpoints.length; i++) {
            points.push(oldpoints[i].transform(this.epsg4326, this.epsg900913));
           }
           line = new OpenLayers.Geometry.LineString(points);
        }
        else
        {
           line = new OpenLayers.Geometry.LineString([options.from, options.to]);
        }
        layer.addFeatures([
            new OpenLayers.Feature.Vector(line, attributes)
        ]);
        blackLayer.addFeatures([
            new OpenLayers.Feature.Vector(line.clone(), attributes)
        ]);
        
        var labelsLayer = this.getLayerNamed("Labels");
        var ll = line.getBounds().getCenterLonLat();
        var point = new OpenLayers.Geometry.Point(ll.lon, ll.lat);
        labelsLayer.addFeatures([
            new OpenLayers.Feature.Vector(point, attributes)
        ]);
        
        var iconsLayer = this.getLayerNamed("Icons");
        iconsLayer.addFeatures([
            new OpenLayers.Feature.Vector(point.clone(), attributes)
        ]);
        
        return line;
    },
        
    /**
     * Method: init
     */
    init: function() {
        
        OpenLayers.ImgPath = 'img/openlayers/';
        OpenLayers.IMAGE_RELOAD_ATTEMPTS = 3;
        OpenLayers.Util.onImageLoadErrorColor = "transparent";
        OpenLayers.Lang.setCode('fr');
        
        OpenLayers.Popup.AutoSizedFramedCloud = OpenLayers.Class(
            OpenLayers.Popup.FramedCloud, {
                'autoSize': true,
                'fixedRelativePosition': true,
                'relativePosition': 'bl'
        });

        var map = this.createMap(this.div);        
            
        var layers = this.createLayers();
        for (var i=0; i<layers.length; i++) {
            map.addLayer(layers[i]);
        }
        
        this.map = map;
        
        this.addDeferedControls();
        
        this.displayRoutes();
    },
    
    /**
     * Method: destroy
     */
    destroy: function() {
        GUnload();
        this.map = null;
    },
        
    /**
     * Method: getLayerNamed
     *     Use this to get a layer object from its name
     */  
    getLayerNamed: function(name) {
        return this.map.getLayersByName(name)[0];
    }
};

