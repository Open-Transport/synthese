define([
  "core/js/Synthese",
  "map/js/SyntheseMap",
  // JQuery autcomplete
  "jquery",
  // TODO: add to shim?
  "networkExplorer/vendor/autocomplete/lib/jquery.bgiframe.min",
  "networkExplorer/vendor/autocomplete/jquery.autocomplete"
], function(Synthese, SyntheseMap) {

/* ************

Define the events for search form

************* */
var TisseoMap = OpenLayers.Class(SyntheseMap, {

    dataList: [],
    
    labelMessages: {
      BAD_CITY    : "Le nom de commune n'est pas valide : ",
      BAD_RESULT  : "Aucun résultat !",
      BAD_PARAM   : "Veuillez renseigner tous les champs !",
      ALL_STOPS   : "Tous les arrêts !",
      ALL_LINES   : "Toutes les lignes !",
      NAME_STOPS  : "Arrêts",
      NAME_STOP   : "Arrêt",
      NAME_LINE   : "Ligne",
      NAME_SCHEDULE : "Horaire",
      NAME_DIR    : "Direction",
      NAME_CITY   : "Commune",
      NAME_ADRESS : "Adresses"
    },
    
    labelData: {
      TRANSPARENT : "transparent",
      STOP        : "stop",
      CITY        : "city",
      LINE        : "line",
      ID          : "id",
      TYPE        : "type",
      LVL         : "lvl",
      NAME        : "name",
      DEST        : "destination"
    },
    
    labelParam: {
      NB_MAX_JOURNEY   : 6,
      ZOOM_IF_XY_GIVEN : 15,
      TIMEOUTPOPUP     : 5000,
      HEIGHT_MAX_POPUP_ADRESS  : 100,
      HEIGHT_ONE_RESULT_ADRESS : 20
    },
    
    extentCache: [],
    
    markers: null,

    // CONSTRUCTOR
    initialize: function(mapId, options) {
    
      SyntheseMap.prototype.initialize.apply(this, arguments);
   
    },
    
    beforeMapInit: function() {
        console.log("TisseoMap::beforeMapInit");
    },
    
    fetchLines: function(filter, withStops) {
      filter.rollingStockId = Synthese.queryStringOptions.rollingStockId;
      return SyntheseMap.prototype.fetchLines.call(this, filter, withStops);
    },
    
    setColorSimilarLine: function() {
    
        console.log("TisseoMap::setColorSimilarLine");
    
        // FIND A SIMILAR SHORT NAME LINE (10 <=> 10s) TO RETURN A VALID COLOR
        function findSimilarLine(line) {
        
            var founded = null;
            
            _.each(self.linesLayer.features, function(feature) {
                var line_attr = feature.attributes;
                if (!founded && parseInt(line_attr.line_short_name) == parseInt(line.line_short_name) && line_attr.line_color && line.line_short_name != line_attr.line_short_name)
                    founded = line_attr.line_color;
            });
            
            return (founded) ? founded : self.labelData.TRANSPARENT;
        }
    
        var self    = this;
        var tabMade = [];
    
        _.each(this.stopsLayer.features, function(featureStop) {
            _.each(featureStop.attributes.lines, function(feature) {
                if(!feature.line_color && !_.include(tabMade, feature.line_short_name)){
                        feature.line_color = findSimilarLine(feature);
                        tabMade[feature.line_short_name] = feature.line_color;
                }
            });
        });
        
        _.each(this.linesLayer.features, function(line) {
               if(tabMade[line.attributes.line_short_name])
                    line.attributes.line_color = tabMade[line.attributes.line_short_name];
        });
        
    },
    
    // DISPLAY OR NOT A MAP'S FEATURE ACCORDING TO ARGS
    setFeatureVisible: function (feature, visible) {
      feature.attributes.visible = visible;
    },
    
    // EMPTY FILTER AREA
    emptyFilter: function(){
        $("#content_line_filter_area").empty();
    },
    
    // SHOW LINES FROM A GIVEN LIST
    showListLines: function(selectedLineIds){
        console.log("TisseoMap::showListLines");
        console.log("selectedLineIds", selectedLineIds);
        var self = this;
        _.each(this.linesLayer.features, function(line) {
                self.setFeatureVisible(line, (selectedLineIds == "*") ? true : selectedLineIds[line.attributes.id]);
        });
        this.linesLayer.redraw();
    },
  
    // SHOW STOPS FROM A GIVEN LIST
    showListStops: function(selectedStopIds){
        console.log("TisseoMap::showListStops");
        var self = this;
        _.each(this.stopsLayer.features, function(stop) {
                self.setFeatureVisible(stop, (selectedStopIds == "*") ? true : selectedStopIds[stop.attributes.id]);
        });
        this.stopsLayer.redraw();
    },
    
    // SORT AND UPDATE COLORS FROM A GIVEN LINE LIST
    displayInFilter: function(linesList, isThereAswitch){
    
        console.log("TisseoMap::displayInFilter");
    
        if(isThereAswitch)
            var switcher = 0; 
    
        var self = this;
        linesList.sort(this.sortLines);
        linesList = _(linesList).chain()
        .map(function(line) {
          line.color        =  line.line_color && ("rgb" + line.line_color);
          line.color_text   = (line.color != "transparent") ? "white" : "black";
          // If a color switch is needed
          if(isThereAswitch){
              line.color_switch = (switcher % 2 == 0) ? "color_switch" : "";
              switcher++;
          }
          return line;
        })
        .value();
        
        return linesList;
    },
    
    updatePTFeatures: function() {
                
      console.log("TisseoMap::updatePTFeatures");
          
      var selectedLineIds = [];
      var selectedStopIds = [];
      var self = this;
      
      $("#content_line_filter_area input:checked").each(function(index, checkbox) {
        // Warning: don't use jQuery.data to fetch the lineid attribute,
        // otherwise it will be converted to a number with its value truncated
        // to JavaScript number precision.
        selectedLineIds[$(checkbox).attr("data-lineid")] = true;
      });
      
      // Gets stops on each specified line
      _.each(self.stopsLayer.features, function(stop) {
        var visible = _.any(stop.attributes.lines, function(line) {
          return selectedLineIds[line.id];
        });

        selectedStopIds[stop.attributes.id] = visible;
      });
      // Show lines & stops
     // this.showListStops(selectedStopIds);
      this.showListLines(selectedLineIds);
            
    },
    
    initLineChange: function(){
    
        console.log("TisseoMap::initLineChange");
    
        var self = this;
    
        // Creates event on change in filter area
        $("#content_line_filter_area").change(this.updatePTFeatures);

        $(".checkUncheck").attr("checked", "true");
        // To check/uncheck all check box
        $(".checkUncheck").click(function(event) {
          $("input[isCheckLine=true]").each(function() {
            var checkState = ($(".checkUncheck").attr("checked")) ? $(".checkUncheck").attr("checked") : false ;
            $(this).attr("checked", checkState);
          });
          self.updatePTFeatures();
        });
        console.log("TisseoMap::initLineChange::End");
    },
    
    
    // Fill filter area according to line list given
    fillFilter: function(linesList) {
    
        console.log("TisseoMap::fillFilter");
        
        // Empty div
        this.emptyFilter();
        
        var self = this

        var template = $.template(null, [
          '<div class="line_filter_area">',
          '        <div style="display: none">${line_id}</div>',
          '        <div class="number_line_filter_area" style="background-color: ${color}; color: ${color_text}">${line_short_name}</div>',
          '        ${lineName}',
          '        <input isCheckLine="true" checked type=checkbox data-lineid="${id}">',
          '</div>'
        ].join("\n"));

        var linesListFilter = _(linesList).chain()
        .map(function (line) {
          line.lineName = (line.lineName.length > 20) ? line.lineName.substr(0,20) + "..." : line.lineName;
          return line;
        })
        .value();
        
        console.log(linesListFilter);
        
        $.tmpl(template, self.displayInFilter(linesListFilter)).appendTo("#content_line_filter_area");
        
        $("input[isCheckLine=true]").click(function() {
          self.updatePTFeatures();
        });
        
        this.updatePTFeatures();
    
    },
    
    // INSERT DATA ABOUT LINES AND STOPS IN AN ASSOCIATIVE ARRAY
    saveData: function(){
    
      console.log("TisseoMap::saveData");
    
      var self = this;
      
      // Insert data in a custom data template
      // Stops
      _.each(this.stopsLayer.features, function(stop) {
        self.dataList[stop.attributes.id] = {
          type   : self.labelData.STOP,
          object : stop.attributes
        };
        // Cities
        self.dataList[stop.attributes.cityId] = {
          type   : self.labelData.CITY,
          object : {
            id: stop.attributes.cityId,
            name: stop.attributes.cityName
          }
        };
      });
      
      //Lines
      _.each(this.linesLayer.features, function(line) {
        self.dataList[line.attributes.id] = {
          type   : self.labelData.LINE,
          object : line.attributes
        };
      });
     
      console.log(this.dataList);
     
    },
    
    // Returns a data list following 
    // TODO: Make it recursive
    searchEngine: function(filter, dataTab){
    
        console.log("TisseoMap::searchEngine - lvl : "+filter.lvl);
    
        var result = [];
        var self   = this;
        console.log("ARGUMENTS : ", filter);
        // Loads dataList is no other data Table is given
        dataTab = (!dataTab) ? this.dataList : dataTab;
        console.log("DATATAB : ", dataTab);
        for (id in dataTab){
            var data = dataTab[id].object || dataTab[id];
            // If a type filter is requested
            if(filter.type && dataTab[id].type && dataTab[id].type != filter.type)
                continue;
            var fine = true;
            // Loop on all the filters and comparing them to data
            for ( var key in data ){
              // If field contains a sub object and a su search is needed
              if ( filter.lvl > 0 && _.isArray(data[key]) ) {
                filter.lvl--;
                var searchRecursive = self.searchEngine(filter, data[key]);
                fine = (_.size(searchRecursive) > 0);
                filter.lvl++;
              // If it's a regular field, trying to match with filter
              }else if ( filter[key]
                         && filter[key] != data[key] ) {
                fine = false;
              }
            }
            // If all filters are positive, adding the element
            if ( fine )
                result.push(data);
        }
        console.log("RESULT SEARCHENGINE ("+result.length+") : ",result);
        
        return result;
        
    },
    
    // INITALIZE AUTO COMPLETION FOR STOPS AND CITIES
    initAutoComplete: function() {
    
        console.log("TisseoMap::initAutoComplete");
        var self = this;
    
        // INITALIZE COMPLETION DIV TO REPLACE AUTOCOMPLETE PLUGIN JQUERY
        function createCompletionDiv(data_adress) {
            
            console.log("TisseoMap::initAutoComplete::createCompletionDiv");
            
            // Create a list of adress or stops with keys
            function createLiList(text, list) {
            
                if(list.length > 0)
                    $("<li>")
                    .addClass('ac_legend_tisseo')
		            .text(text)
		            .appendTo(element_ul);
		        
                 for(id in list){
                    var element_li = $("<li>")
		            .addClass("ac_even_tisseo")
		            .html("<span name='name_place'>"+list[id].name + "</span><span name='x' style='display:none'>"+list[id].pos.x+"</span><span name='y' style='display:none'>"+list[id].pos.y+"</span>")
		            .appendTo(element_ul);
		            heightResult += (heightResult < self.labelParam.HEIGHT_MAX_POPUP_ADRESS) ? self.labelParam.HEIGHT_ONE_RESULT_ADRESS : 0;
		            if(cpt % 2 == 0)
		                $(element_li).addClass("ac_odd_tisseo"); 
		            cpt++;
                }
                
            }
        
            // Delete all olds search div
            $(".ac_results_tisseo").remove();
        
            var heightResult = 0;
            var cpt          = 0;
            
            // Creates <ul> element to contain <li> fields
            var element_ul = $("<ul>");
            
            // Loop on all result to display them
            createLiList(self.labelMessages.NAME_STOPS, data_adress.stop);
            createLiList(self.labelMessages.NAME_ADRESS, data_adress.street);
            
            // Recover parent's input position
            var position = $('input[name="name_adress"]').position();
            
            // Creates a div containing data with extents matching parent's input
            var element_div = $("<div>")
		        .addClass("ac_results_tisseo")
		        .css("height", heightResult)
		        .css("width", $('input[name="name_adress"]').width())
		        .css("top", position.top + $('input[name="name_adress"]').outerHeight(true))
		        .css("left", position.left)
		        // Appends <div> to <body>
		        .appendTo(document.body);
	        
	        // Appends <ul> to <div>
	        element_ul.appendTo(element_div);
	        
	        // Load events for selector
            $(".ac_even_tisseo").click(function() {
                var text = $(this).find("[name='name_place']").text();
                $("input[name='name_adress']").val(text);
                $("input[name='name_adress_x']").val($(this).find("[name='x']").text());
                $("input[name='name_adress_y']").val($(this).find("[name='y']").text());
                $(element_div).remove();
            }); 
            
	        // When click out of adress div => hide
	        $(document).bind('click', function(e) {
                var $clicked = $(e.target);
                if (! $clicked.parents().hasClass("ac_results_tisseo"))
                    $(element_div).remove();
            });
		
        }
    
        // Load autocompletion for adress using synthese's calls
        function initCompletionAdress(){
            
            console.log("TisseoMap::initAutoComplete::initCompletionAdress");
            
            var adressXHR   = null;
            var args        = {};
            
            // Load event onKeyUp (>= 3 chars && != 'Adresse')
            $('input[name="name_adress"]').keyup(function() {
            
                if($('input[name="name_adress"]').val().length < 3){
                    $(".ac_results_tisseo").hide();
                    return;   
                }
                
                args     = {
                  si       : Synthese.queryStringOptions.siteId,
                  i        : $('input[name="name_adress"]').val(),
                  ct       : $('[name="name_adress_city"]').val(),
                  n        : 20,
                  srid     : 4326
                };
                // Call Synthese
                adressXHR = self.callSynthese("lp", args, "xml");
                // When XML is loaded find name
                adressXHR.done(function(adressDocument) {
                  var data_adress = {
                    street : [],
                    stop   : []
                  };
                  
                  // Loop on all options given by Synthese
                  $(adressDocument).find("option").each(function() {
                    var data_adress_temp = [];
                    if($(this).attr("type") == "stop")
                        data_adress_temp = data_adress.stop;
                    else
                        data_adress_temp = data_adress.street;
                    data_adress_temp.push({
                        name: $(this).attr("name"),
                        pos: {
                            x: $(this).attr("x"), 
                            y: $(this).attr("y")
                        }
                    });
                      // Create autoCompletion on adress div
                      createCompletionDiv(data_adress);                                                   
                  });
                });
            });   
        }
        
        // Search new stops following the city fields
        function searchNewStops(){
        
            console.log("TisseoMap::initAutoComplete::searchNewStops");
            
            // Find city id according to the town's name given
            function findCityId(nameCityStop){
            
               console.log("TisseoMap::initAutoComplete::searchNewStops:findCityId");
               
                // Check if a given city exists
                var filter = {
                    type: self.labelData.CITY,
                    name: nameCityStop
                }
                var idReturn = self.searchEngine(filter);
                idReturn     = (_.size(idReturn) > 0) ? _.first(idReturn).id : null;
                console.log("TisseoMap::initAutoComplete::searchNewStops:findCityId::end");
                return idReturn;
            }
            
            var nameCityStop = $('div[name="stop"]').find("input[name=name_stop_city]").val();
            var idCity       = findCityId(nameCityStop);
           
            var filter = {
                type: self.labelData.STOP
            }
            if(idCity)
                filter.cityId = idCity;
            data_stop = self.searchEngine(filter);
                   
            console.log("TisseoMap::initAutoComplete::searchNewStops:end");
            return data_stop;
            
        }
        
        // Recovering city names
        var filter = {
            type: self.labelData.CITY,
        };
        var data_cities = this.searchEngine(filter);

        /* 
         * Stop Part
         */
        // Loads autoCompletion on stop's field
        $('input[name="name_stop"]')
        .autocomplete(searchNewStops(), {
            matchContains: true,
            autoFill: true,
            // Data's format in select box
            formatItem: function (item) {
              return item.name + " (" + item.city_name + ")";
            },
            // Data's format for search
            formatMatch: function (item) {
              return item.name;
            }
        })
        // Loads new stops when clicking on stop's field
        .click(function () {
            $('input[name="name_stop"]').setOptions({ 
              data: searchNewStops() 
            });
        });
        
        /* 
         * Adress Part
        */
        
        function checkCity (city) {
          var filter = {
            type: self.labelData.CITY,
            name: city
          };
          var data_city = self.searchEngine(filter);
          var isCtName = (data_city.length > 0);
          $('[name="name_adress"]').attr("disabled", !isCtName); 
          if(!isCtName)
              self.showMessage(self.labelMessages.BAD_CITY + $('[name="name_adress_city"]').val());
        }
        
        initCompletionAdress();
         
        // Change autocompletion for stops chen cities are changing
        $('input[name="name_adress"]').click(function (){
            checkCity($('[name="name_adress_city"]').val());
        });
        
        // Add event onResult on town's form
        $('[name="name_adress_city"]').result(function() {
            checkCity($('[name="name_adress_city"]').val());
        });
        
        /* 
         * City Part
         */
        
        // Add autocompletion to specific fields
        $('input[name*="_city"]')
        .autocomplete(data_cities, {
            matchContains: true,
            autoFill: true,
            // Data's format in select box
            formatItem: function (item) {
              return item.name;
            }
        });
        
    },
    
    // SORT LINES IN A SPECIFIC ORDER :
    // 1 - WORD WITH ALPHA ORDER ( AERO, B, etc...)
    // 2 - LINES IN NUM ORDER (20, 30, 40,e etc...)
    // 3 - LINES WITH LETTER(S) BEHIND THEIR 'PARENT' (10, 10s, 30, 30s, etc...)
    sortLines: function(a, b)
    {
        var expr  = new RegExp("^[a-zA-Z]");
        a        = a.line_short_name;
        b        = b.line_short_name;

        if(a.match(expr) && b.match(expr))
          return a > b;
        if(a.match(expr) && !b.match(expr))
          return false;
        if(!a.match(expr) && b.match(expr))
          return true;
        if(parseInt(a) == parseInt(b))
          return a > b;

        return parseInt(a) - parseInt(b);
    },
    
    // INITIALIZE LINE SELECTOR
    initLinesSelector: function() {
       
        var self      = this;
        var linesList = this.searchEngine({type: self.labelData.LINE});
    
        var template  = $.template(null, [
          '<li class="${color_switch}" lineid="${id}">',
           '<div class="number_line_selector" style="background-color: ${color}; color: ${color_text}">',
           ' ${line_short_name}',
           '</div>',
           '<p class="name_line_selector">${name}</p>',
         '</li>'
        ].join('\n'));
          
        var switcher = 0;
        // Sort and update background and font colors
        linesList = this.displayInFilter(linesList, true);
        $.tmpl(template, linesList).appendTo("ul[name='name_line']");
        
        // Load events for line selector
        $(".dropdown dd ul li").click(function() {
            var text = $(this).html();
            var id   = $(".dropdown dt input").val($(this).attr("lineid"));
            $(".dropdown dt a span")
            .css("min-height", "37px")
            .html(text);
            $(".dropdown dd ul").hide();
        }); 
        // Toggle display when there is a click on the selector
        $(".dropdown dt a").click(function() {
            $(".dropdown dd ul").toggle();
        });
        // Close Selector when there is a click elsewhere
        $(document).bind('click', function(e) {
            var $clicked = $(e.target);
            if (! $clicked.parents().hasClass("dropdown"))
                $(".dropdown dd ul").hide();
        });
    },
    
    getExtent: function(args) {

        console.log("TisseoMap::getExtent");
        
        var id = args.id;
        
        //return this.extentCache[id];
          
         stopFeatures = this.getGeometry(args);
        
          if (stopFeatures.length == 0)
            return null;

          var extent = new OpenLayers.Bounds();
          _.each(stopFeatures, function(stopFeature) {
            extent.extend(stopFeature.geometry);
          });

          return this.extentCache[id] = extent;
    },
    
    // Zoom following a given BBOX
    zoomToExtentTisseo: function(args){
    
        console.log("TisseoMap::zoomToExtentTisseo");
        
        var self = this;
        
        console.log("ARGS", args);
        
        if(args.filter.x && args.filter.y){
          // Zoom to given position
          this.map.setCenter(
            new OpenLayers.LonLat(args.filter.x, args.filter.y).
              transform(new OpenLayers.Projection("EPSG:4326"),
                self.map.getProjectionObject()),
            self.labelParam.ZOOM_IF_XY_GIVEN);
          return;   
        }
        var extent = this.getExtent(args);
        if (extent)
           this.map.zoomToExtent(extent);
    },
    
    // CHECK GIVEN FIELDS FROM A SEARCH FORM
    checkData: function(fields) {
        console.log("TisseoMap::checkData");
        for(id in fields)
          if (!fields[id] || fields[id] == "") {
            this.showMessage(this.labelMessages.BAD_PARAM);
            return false;
          }
        
        return true;
    },
    
    // INITIALIZE DEFAULT EVENTS
    initDefaultEvent: function() {
    
      console.log("TisseoMap::initDefaultEvent");
    
      var self = this;
      // Through all forms of type "searchtype"
      $(".form_search_area:has(input[searchtype])").each(function(){
        
        var type       = $(this).attr("name");
        var noSearchId = $(this).attr("noDataSearch");

        // Bind click on form's search button
        $(this).find(":button").bind('click', function() {
      
          // Gets text to be displayed on the map of an element as been found
          var toBeDisplayed = $(this).parent().find("[toBeDisplayed='true']").html() || $(this).parent().find("[toBeDisplayed='true']").val();
        
          // Gets the data fields
          var type_data2 = [];
          $(this).parent().find("input[searchtype]").each(function () {
            type_data2[$(this).attr("searchtype")] = $(this).val();
          });
          console.log(type_data2);
          // Check form
          if(!self.checkData(type_data2))
            return;
          
          // Either the id is already given, or we seek
          if(!noSearchId){
            type_data2[self.labelData.TYPE] = type;
            var obj                         = self.searchEngine(type_data2);
            type_data2[self.labelData.ID]   = (_.size(obj) > 0) ? _.first(obj).id : null;
            // If no element has been found, it returns an error message
            if (!type_data2[self.labelData.ID]){
              self.showMessage(self.labelMessages.BAD_RESULT);
              return;
            }
          }
          
          // Filter to apply to search Engine
          // Several cases are possible
          var filter                  = [];
          // 1st case : Seeking for element's coordonate 
          filter[type+"_id"]          = type_data2[self.labelData.ID];
          filter[self.labelData.TYPE] = self.labelData.STOP;
          // 2nd case : Coordonates already given
          if(type_data2["x"] && type_data2["y"])
            filter = type_data2;
          // 3rd case : Seeking coordonate in sub levels
          if( type == self.labelData.LINE ){
            filter[self.labelData.LVL] = 1;
            filter.test = true;
            }
          // Zoom to the element
          self.zoomToExtentTisseo({
            id     : type_data2[self.labelData.ID],
            filter : filter
          });
          
          // Seek lines to be displayed
          // TODO: Insert the stops in the xml lines
          var linesToShow = [];
          if( type == self.labelData.STOP )
            linesToShow = self.dataList[type_data2[self.labelData.ID]].object.lines;
          if( type == self.labelData.LINE )
            linesToShow = [self.dataList[type_data2[self.labelData.ID]].object];
          console.log("LINES TO SHOW", linesToShow);
          self.fillFilter(linesToShow);
          
          // Shows Place's informations
          self.showMessage(toBeDisplayed);
        });
        
      });
    },
    
    // SHOWS POPUP WHEN A STOP IS SELECTED
    onStopSelected: function(feature) {
    
        console.log("TisseoMap::onStopSelected");
    
        var self = this;
    
        function recoverJourneys(feature) {
        
            function constructJourney(journeyDocument) {
            
                var cpt  = 0;
                
                var textJourney  = [
                  "<table class='tabJourneyStop'>",
                    "<tr>",
                      "<td class='keysStopJourney'>"+self.labelMessages.NAME_SCHEDULE+"</td>",
                      "<td class='keysStopJourney'>"+self.labelMessages.NAME_LINE+"</td>",
                      "<td class='keysStopJourney'>"+self.labelMessages.NAME_DIR+"</td>",
                    "</tr>",
                ].join('\n');
              
                // Loop on all journey given by Synthese
                $(journeyDocument).find("journey").each(function() {
                    
                    if(cpt < self.labelParam.NB_MAX_JOURNEY){
                        var time = $(this).attr("dateTime").split(" ");
                        time = time[1];
                        var dest = $(this).find(self.labelData.DEST).attr("cityName") + "/" + $(this).find("destination").attr(self.labelData.NAME);
                        var switchClass = (cpt % 2 == 0) ? "switchCellJourneyStop" : "";
                        
                        textJourney += [
                          "<tr class='cellJourneyStop "+switchClass+"'>",
                            "<td class='textInJourneyStop'>",
                              time,
                            "</td>",
                            "<td class='number_line_filter_area number_line_journey_stop' style='background-color: rgb"+tabColors[$(this).find(self.labelData.LINE).attr("shortName")]+"; color: white'>",
                              $(this).find("line").attr("shortName"),
                            "</td>",
                            "<td class='textInJourneyStop'>",
                              dest,
                            "</td>",
                          "</tr>"
                        ].join("\n");
                    }
                    cpt++;
                });
                
                textJourney += [
                  "</table>"
                ].join("\n");
                
                return textJourney;
                
            }
        
            var tabColors = [];
        
            // Insert RGB colors of lines in tab
            _.each(feature.attributes.lines, function(line){ 
                tabColors[line.line_short_name] = line.line_color;
            });
            
            // Coor recovering
            var latLonProj = new OpenLayers.Projection("EPSG:4326");
            var point  = new OpenLayers.Geometry.Point(
               parseFloat(feature.geometry.x),
               parseFloat(feature.geometry.y)
            );
            // Recovers LonLat from feature's point
            var ll = new OpenLayers.LonLat(point.x, point.y); 
        
            // Add marker on the map
            addMarker(ll, "<img src='images/loader.gif' />", true, true);
        
            // Today's date
            var date     = new Date();
            var date2str = date.getFullYear() + "-" + (date.getMonth()+1) + "-" + (date.getDay()+1) + " " + (date.getHours()+1) + ":" + (date.getMinutes()+1);
            
            var args = {
                    si     : Synthese.queryStringOptions.siteId,
                    ni     : this.networkId,
                    // Date format : YYYY-MM-DD HH:MiMi i.e -> 2011-08-22 14:35
                    date   : date2str,
                    // Number of result
                    rn     : 5,
                    // City name
                    cn     : feature.attributes.city_name,
                    // Stop name
                    sn     : feature.attributes.stop_name
            };
            
            // Call Synthese
            var journeyXHR = self.callSynthese("tdg", args, "xml");
                    
            // When XML is loaded find name
            journeyXHR.done(function(journeyDocument) {
                 
                  var text = [
                    "<table class='titleStopJourney'>",
                      "<tr>",
                        "<td>",
                          self.labelMessages.NAME_STOP+" : <b>" + feature.attributes.stop_name,
                          "</b> "+self.labelMessages.NAME_CITY+" : <b>" + feature.attributes.city_name + "</b>",
                        "</td>",
                      "</tr>",
                    "</table>"
                 ].join('\n');
                 
                 text += [
                  constructJourney(journeyDocument)
                 ].join("\n");
                 
                // Add marker on the map
                addMarker(ll, text, true, true); 
            });
            
        }
    
        function addMarker(ll, popupContentHTML, closeBox, overflow) {
        
            function onPopupClose(evt) {
                this.destroy();
            } 
            
            OpenLayers.Popup.AutoSizeFramedCloudMinSize = OpenLayers.Class(OpenLayers.Popup.FramedCloud, {
                'autoSize': true,
                'minSize': new OpenLayers.Size(100,100),
                'contentDisplayClass': 'contentPopupJourney'
            }); 
            
            
            var popup = new OpenLayers.Popup.AutoSizeFramedCloudMinSize("featurePopup",
                    ll,
                    new OpenLayers.Size(100,100),
                    popupContentHTML,
                    null,
                    true, 
                    onPopupClose); 
                    
           self.map.addPopup(popup, true);
            
        } 
        
        recoverJourneys(feature);
        
    },
    
    // Show a temporary message on the map
    showMessage: function(text) {
        console.log("TisseoMap::showMessage");
        // Load Text in div
        $('#message_box').html(text);
        // Show message
        $('#message_box').fadeIn('slow');
        // Hide emssage after 5 sec
        setTimeout("$('#message_box').fadeOut()", this.labelParam.TIMEOUTPOPUP);
    },

    // INITIALIZE EVENTS ON BUTTONS TO SEE ALL STOPS OR LINES
    initButtonEvents: function(){
        
        console.log("TisseoMap::initButtonEvents");
    
        var self = this;
        var dataListTempLines = this.searchEngine({
          type: self.labelData.LINE
        });
        
        $('[name*="see_all"]').find(":button").bind('click', function(event) {
            if($(this).parents().attr("name") == "see_all_stops"){
                self.showListStops("*");
                self.showMessage(self.labelMessages.ALL_STOPS);   
            }
            if($(this).parents().attr("name") == "see_all_lines"){
                self.showListLines("*");
                self.fillFilter(self.displayInFilter(dataListTempLines));
                self.showMessage(self.labelMessages.ALL_LINES); 
            }
        });
    },

    // FUNCTION CALLED AFTER MAP LOADED
    afterPTFeaturesAdded: function() {
    
        console.log("TisseoMap::afterPTFeaturesAdded");

      var self = this;
  
      this.markers = new OpenLayers.Layer.Markers("markerLayer");
      this.map.addLayer(this.markers); 
      
      this.setColorSimilarLine();
      
      this.saveData();

      this.initAutoComplete();
      
      this.initDefaultEvent();

      this.initLinesSelector();

      //this.initButtonEvents();

      this.initLineChange();
            
    },
    
    // GET GEOMETRY FROM STOP, LINE OR CITY
    getGeometry: function(args) {
    
        console.log("TisseoMap::getGeometry");
        
        var self = this;
        
        // If coordonates are already given, no research
        var stopList = this.searchEngine(args.filter);
        var stopFeatures = [];
        var latLonProj   = new OpenLayers.Projection("EPSG:4326");
    
        // Loop on all stops
        _.each(stopList, function(stop){ 
          
            var point  = new OpenLayers.Geometry.Point(
              parseFloat(stop.x),
              parseFloat(stop.y)
            );

            var pointLatLon = OpenLayers.Projection.transform(
                point.clone(), self.map.getProjectionObject(),
                latLonProj);
                
            stopFeatures.push(
              new OpenLayers.Feature.Vector(
                point, stop
              )
            );
        });
        
        return stopFeatures; 
         
    },
    
    // Overwrites SyntheseMap::getLinesStyleMap to implement getDisplay method to handle display
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
            return "#111";
          // default fall back color
          if (!attrs.line_color)
            return "gray";
          return "rgb" + attrs.line_color;
        },
        getDisplay: function(feature) {
          return (feature.attributes.visible) ? "" : "none";
        }
      };

      var style = new OpenLayers.Style({
        strokeColor : "${getStrokeColor}",
        strokeWidth : "${getStrokeWidth}",
        display     : "${getDisplay}",
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
    var context = {
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

    var style = new OpenLayers.Style({
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
      display: "${getDisplay}"
    }, {
      context: context
    });

    return new OpenLayers.StyleMap({
      'default': style
    });
  }
  
});

return {
  TisseoMap: TisseoMap
};

});
