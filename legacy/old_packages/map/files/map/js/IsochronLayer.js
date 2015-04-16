/*
¨* Inspired by :
 * http://www.websitedev.de/temp/openlayers-heatmap-layer.html
 * Copyright (c) 2010 Bjoern Hoehrmann <http://bjoern.hoehrmann.de/>.
 * This module is licensed under the same terms as OpenLayers itself.
 */

var G_vmlCanvasManager;
 
Isochron = {};

/**
 * Class: Isochron.Stop
 */
Isochron.Stop = OpenLayers.Class({

  /** 
   * APIProperty: lonlat
   * {OpenLayers.LonLat} location of the stop
   */
  lonlat: null,

  /** 
   * APIProperty: step
   * {Number} Step
   */
  step: null,
 
  /**
   * Constructor: Isochron.Stop
   * Create stop.
   *
   * Parameters:
   * lonlat - {OpenLayers.LonLat} Coordinates of stop
   * step - {Number} Optional step
   */
  initialize: function(lonlat, step) {
    this.lonlat = lonlat;
    this.step = step;
  },

  CLASS_NAME: 'Isochron.Stop'
});

/**
 * Class: Isochron.Step
 */
Isochron.Step = OpenLayers.Class({

  /** 
   * APIProperty: stepInf
   * {number} Lower bound of the step
   */
  stepInf: null,

  /** 
   * APIProperty: stepSup
   * {number} Upper bound of the step
   */
  stepSup: null,
  
  /** 
   * APIProperty: color
   * {String} Color of the step
   */
  color: null,

  /**
   * Constructor: Isochron.Step
   * Create a isochron step.
   *
   * Parameters:
   * stepInf - {number} Lower bound of the step
   * stepSup - {Number} Upper bound of the step
   * color - {String} Color of the step
   */
  initialize: function(stepInf, stepSup, color) {
    this.stepInf = stepInf;
    this.stepSup = stepSup;
    this.color = color;
  },

  CLASS_NAME: 'Isochron.Stop'
});

/**
 * Class: Isochron.Layer
 * 
 * Inherits from:
 *  - <OpenLayers.Layer>
 */
Isochron.Layer = OpenLayers.Class(OpenLayers.Layer, {

  /** 
   * APIProperty: isBaseLayer 
   * {Boolean} Isochron layer is never a base layer.  
   */
  isBaseLayer: false,

  /** 
   * Property: stops
   * {Array(<Isochron.Stop>)} internal coordinate list
   */
  stops: null,

  /** 
   * Property: steps
   * {Array(<Isochron.Step>)} steps list
   */
  steps: null,
  
  /** 
   * Property: canvas
   * {DOMElement} Canvas element.
   */
  canvas: null,

  /** 
   * Property: strokeColor
   * {String} Stroke color
   */
  strokeColor: '#000000',

  /** 
   * Property: speed
   * {String} Speed
   */
  speed: 4,

  /**
   * Constructor: Isochron.Layer
   * Create a Isochron layer.
   *
   * Parameters:
   * name - {String} Name of the Layer
   * options - {Object} Hashtable of extra options to tag onto the layer
   */
  initialize: function(name, options) {
    OpenLayers.Layer.prototype.initialize.apply(this, arguments);
    this.stops = [];
    this.steps = [];
    this.canvas = document.createElement('canvas');
    if (G_vmlCanvasManager != undefined) { // ie IE
      G_vmlCanvasManager.initElement(this.canvas);
    }

    this.canvas.style.position = 'absolute';

    // For some reason OpenLayers.Layer.setOpacity assumes there is
    // an additional div between the layer's div and its contents.
    var sub = document.createElement('div');
    sub.appendChild(this.canvas);
    this.div.appendChild(sub);
  },

  /**
   * APIMethod: addStep
   * Adds a step to the layer.
   *
   * Parameters:
   * step - {<Isochron.Step>} 
   */
  addStep: function(step) {
    this.steps.push(step);
  },
 
  /**
   * APIMethod: removeStep
   * Removes a step from the layer.
   * 
   * Parameters:
   * step - {<Isochron.Steps>} 
   */
  removeStep: function(step) {
    if (this.steps && this.steps.length) {
      OpenLayers.Util.removeItem(this.steps, step);
    }
  },
 
  /**
   * APIMethod: addSstop
   * Adds a stop to the layer.
   *
   * Parameters:
   * stop - {<Isochron.Stop>} 
   */
  addStop: function(stop) {
    this.stops.push(stop);
  },

  /**
   * APIMethod: removeStop
   * Removes a stop from the layer.
   * 
   * Parameters:
   * stop - {<Isochron.Stop>} 
   */
  removeStop: function(stop) {
    if (this.stops && this.stops.length) {
      OpenLayers.Util.removeItem(this.stops, stop);
    }
  },

  /**
   * APIMethod: setSpeed
   *
   * Parameters:
   * Speed - Integer 
   */
  setSpeed: function(speed) {
    this.speed = speed;
  },

  /**
   * APIMethod: setStrokeColor
   *
   * Parameters:
   * strokeColor - String 
   */
  setStrokeColor: function(strokeColor) {
    this.strokeColor = strokeColor;
  },  
   
  /**
   * APIMethod: initSteps
   * Initialize steps.
   *
   * Parameters:
   * curvesStep - Integer
   * maxDuration - Integer
   * beginColor - Integer
   * endColor - Integer 
   */
  initSteps: function(curvesStep, maxDuration, beginColor, endColor) {
    var r = (parseInt(beginColor.substr(1,2), 16));
    var g = (parseInt(beginColor.substr(3,2), 16));
    var b = (parseInt(beginColor.substr(5,2), 16));    
  
    var dr = (parseInt(endColor.substr(1,2), 16) - parseInt(beginColor.substr(1,2), 16)) / (maxDuration / curvesStep - 1);
    var dg = (parseInt(endColor.substr(3,2), 16) - parseInt(beginColor.substr(3,2), 16)) / (maxDuration / curvesStep - 1);
    var db = (parseInt(endColor.substr(5,2), 16) - parseInt(beginColor.substr(5,2), 16)) / (maxDuration / curvesStep - 1);

    for ( var end = maxDuration; end > 0 ; end -= curvesStep ) {
      var i = (end - curvesStep) / curvesStep;
      var nr = parseInt(r + dr * i);
      var ng = parseInt(g + dg * i);
      var nb = parseInt(b + db * i);
      var color = '#' + nr.toString(16) + ng.toString(16) + nb.toString(16);
      var _step = new Isochron.Step((end - curvesStep), end, color);
      this.addStep(_step);
    }
  },
  
  /** 
   * Method: moveTo
   *
   * Parameters:
   * bounds - {<OpenLayers.Bounds>} 
   * zoomChanged - {Boolean} 
   * dragging - {Boolean} 
   */
  moveTo: function(bounds, zoomChanged, dragging) {
    OpenLayers.Layer.prototype.moveTo.apply(this, arguments);

    // The code is too slow to update the rendering during dragging.
    if (dragging)
      return;
    
    // Pick some point on the map and use it to determine the offset
    // between the map's 0,0 coordinate and the layer's 0,0 position.
    var someLoc = new OpenLayers.LonLat(0,0);
    var offsetX = this.map.getViewPortPxFromLonLat(someLoc).x -
                  this.map.getLayerPxFromLonLat(someLoc).x;
    var offsetY = this.map.getViewPortPxFromLonLat(someLoc).y -
                  this.map.getLayerPxFromLonLat(someLoc).y;

    this.canvas.width = this.map.getSize().w;
    this.canvas.height = this.map.getSize().h;

    var ctx = this.canvas.getContext('2d');

    ctx.save(); // Workaround for a bug in Google Chrome
    ctx.fillStyle = 'transparent';
    ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
    ctx.restore();

    for (var step in this.steps) {
      for (var i in this.stops) {
        var src = this.stops[i];

        if (src.step < this.steps[step].stepSup)
        {
          // distance in meters
          var distance = (this.steps[step].stepSup - src.step) * (this.speed * 1000) / 60;
          var rad = (distance / (this.map.getResolution())) * 1.5; 
          var pos = this.map.getLayerPxFromLonLat(src.lonlat);
          var x = pos.x + offsetX;
          var y = pos.y + offsetY;
    
          ctx.fillStyle = this.strokeColor;
          ctx.beginPath();
          ctx.translate(x, y);
          // centerX, centerY, radius, startingAngle, endingAngle, counterclockwise
          ctx.arc(0,0,rad+1,0,Math.PI*2,true);
          ctx.closePath();
          
          ctx.translate(-x, -y);
          ctx.fill(); 
        }
        
      }
      for (var i in this.stops) {
        var src = this.stops[i];

        if (src.step < this.steps[step].stepSup)
        {
          // distance in meters
          var distance = (this.steps[step].stepSup - src.step) * (this.speed * 1000) / 60;
          var rad = (distance / (this.map.getResolution())) * 1.5; 
          var pos = this.map.getLayerPxFromLonLat(src.lonlat);
          var x = pos.x + offsetX;
          var y = pos.y + offsetY;
    
          ctx.fillStyle = this.steps[step].color;
          ctx.beginPath();
          ctx.translate(x, y);
          // centerX, centerY, radius, startingAngle, endingAngle, counterclockwise
          ctx.arc(0,0,rad,0,Math.PI*2,true);
          ctx.closePath();
          ctx.translate(-x, -y);
          ctx.fill(); 
        }
      }
    }


    // Unfortunately OpenLayers does not currently support layers that
    // remain in a fixed position with respect to the screen location
    // of the base layer, so this puts this layer manually back into
    // that position using one point's offset as determined earlier.
    this.canvas.style.left = (-offsetX) + 'px';
    this.canvas.style.top = (-offsetY) + 'px';
  },

  /** 
   * APIMethod: getDataExtent
   * Calculates the max extent which includes all of stops.
   * 
   * Returns:
   * {<OpenLayers.Bounds>}
   */
  getDataExtent: function () {
    var maxExtent = null;
        
    if (this.stops && (this.stops.length > 0)) {
      var maxExtent = new OpenLayers.Bounds();
      for(var i = 0, len = this.stops.length; i < len; ++i) {
        var stop = this.stops[i];
        maxExtent.extend(stop.lonlat);
      }
    }

    return maxExtent;
  },

  CLASS_NAME: 'Isochron.Layer'

});
