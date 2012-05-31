var map,tripLayer;
var divId="olmap";

Proj4js.defs["EPSG:27572"] = "+proj=lcc +lat_1=46.8 +lat_0=46.8 +lon_0=0 +k_0=0.99987742 +x_0=600000 +y_0=2200000 +a=6378249.2 +b=6356515 +towgs84=-168,-60,320,0,0,0,0 +pm=paris +units=m +no_defs";


var result_style = OpenLayers.Util.applyDefaults(
    { 
        strokeWidth: 3, 
        strokeColor: "#ff0000", 
        strokeOpacity: 0.5,
        fillOpacity: 0 
    },
    OpenLayers.Feature.Vector.style['default']); 


var mapProjection = new OpenLayers.Projection("EPSG:900913");
var dataProjection = new OpenLayers.Projection("EPSG:27572");

function map_init() {
    var mapDiv = document.getElementById(divId);
    mapDiv.style.height="400px";
    mapDiv.style.width="100%";
    map = new OpenLayers.Map (divId, {
        controls:[
            new OpenLayers.Control.Navigation(),
            new OpenLayers.Control.PanZoomBar(),
            new OpenLayers.Control.Attribution()
        ],
        maxExtent: new OpenLayers.Bounds(-20037508.34,-20037508.34,20037508.34,20037508.34),
        maxResolution: 156543.0399,
        numZoomLevels: 19,
        units: 'm',
        projection: mapProjection
    } );


    // Define the map layer
    // Other defined layers are OpenLayers.Layer.OSM.Mapnik, OpenLayers.Layer.OSM.Maplint and OpenLayers.Layer.OSM.CycleMap
    var layerOSM = new OpenLayers.Layer.OSM.Mapnik("OSM");
    map.addLayer(layerOSM);

    if( tripWKT ){
        var wktFormat = new OpenLayers.Format.WKT();
        var journeys = wktFormat.read(tripWKT);

        tripLayer = new OpenLayers.Layer.Vector("trip", {style: result_style});
        map.addLayer(tripLayer);

        var bbox;
        if(journeys[0]) {
            var journey = journeys[0].geometry.clone();
            journey.transform(dataProjection,mapProjection);
            var journeyFeature = new OpenLayers.Feature.Vector(journey); 
            tripLayer.addFeatures([journeyFeature]);
            bbox = journey.getBounds();
            map.zoomToExtent(bbox);
        }
   }
}

var onload = window.onload;
window.onload = new function(){onload();map_init();};

