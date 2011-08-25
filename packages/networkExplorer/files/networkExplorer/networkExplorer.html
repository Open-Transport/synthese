<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
 
<html>

<head>

<title>SYNTHESE 3.3.0</title>

  <link rel="stylesheet" href="map/css/syntheseMap.css">
  
  <link rel="stylesheet" href="css/index.css" />
  <link rel="stylesheet" href="css/customSelect.css" />
  <link rel="stylesheet" href="css/customAutocomplete.css" />
  <link rel="stylesheet" href="autocomplete/jquery.autocomplete.css" type="text/css" />

  <script type="text/javascript" src="routePlanner/js/legacy/core.js"></script>
  <script type="text/javascript" src="routePlanner/js/legacy/interface.js"></script>
  <script type="text/javascript" src="routePlanner/js/legacy/FicheHoraire.js"></script>

  <script src="http://maps.google.com/maps/api/js?v=3.5&amp;sensor=false"></script>

  <!-- Release versions -->
  <script src="core/vendor/jquery-1.6.2.min.js"></script>
  <script src="core/vendor/jquery.tmpl.beta1.min.js"></script>
  <script src="core/vendor/underscore-1.1.6.min.js"></script>
  <script src="core/vendor/backbone-0.5.1.min.js"></script>

  <script src="map/vendor/OpenLayers/OpenLayers.js"></script>

  <script src="map/vendor/OpenLayers/lib/OpenLayers/Lang/fr.js"></script>
  <script src="map/vendor/OpenLayers_maptypepanel/MapType.js"></script>

  <script src="map/vendor/OpenLayers_maptypepanel/MapTypePanel.js"></script>

  <script src="core/js/Synthese.js"></script>
  <script type="text/javascript" src="routePlanner/js/legacy/utils.js"></script>
  <script src="map/js/SyntheseMap.js"></script>
  <script src="map/js/CityBrowser.js"></script>
  <script src="map/js/CityMap.js"></script>
  
   <!-- TISSEO JS -->

  <script type="text/javascript" src="map/js/tisseo.js"></script>
  
  <!-- AutoComplete -->
  <script type="text/javascript" src="autocomplete/lib/jquery.bgiframe.min.js"></script>
  <!-- <script type="text/javascript" src="autocomplete/lib/jquery.dimensions.js"></script> -->
  <script type="text/javascript" src="autocomplete/jquery.autocomplete.js"></script>

  <!---------------> 

  <script src="routePlanner/js/RoutePlanner.js"></script>
  <script src="map/js/StopSelector.js"></script>

  <script type="text/javascript" src="map/js/legacy/app.Gis.js"></script>
  
  <script>
  Synthese.init(7036874417766402);

  Synthese.ieFix();

  // route planner configuration:
  legacyRoutePlannerConfig.minCharsToTriggerAutocomplete = 0;

  //initAutoCompletions();

  // legacy map application project customizations

  app.Gis.prototype.getBackgroundLayers = function() {
      var bgLayers = [];
      var GOOGLE_NUM_ZOOM_LEVELS = 22;

      bgLayers.push(
          new OpenLayers.Layer.Google("Carte", {
              numZoomLevels: GOOGLE_NUM_ZOOM_LEVELS
          })
      );

      bgLayers.push(
          new OpenLayers.Layer.Google("Photo", {
              type: google.maps.MapTypeId.SATELLITE,
              numZoomLevels: GOOGLE_NUM_ZOOM_LEVELS
          })
      );
      return bgLayers;
  };

  app.init();

  </script>
  
  <script>
  $(function() {
    var Cm79Map = OpenLayers.Class(CityBrowserMap, {
      showLayerSwitcher: false,
      center: ["1.444209", "43.604652", "10"],

      // Don't show OSM, which lacks data in this region.
      getBackgroundLayers: function() {
        var parentLayers = CityBrowserMap.prototype.getBackgroundLayers.apply(this, arguments);
        var googleSatellite = parentLayers[1];
        var google = parentLayers[2];
        return [google, googleSatellite];
      },
      initTranslations: function() {
        CityBrowserMap.prototype.initTranslations.apply(this, arguments);
        OpenLayers.Lang.fr.googleDefaultLayer = "Carte";
      }
    });
    
    var routePlanner = new RoutePlannerView({
      el: document.getElementsByName("ri")
    });

    var stopSelector = new StopSelector({
      routePlanner: routePlanner,
      popupOptions: {
        cityBrowserOptions: {
          mapClass: Cm79Map
        }
      }
    });
    // For debugging:
    $(".mapLink:first").trigger("click");
  });
  </script>

</head>

<body>

<div id="form_map">
    <script id="optionLinesTemplate" type="text/x-jquery-tmpl">
    <li class="${color_switch}">
        <div class="number_line_selector" style="background-color: ${color}; color: ${color_text}">
            ${line_short_name}
        </div>
        <p class="name_line_selector">${name}</p>
        <div name="id_line_filter_area" style="display: none" data-lineid="${id}"></div>
    </li>
    </script>
    <div id="search_area">
        <div class="form_search_area_logo"></div>
        <div class="form_search_area" name="stop">
            <div class="title_form_search_area">Rechercher un arret</div>
            <input searchdata=true name="name_stop_city" type="text" value="Commune" />
            <input searchdata=true name="name_stop" type="text" value="Nom" />
           <!-- <input name="name_city" type="text" value="Commune" /> -->
            <input type="button" value="Chercher" />
        </div>
        
        <div class="form_search_area" name="city">
            <div class="title_form_search_area">Rechercher une commune</div>
            <input searchdata=true name="name_city" type="text" value="Commune" />
            <input type="button" value="Chercher" />
        </div>
        
        <div class="form_search_area" name="adress">
            <div class="title_form_search_area">Rechercher une adresse</div>
            <input searchdata=true name="name_adress_city" value="Commune"/>
            <input searchdata=true name="name_adress" type="text" value="Adresse" disabled="true"/>
            <input name="name_adress_x" type="hidden" value="" />
            <input name="name_adress_y" type="hidden" value="" />
            <input type="button" value="Chercher" />
        </div>
        
        <div class="form_search_area" name="line">
            <div class="title_form_search_area">Rechercher une ligne</div>
            <!-- Line selection area -->
            <div class="dropdown">
                <dt><a href="#"><span searchdata=true>Lignes</span></a></dt>
                <dd>
                    <ul name="name_line">
                        
                    </ul>
                </dd>
            </div>
            <input type="button" value="Chercher" />
        </div>
        
        <div class="form_search_area" name="see_all_stops">
            <input type="button" value="Voir tous les arrêts" />
        </div>
        <div class="form_search_area" name="see_all_lines">
            <input type="button" value="Voir toutes les lignes" />
        </div>
        
    </div>
    <script id="linesTemplate" type="text/x-jquery-tmpl">
    <div class="line_filter_area">
            <div style="display: none">${line_id}</div>
            <div class="number_line_filter_area" style="background-color: ${color}; color: ${color_text}">${line_short_name}</div>
            ${lineName}
            <input isCheckLine="true" checked type=checkbox data-lineid="${id}">
    </div>
    </script>
    <div id="filter_area">
        <div id="title_filter_area">Filtre <input type=checkbox class="checkUncheck"></div>
        <div id="content_line_filter_area"></div>
    </div>
</div>

<div id="message_box">Test de message</div>

<div id="map"></div>
<script>
var cityMap = new TisseoMap("map");
</script>
</body>

</html>
