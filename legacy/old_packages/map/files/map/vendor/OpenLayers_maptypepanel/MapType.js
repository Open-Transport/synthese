/* Copyright (c) 2006-2008 MetaCarta, Inc., published under the Clear BSD
 * license.  See http://svn.openlayers.org/trunk/openlayers/license.txt for the
 * full text of the license. */

/**
 * @requires OpenLayers/Control.js
 */

/**
 * Class: OpenLayers.Control.MapType
 *
 * Inherits from:
 *  - <OpenLayers.Control>
 */
OpenLayers.Control.MapType = OpenLayers.Class(OpenLayers.Control, {

    /**
     * Property: layer
     * {OpenLayers.Layer} The baseLayer this control is attached to.
     */
    layer: null,

    /**
     * Constructor: OpenLayers.Control.MapType
     * Control which handles the button for the MapTypePanel control.
     *
     * Parameters:
     * layer - {String} The baseLayer this control is attached to
     * options - {Object} An optional object whose properties will be used
     *     to extend the control.
     */
    initialize: function(layer, options) {
        this.layer = layer;
        OpenLayers.Control.prototype.initialize.apply(this, [options]);
    },

    /**
     * Method: draw
     * The draw method is called when the control is ready to be displayed
     * on the page.  If a div has not been created one is created.  Controls
     * with a visual component will almost always want to override this method
     * to customize the look of control.
     *
     * Parameters:
     * px - {<OpenLayers.Pixel>} The top-left pixel position of the control
     *      or null.
     *
     * Returns:
     * {DOMElement} A reference to the DIV DOMElement containing the control
     */
    draw: function (px) {
        OpenLayers.Control.prototype.draw.apply(this, arguments);

        var inner = OpenLayers.Util.createDiv();
        inner.innerHTML = this.layer.name;
        this.div.appendChild(inner);
        this.div.style.position = 'relative';

        this.panel_div.appendChild(this.div);
        return this.panel_div;
    },

    /**
     * Method: activate
     */
    activate: function(){
        OpenLayers.Control.prototype.activate.apply(this);
        this.map.setBaseLayer(this.layer);
    },

    CLASS_NAME: "OpenLayers.Control.MapType"
});
