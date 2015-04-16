/*!
 * Timeline onBoard v1.0
 * Created by Sébastien BONESTEVE on 16/05/14.
 */

if (typeof jQuery === 'undefined') { throw new Error('Timeline JavaScript requires jQuery') }

(function( $ ){

  var PLUGIN_NS = 'timelinePlugin';

  /*###################################################################################
   * PLUGIN BRAINS
   *
   * INSTRUCTIONS:
   *
   * To init, call...
   * $('selector').myPluginName(options)
   *
   * Some time later...
   * $('selector').myPluginName('myActionMethod')
   *
   * DETAILS:
   * Once inited with $('...').myPluginName(options), you can call
   * $('...').myPluginName('myAction') where myAction is a method in this
   * class.
   *
   *###################################################################################*/

  var Plugin = function ( target, options ) {
    this.$T = $(target);

    this.options= $.extend(
        true,
        {
          statusNumber: 3, // Nombre de statut dans la timeline.
          containerDiv: target.prop('id'), // L'identifiant de la timeline.
          breakpointSlot: 2, // Longeur en minute des points d'arrêts.
          unitTime: 'min', // Unité de temps à affichée à coté du chiffre.
          status: {
            late: {
              pourcent: 26, // Valeur un pourcentage de la zone du statut.
              type: 'warning', // Type du statut.
              text: null, // [Optionnel] Texte du statut.
              textPosition: null, // [Optionnel] Permet de définir la position du texte par rapport à la progressBar en pourcentage.
              nbParts: 2, // [Optionnel] Nombre de point d'arrêt pour ce statut.
              arrowColor: null, // [Optionnel] Couleur de la flèche pour ce statut.
              displayTime: true // [Optionnel] Permet de définir l'affichage ou non du temps au dessus de la flèche.
            },
            standard: {
              pourcent: 48,
              type: 'neutral',
              text: null,
              textPosition: null,
              nbParts: 3,
              arrowColor: null,
              displayTime: true
            },
            ahead: {
              pourcent: 26,
              type: 'danger',
              text: null,
              textPosition: null,
              nbParts: 2,
              arrowColor: null,
              displayTime: true
            }
          },
          arrowColorStatus: true, // Défini si la flèche prend la couleur du statut de la timeline.
          arrowDefaultPosition: 'right' // Défini la position de la flèche par défaut si la calcul de position a échoué: left, right, center
        },
        options
    );

    this._init( target, options );

    return this;
  }
  Plugin.prototype._init = function ( target, options ) {
    // On défini la couleur de la flèche pour chaque statut.
    if (this.arrowColorStatus == true) {
      var progress = target.children('.progress');
      $.each(this.options.status, function(index, state) {
        var color = progress.children('progress-bar-' + state.type).css('background-color');
        if (typeof color != 'undefined') {
          this.options.status[index].arrowColor = color;
        }
      });
    }
    // Création de la structure HTML.
    this._generateHTML(target);
  }
  Plugin.prototype.updateStatusArrow = function(options) {
    if (options.length < 3) {
      $.error('Method updateStatusArrow() need three parameters.');
    } else {
      // On vérifi le formet des paramètres.
      for (var i = 0, lg = options.length; i < lg; i++) {
        if(!/(^([0-1]?[0-9]|2[0-4]):([0-5][0-9])(:[0-5][0-9])?)|null$/.test(options[i])) {
          $.error("The parameters with value of " + options[i] + " isn't a correct time.");
        }
      }
    }
    var tL_arrowWidget = $('#' + this.options.containerDiv).find('.arrow-widget');
    this._updateArrowPosition(tL_arrowWidget, options[0], options[1], options[2]);
    return this.$T;
  }
  Plugin.prototype._generateHTML = function(target) {
    var arrow, progress;

    arrow = '<div class="arrow-widget"><div class="timeline-arrow"><span class="time"></span><span class="arrow"></span></div></div>';
    progress = '<div class="progress">';
    $.each(this.options.status, function(index, state) {
      progress += '<div class="progress-bar progress-bar-' + state.type + '" style="width: ' + state.pourcent + '%">';
      if (state.textPosition) {
        var text = (state.text != null) ? state.text : '';
        progress += '<span class="sr-only-' + state.textPosition + '">' + text + '</span>';
      } else if (state.text != null) {
        progress += state.text;
      }
      progress += '</div>';
    });
    progress += '</div>';

    target.append(arrow + progress);
  }
  Plugin.prototype._updateArrowText = function(elem, time) {
    time = time || "&nbsp;";
    if (time != "&nbsp;")
      elem.html(time + this.options.unitTime);
    else
      elem.html(time);
  }
  Plugin.prototype._updateArrowPosition = function(elem, time, start, end) {
    var position, newColor, timelineArrow = elem.children('.timeline-arrow'),
        options = this.options,
        containerWidth = elem.outerWidth();

    // Calcule du déplacement de la flèche.
    if (start == null || end == null) {
      if (start == null) {
        if (time >= end) {
          this._updateArrowPosition(elem, time, end, end);
        }
      } else  if (end == null) {
          this._updateArrowPosition(elem, time, start, start);
      }
      return;
    }
    if ((start && time < start)) { // avance
      newColor = options.status.ahead.arrowColor;
      var newTime = ((Date.parse("01/01/2014 " + start) - Date.parse("01/01/2014 " + time)) / 60000) || 1,
          timePoint = options. breakpointSlot;
      if (Math.round(newTime) == 0) {
        this._updateArrowPosition(elem, start, start, end);
        return;
      }
      this._updateArrowText(timelineArrow.children('.time'), Math.round(newTime));
      var aheadSize = options.status.ahead.pourcent  * containerWidth / 100;
      if (options.status.ahead.nbParts == 1 && newTime < timePoint) {
        position = containerWidth - aheadSize - (timelineArrow.outerWidth() / 2);
      } else if(newTime < timePoint) { // Première zone: en avance.
        position = (containerWidth - aheadSize) + (aheadSize / 2) - (timelineArrow.outerWidth() / 2);
      } else { // Deuxième zone: très en avance.
        position = containerWidth - timelineArrow.outerWidth();
      }
    } else if (end && time > end) { // retard
      newColor = options.status.late.arrowColor;
      var newTime = ((Date.parse("01/01/2014 " + time) - Date.parse("01/01/2014 " + end)) / 60000) || 1, // Calcul du temps de retard en Int.
          timePoint = options.breakpointSlot;
      if (Math.round(newTime) == 0) {
        this._updateArrowPosition(elem, end, start, end);
        return;
      }
      this._updateArrowText(timelineArrow.children('.time'), Math.round(newTime));
      // Si la zone de retard n'est pas divisée.
      if (options.status.late.nbParts == 1 && newTime < timePoint) {
        position = (options.status.late.pourcent * containerWidth / 100) - (timelineArrow.outerWidth() / 2);
      } else if (newTime < timePoint) { // Première zone: retard.
        position = ((options.status.late.pourcent * containerWidth / 100) / 2) - (timelineArrow.outerWidth() / 2);
      } else { // Deuxième zone: très en retard.
        position = 0;
      }
    } else if (start <= time && time <= end) { // conforme
      newColor = options.status.standard.arrowColor;
      this._updateArrowText(timelineArrow.children('.time'));
      if (false) {
      } else if (time == start && end) {
        var aheadSize = options.status.ahead.pourcent  * containerWidth / 100;
        position = Math.round(containerWidth - aheadSize) - (timelineArrow.outerWidth() / 2);
      } else if (time == end && start) {
        position = (options.status.late.pourcent * containerWidth / 100) - (timelineArrow.outerWidth() / 2);
      } else {
        var ratioTime = Date.parse("01/01/2014 " + end) - Date.parse("01/01/2014 " + start),
            positionTime = Date.parse("01/01/2014 " + end) - Date.parse("01/01/2014 " + time);
        var found = false;
        for (var i = 1; i <= options.status.standard.nbParts; i++) {
          if (positionTime < ((ratioTime / options.status.standard.nbParts) * i)) {
            found = i;
            break;
          }
        }
        // Si la position a été trouvé, on calcul l'emplacement de la flèche sur la zone.
        if (found) {
          position = Math.round(((options.status.late.pourcent * containerWidth / 100) + (((options.status.standard.pourcent * containerWidth / 100) / options.statusNumber) * found)) - (timelineArrow.outerWidth() / 2));
        } else { // Sinon, la flèche est positionner au centre. TODO pas sur que ca soit bon
          switch(options.arrowDefaultPosition) {
            case 'left':
              position = (options.status.late.pourcent * containerWidth / 100) - (timelineArrow.outerWidth() / 2);
              break;
            case 'right':
              position = containerWidth - (options.status.ahead.pourcent * containerWidth / 100) - (timelineArrow.outerWidth() / 2);
              break;
            case 'center':
              position = (containerWidth / 2) - (timelineArrow.outerWidth() / 2);
              break;
          }
        }
      }
    }
    // Animation.
    if (position != null) {
      var arrow = timelineArrow.children('.arrow');
      if (timelineArrow.is(':visible')) {
        timelineArrow
      	  //.css('left', function(){ return $(this).position().left; })
          .animate({left: position}, "slow", "easeInOutCubic");
        if (newColor != arrow.css('color')) { // Si la couleur de la flèche doit changer.
          arrow.animate({
            color: newColor
          }, "slow");
        }
      } else {
        timelineArrow.css('left', position);
        arrow.css('color', newColor);
        timelineArrow.fadeIn();
      }
    }
  }
  Plugin.prototype.disableArrow = function() {
    var timeline = $('#' + this.options.containerDiv);
    timeline.find('.timeline-arrow').fadeOut();
  }
  Plugin.prototype._calculPositionArrow = function(status, time, containerWidth, timelineArrow) {

  }
  Plugin.prototype._calculWithEmpty = function(status, time, containerWidth, timelineArrow) {

  }

  (function (factory) {
    if (typeof define === 'function' && define.amd) {
      // AMD. Register as an anonymous module depending on jQuery.
      define(['jquery'], factory);
    } else {
      // No AMD. Register plugin with global jQuery object.
      factory(jQuery);
    }
  }(function ($) {
    $.fn[ PLUGIN_NS ] = function ( methodOrOptions ) {
      if (!$(this).length) {
        return $(this);
      }
      var instance = $(this).data(PLUGIN_NS);

      // CASE: action method (public method on PLUGIN class)
      if (instance
          && methodOrOptions.indexOf('_') != 0
          && instance[methodOrOptions]
          && typeof(instance[ methodOrOptions ]) == 'function') {

        return instance[ methodOrOptions ](Array.prototype.slice.call(arguments, 1));

        // CASE: argument is options object or empty = initialise
      } else if (typeof methodOrOptions === 'object' || ! methodOrOptions) {

        instance = new Plugin($(this), methodOrOptions);    // ok to overwrite if this is a re-init
        $(this).data(PLUGIN_NS, instance);
        return $(this);

        // CASE: method called before init
      } else if (!instance) {
        $.error('Plugin must be initialised before using method: ' + methodOrOptions);

        // CASE: invalid method
      } else if (methodOrOptions.indexOf('_') == 0) {
        $.error('Method ' +  methodOrOptions + ' is private!');
      } else {
        $.error('Method ' +  methodOrOptions + ' does not exist.');
      }
    };  

  }));

})(jQuery);