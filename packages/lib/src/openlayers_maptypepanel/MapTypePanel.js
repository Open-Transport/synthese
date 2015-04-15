/**
 * @requires OpenLayers/Control/Panel.js
 * @requires OpenLayers/Control/MapType.js
 */

/**
 * Class: OpenLayers.Control.MapTypePanel
 *
 *
 * Inherits from:
 *  - <OpenLayers.Control.Panel>
 */
OpenLayers.Control.MapTypePanel = OpenLayers.Class(OpenLayers.Control.Panel, {

    /**
     * Constructor: OpenLayers.Control.MapTypePanel
     * Add buttons to switch between baseLayers.
     *
     * Parameters:
     * options - {Object} An optional object whose properties will be used
     *     to extend the control.
     */
    initialize: function(options) {
        OpenLayers.Control.Panel.prototype.initialize.apply(this, [options]);
    },

    /**
     * Method: setMap
     *
     * Properties:
     * map - {<OpenLayers.Map>}
     */
    setMap: function(map) {
        var layers = map.layers;
        for(var i=0, len=layers.length; i<len; i++) {
            var layer = layers[i];
            if (layer.isBaseLayer) {
                var control = new OpenLayers.Control.MapType(layer);

                if (layer == map.baseLayer) {
                    this.defaultControl = control;
                }
                this.addControls(control);
            }
        }

        OpenLayers.Control.prototype.setMap.apply(this, arguments);
    },

    CLASS_NAME: "OpenLayers.Control.MapTypePanel"
});