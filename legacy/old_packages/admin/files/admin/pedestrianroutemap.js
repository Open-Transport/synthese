var map,tripLayer;
var divId="olmap";

var result_style = OpenLayers.Util.applyDefaults(
    { 
        strokeWidth: 3, 
        strokeColor: "#ff0000", 
        strokeOpacity: 0.5,
        fillOpacity: 0 
    },
    OpenLayers.Feature.Vector.style['default']); 


var dataProjection = new OpenLayers.Projection("EPSG:27572");
var mapProjection = new OpenLayers.Projection("EPSG:900913");

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
      	maxExtent: new OpenLayers.Bounds(130922, 5378938, 185547, 5414263),
        units: 'm',
        projection: "EPSG:900913"
    } );

    var layerOSM = new OpenLayers.Layer.OSM("OSM", null, {
	isBaseLayer: true,

	sphericalMercator: true
    });
    map.addLayer(layerOSM);

    if(tripWKT ){
        var wktFormat = new OpenLayers.Format.WKT();
        var journeys = wktFormat.read(tripWKT);

        tripLayer = new OpenLayers.Layer.Vector("trip", {style: result_style});
        map.addLayer(tripLayer);

        var bbox = new OpenLayers.Bounds();
        for(var i = 0 ; i < journeys.length ; i++) {
            var journey = journeys[i].geometry.clone();
            journey.transform(dataProjection,mapProjection);
            var journeyFeature = new OpenLayers.Feature.Vector(journey); 
            tripLayer.addFeatures([journeyFeature]);
	    bbox.extend(journey.getBounds());
        }
	map.zoomToExtent(bbox);
   }
}

$(document).ready(function() {
	map_init();
});
