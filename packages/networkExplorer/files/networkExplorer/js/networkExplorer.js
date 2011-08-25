/* ************

Define the events for search form

************* */
var TisseoMap = OpenLayers.Class(SyntheseMap, {

    dataList: {
        stops  : [],
        lines  : [],
        cities : []
    },
    //dataList: [],   
    
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
            
            _.map(self.linesLayer.features, function(feature) {
                var line_attr = feature.attributes;
                if (!founded && parseInt(line_attr.line_short_name) == parseInt(line.line_short_name) && line_attr.line_color && line.line_short_name != line_attr.line_short_name)
                    founded = line_attr.line_color;
            });
            
            return (founded) ? founded : "transparent";
        }
    
        var self    = this;
        var tabMade = [];
    
        _.map(this.stopsLayer.features, function(featureStop) {
            _.map(featureStop.attributes.lines, function(feature) {
                if(!feature.line_color && !_.include(tabMade, feature.line_short_name)){
                        feature.line_color = findSimilarLine(feature);
                        tabMade[feature.line_short_name] = feature.line_color;
                }
            });
        });
        
        _.map(this.linesLayer.features, function(line) {
               if(tabMade[line.attributes.line_short_name])
                    line.attributes.line_color = tabMade[line.attributes.line_short_name];
        });
        
    },
    
    // DISPLAY OR NOT A MAP'S FEATURE ACCORDING TO ARGS
    setFeatureVisible: function (feature, visible) {
            feature.attributes.display = visible;
    },
    
    // EMPTY FILTER AREA
    emptyFilter: function(){
        $("#content_line_filter_area").empty();
    },
    
    // SHOW LINES FROM A GIVEN LIST
    showListLines: function(selectedLineIds){
        console.log("TisseoMap::showListLines");
        
        var self = this;
        _.map(this.linesLayer.features, function(line) {
                self.setFeatureVisible(line, (selectedLineIds == "*") ? true : selectedLineIds[line.attributes.id]);
        });
        this.linesLayer.redraw();
    },
  
    // SHOW STOPS FROM A GIVEN LIST
    showListStops: function(selectedStopIds){
        console.log("TisseoMap::showListStops");
        var self = this;
        _.map(this.stopsLayer.features, function(stop) {
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
        linesList = _(linesList).chain().map(function(line) {
          line.color        =  line.line_color && ("rgb" + line.line_color);
          line.color_text   = (line.color != "transparent") ? "white" : "black";
          // If a color switch is needed
          if(isThereAswitch){
              line.color_switch = (switcher % 2 == 0) ? "color_switch" : "";
              switcher++;
          }
          return line;
        }).value();
        
        return linesList;
    },
    
    updatePTFeatures: function() {
                
              console.log("TisseoMap::updatePTFeatures");
                
              var selectedLineIds = [];
              var selectedStopIds = [];
              $("#content_line_filter_area input:checked").each(function(index, checkbox) {
                // Warning: don't use jQuery.data to fetch the lineid attribute,
                // otherwise it will be converted to a number with its value truncated
                // to JavaScript number precision.
                selectedLineIds[$(checkbox).attr("data-lineid")] = true;
              });
              
              // Recovering stops on each specified line
              _.map(this.stopsLayer.features, function(stop) {
                        var visible = _.any(stop.attributes.lines, function(line) {
                          return selectedLineIds[line.id];
                        });

                        selectedStopIds[stop.attributes.id] = visible;
              });
              
              // Show lines & stops
            this.showListStops(selectedStopIds);
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

        for(id in linesList)
            //if(linesList[id].linename.length > 20)
            if(linesList[id].lineName.length > 20)
               //linesList[id].linename = linesList[id].linename.substr(0,20) + "...";
               linesList[id].lineName = linesList[id].lineName.substr(0,20) + "...";
        
        $("#linesTemplate").tmpl(this.displayInFilter(linesList)).appendTo("#content_line_filter_area");
        
        $("input[isCheckLine=true]").click(function(event) {
          self.updatePTFeatures();
        });
        
        this.updatePTFeatures();
    
    },
    
    // INSERT DATA ABOUT LINES AND STOPS IN AN ASSOCIATIVE ARRAY
    saveData: function(){
    
        console.log("TisseoMap::saveData");
    
        var self    = this;
        var tabMade = [];
        
        // FIND A SIMILAR SHORT NAME LINE (10 <=> 10s) TO RETURN A VALID COLOR
        function findSimilarLine(line) {
        
            var founded = null;
                
            _.map(self.linesLayer.features, function(feature) {
                var line_attr = feature.attributes;
                if (!founded && parseInt(line_attr.line_short_name) == parseInt(line.line_short_name) && line_attr.line_color && line.line_short_name != line_attr.line_short_name)
                    founded = line_attr.line_color;
            });
                    
            return (founded) ? founded : "transparent";
        }
        
        // Insert data in a custom data template
        // Stop's data
        _.map(this.stopsLayer.features, function(feature) {
            // Add stop
            self.dataList.stops[feature.attributes.id] = feature.attributes;
            // Add city
            if(!self.dataList.cities[feature.attributes.city_id]){
                self.dataList.cities[feature.attributes.city_id] = {
                    name: feature.attributes.city_name,
                    stops: []
                }
            }
            self.dataList.cities[feature.attributes.city_id].stops.push(feature.attributes.id);
            // Loop on all lines contained in this stop
            _.map(feature.attributes.lines, function(line) {
            
                // Find color for special lines (As XXs, i.e : 10s)
                if(!line.line_color && !_.include(tabMade, line.line_short_name)){
                     line.line_color = findSimilarLine(line);
                     tabMade[line.line_short_name] = line.line_color;
                }
                // Insert data in custom structure
                if(!self.dataList.lines[line.id]){
                    self.dataList.lines[line.id] = {
                        attr: line,
                        stops: []
                    }
                }
                // Add current stop to current line
                self.dataList.lines[line.id].stops.push(feature.attributes.id);
            });
        });
       
        // Add background for special lines
        _.map(this.linesLayer.features, function(line) {
            if(tabMade[line.attributes.line_short_name])
                line.attributes.line_color = tabMade[line.attributes.line_short_name];
        });
       
        console.log(this.dataList);
        
    },
    
    // INITALIZE COMPLETION DIV TO REPLACE AUTOCOMPLETE PLUGIN JQUERY
    createCompletionDiv: function(data_adress) {
    
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
		        heightResult += (heightResult < 100) ? 20 : 0;
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
        createLiList("Arrêts", data_adress.stop);
        createLiList("Adresses", data_adress.street);
        
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
	    
	    // Load events for line selector
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
		
    },
    
    // INITALIZE AUTO COMPLETION FOR STOPS AND CITIES
    initAutoComplete: function() {
    
        console.log("TisseoMap::initAutoComplete");
        var self = this;
    
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
                      si       : 7036874417766402,
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
                        if($(this).attr("type") == "street")
                            data_adress_temp = data_adress.street;
                        if($(this).attr("type") == "stop")
                            data_adress_temp = data_adress.stop;
                        data_adress_temp.push({
                            name: $(this).attr("name"),
                            pos: {
                                x: $(this).attr("x"), 
                                y: $(this).attr("y")
                            }
                        });
                          // Create autoCompletion on adress div
                          self.createCompletionDiv(data_adress);                                                   
                      });
                });
            });   
        }
        
        function searchNewStops(){
        
            console.log("TisseoMap::initAutoComplete::searchNewStops");
        
            // Find city id according to the town's name given
            function findCityId(nameCityStop){
                var idReturn = null;
                // If a city is defined and founded
                if(nameCityStop){
                    for(id in self.dataList.cities)
                        if(self.dataList.cities[id].name == nameCityStop)
                            idReturn = id;
                }
                return idReturn;
            }
            
            var nameCityStop = $('div[name="stop"]').find("input[name=name_stop_city]").val();
            var idCity       = findCityId(nameCityStop);
            var data_stop    = [];
            
            // Searching stop
            for(id in self.dataList.stops)
                if((idCity && self.dataList.stops[id].cityId == idCity) || !idCity)
                    data_stop.push(self.dataList.stops[id].name);
            
            console.log(data_stop);
            return data_stop;
        }
    
        var data_stop   = [];
        var data_cities = [];
        var list_data_cities = [];
         
        // Recovering stop names
         for(id in this.dataList.stops)
          data_stop.push(this.dataList.stops[id].name);
        //data_stop = _.select(dataList, function(stop){ return stop.type == "stop" });
          
        // Recovering city names
        for(id in this.dataList.cities){
          data_cities.push(this.dataList.cities[id].name);
          list_data_cities.push({name: this.dataList.cities[id].name});
        }
        
        //data_cities = _.select(dataList, function(city){ return city.type == "city" });
          
        initCompletionAdress();
        
        // Change autocompletion for stops chen cities are changing
        $('input[name="name_stop"]').click(function (){
            $('input[name="name_stop"]').setOptions({ data: searchNewStops() });
        });
        
        // Change autocompletion for stops chen cities are changing
        $('input[name="name_adress"]').click(function (){
            var isCtName = _.include(data_cities, $('[name="name_adress_city"]').val());
            $('[name="name_adress"]').attr("disabled", !isCtName); 
            if(!isCtName)
                self.showMessage("Le nom de commune '"+$('[name="name_adress_city"]').val()+"' n'est pas valide !");
        });
        
        // Add event onResult on town's form
        $('[name="name_adress_city"]').result(function() {
            var isCtName = _.include(data_cities, $('[name="name_adress_city"]').val());
            $('[name="name_adress"]').attr("disabled", !isCtName); 
            if(!isCtName)
                self.showMessage("Le nom de commune '"+$('[name="name_adress_city"]').val()+"' n'est pas valide !");
        });
        
        // Add autocompletion to specific fields
        $('input[name="name_stop_city"]').autocomplete(data_cities, {
            matchContains: true,
            autoFill: true
        });
        
        $('input[name="name_adress_city"]').autocomplete(data_cities, {
            matchContains: true,
            autoFill: true
        });
        
        $('input[name="name_stop"]').autocomplete(searchNewStops(), {
            matchContains: true
        });
        
        $('input[name="name_city"]').autocomplete(data_cities, {
            matchContains: true,
            autoFill: true
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
    
        var linesList = [];
        var self      = this;
    
        for(id in this.dataList.lines)
          linesList.push(this.dataList.lines[id].attr);
          
        var switcher = 0;
        // Sort and update background and font colors
        linesList = this.displayInFilter(linesList, true);
    
        $("#optionLinesTemplate").tmpl(linesList).appendTo("ul[name='name_line']");
        
        // Load events for line selector
        $(".dropdown dd ul li").click(function() {
            $(".dropdown dt a span").css("min-height", "37px");
            var text = $(this).html();
            $(".dropdown dt a span").html(text);
            $(".dropdown dd ul").hide();
        }); 
        
        $(".dropdown dt a").click(function() {
            $(".dropdown dd ul").toggle();
        });
        
        $(document).bind('click', function(e) {
            var $clicked = $(e.target);
            if (! $clicked.parents().hasClass("dropdown"))
                $(".dropdown dd ul").hide();
        });
    },
    
    // RECOVER CITY'S EXTENT TO ZOOM IN
    getExtent: function(args) {

        console.log("TisseoMap::getExtent");
    
        if (this.extentCache[args.cityId])
          return this.extentCache[args.cityId];
          
        if (this.extentCache[args.lineId])
          return this.extentCache[args.lineId];
          
        var id = null;

        if(args.cityId)
            id = args.cityId;
            
        if(args.lineId)
            id = args.lineId;
            
        if(args.stopId)
            id = args.stopId;

         stopFeatures = this.getGeometry(args);
        
          if (stopFeatures.length == 0)
            return null;

          var extent = new OpenLayers.Bounds();
          _.each(stopFeatures, function(stopFeature) {
            extent.extend(stopFeature.geometry);
          });

          return this.extentCache[id] = extent;
    },
    
    zoomToExtentTisseo: function(args){
    
        console.log("TisseoMap::zoomToExtentTisseo");
    
        var extent = this.getExtent(args);
        if (extent)
           this.map.zoomToExtent(extent);
    },
    
    // CHECK GIVEN FIELDS FROM A SEARCH FORM
    checkData: function(fields) {
        console.log("TisseoMap::checkData");
        if (_.include(fields, ""))
            return false;
        return true;
    },
    
    initCityEvent: function() {
        var self = this;
        $('div[name="city"]').find(":button").bind('click', function(event) {
           // recover town's name
           var nameTown = $(this).parent().find(":text").val();
           // Check form
           if(!self.checkData([nameTown]))
                return;
           var citySelected = null;
           // Seek City's id
           for(id in self.dataList.cities)
              if(self.dataList.cities[id].name == nameTown)
                citySelected = id;
           if(!citySelected){
                self.showMessage("Aucun résultat !");
                return;   
           }                
           // Empty filter
           self.emptyFilter();
           // Zoom to city
           self.zoomToExtentTisseo({cityId: citySelected});   
           // Show stops from the city
           var selectedStopIds = {};
           
              _.map(self.stopsLayer.features, function(stop) {
                for(id in self.dataList.cities[citySelected].stops)
                    if(self.dataList.cities[citySelected].stops[id] == stop.attributes.id)
                        selectedStopIds[stop.attributes.id] = true;
              });
              
              self.showListStops(selectedStopIds);
              self.showListLines([]);
           // Show town's name
           self.showMessage(nameTown);
        });
    },
    
    // INITIALIZE STOP EVENT
    initStopEvent: function() {
    
        var self = this;
        $('div[name="stop"]').find(":button").bind('click', function() {
            var nameStop = $('div[name="stop"]').find("input[name=name_stop]").val();
            
            // Check form
            if(!self.checkData([nameStop]))
                return;
                
            var idStop = null;
            
            for(id in self.dataList.stops)
              if(self.dataList.stops[id].name == nameStop)
                idStop = id;
                                
            if(!idStop){
                self.showMessage("Aucun résultat !");
                return;   
            }
            // Zoom to city
            self.zoomToExtentTisseo({stopId: idStop});   
            // Display line choosen
            self.fillFilter(self.dataList.stops[idStop].lines);
            // Show Place's informations
            self.showMessage("Arrêt " + nameStop + " à "+ self.dataList.stops[idStop].cityName);
        });
    },
    
    // INITIALIZE LINE EVENT
    initLineEvent: function() {
        var self       = this;
        // Add event to search button
        $('div[name="line"]').find(":button").bind('click', function(event) {
            var linesToShow = [];
           // recover town's name
           var idLine = $(this).parent().find("dt div[name='id_line_filter_area']").attr("data-lineid");
           // Check form
           if(!self.checkData([idLine]))
                return;
           if(!idLine){
                self.showMessage("Aucun résultat !");
                return;   
           }
           // Zoom to city
           self.zoomToExtentTisseo({lineId: idLine});   
           // Display line choosen
           linesToShow.push(self.dataList.lines[idLine].attr);
           self.fillFilter(linesToShow);
           // Show Place's informations
           self.showMessage($(this).parent().find("dt span").html());
        });
    },
    
    // INITIALIZE ADRESS EVENT
    initAdressEvent: function() {
        var self       = this;
        
        // Add event to search button
        $('div[name="adress"]').find(":button").bind('click', function(event) {
           // Recover place's name
           var nameStreet = $(this).parent().find("input[name='name_adress']").val();
           var x          = $("input[name='name_adress_x']").val();
           var y          = $("input[name='name_adress_y']").val();
           if(!self.checkData([nameStreet, x, y])){
                self.showMessage("Aucun résultat !");
                return;
           }
           // Zoom to given position
           self.map.setCenter(
              new OpenLayers.LonLat(x, y).
                transform(new OpenLayers.Projection("EPSG:4326"),
                  self.map.getProjectionObject()),
              15);
           // Erase lines 
           self.showListLines([]);
           // Show Place's informations
           self.showMessage(nameStreet + " à " + $(this).parent().find("input[name='name_adress_city']").val());
        });
    },
    
    // SHOWS POPUP WHEN A STOP IS SELECTED
    onStopSelected: function(feature) {
    
        console.log("TisseoMap::onStopSelected");
    
        var self = this;
    
        function recoverJourneys(feature) {
        
            function constructJourney(journeyDocument) {
            
                var cpt  = 0;
                var textJourney  = "<div class='contentStopJourney'>";
                textJourney     += "<div class='nameStopJourney'>";
                textJourney     += "Arrêt : <b>" + feature.attributes.stop_name;
                textJourney     += "</b> Commune : <b>" + feature.attributes.city_name + "</b>";
                textJourney     += "</div>";
                textJourney     += "<div class='titleJourneyStop'>";
                textJourney     += "<div class='cellTitleJourneyStop'>Départs</div>";
                textJourney     += "<div class='cellTitleJourneyStop'>Lignes</div>";
                textJourney     += "<div class='cellTitleJourneyStop'>Destinations</div>";
                textJourney     += "</div>";
              
                // Loop on all journey given by Synthese
                $(journeyDocument).find("journey").each(function() {
                    
                    if(cpt < 6){
                        var time = $(this).attr("dateTime").split(" ");
                        time = time[1];
                        var dest = $(this).find("destination").attr("cityName") + "/" + $(this).find("destination").attr("name");
                        var switchClass = (cpt % 2 == 0) ? "switchCellJourneyStop" : "";
                        
                        textJourney += "<div class='cellJourneyStop "+switchClass+"'>";  
                        textJourney += "<div class='textInJourneyStop'>" + time + "</div>";
                        textJourney += "<div class='number_line_filter_area number_line_journey_stop' style='background-color: rgb"+tabColors[$(this).find("line").attr("shortName")]+"; color: white'>"+$(this).find("line").attr("shortName")+"</div>";
                        textJourney += "<div class='textInJourneyStop'>" + dest + "</div>";;
                        textJourney += "</div>";
                    }
                    cpt++;
                });
                
                textJourney += "</div>";
                
                return textJourney;
                
            }
        
            var tabColors = [];
        
            // Insert RGB colors of lines in tab
            _.map(feature.attributes.lines, function(line){ 
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

            // Creates Popup Content
            var text  = "";
        
            // Add marker on the map
            addMarker(ll, "<img src='http://srv-was12/synthese/load.gif' />", true, true);
        
            // Today's date
            var date = new Date();
            var date2st = date.getFullYear() + "-" + (date.getMonth()+1) + "-" + (date.getDay()+1) + " " + (date.getHours()+1) + ":" + (date.getMinutes()+1);
            
            var args = {
                    si     : 7036874417766402,
                    ni     : 6192449487677451,
                    // Date format : YYYY-MM-DD HH:MiMi i.e -> 2011-08-22 14:35
                    date   : date2st,
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
                 
                 text += "<div class='tabJourneyStop'>";
                 text += constructJourney(journeyDocument);
                 text += "</div>";
                 
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
                //'displayClass': "contentPopupJourney"
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
    
    // SHOW A TEMPORARY MESSAGE ON THE MAP
    showMessage: function(text) {
        console.log("TisseoMap::showMessage");
        // Load Text in div
        $('#message_box').html(text);
        // Show message
        $('#message_box').fadeIn('slow');
        // Hide emssage after 5 sec
        setTimeout("$('#message_box').fadeOut()", 5000);
    },

    // INITIALIZE EVENTS ON BUTTONS TO SEE ALL STOPS OR LINES
    initButtonEvents: function(){
        
        console.log("TisseoMap::initButtonEvents");
    
        var self = this;
        var dataListTemp = [];
        
        for(id in this.dataList.lines)
          dataListTemp.push(this.dataList.lines[id].attr);
        
        $('[name*="see_all"]').find(":button").bind('click', function(event) {
            if($(this).parents().attr("name") == "see_all_stops"){
                self.showListStops(self.dataList.stops);
                self.showMessage("Tous les arrêts !");   
            }
            if($(this).parents().attr("name") == "see_all_lines"){
                self.showListLines(self.dataList.lines);
                self.fillFilter(self.displayInFilter(dataListTemp));
                self.showMessage("Toutes les lignes !"); 
            }
        });
    },

    // FUNCTION CALLED AFTER MAP LOADED
    afterPTFeaturesAdded: function() {
    
        console.log("TisseoMap::afterPTFeaturesAdded");

        this.markers = new OpenLayers.Layer.Markers("markerLayer");
        this.map.addLayer(this.markers); 
        
        this.setColorSimilarLine();
        
        this.saveData();

        this.initAutoComplete();

        this.initLinesSelector();
        
        this.initCityEvent();
        
        this.initLineEvent();
        
        this.initStopEvent();
        
        this.initAdressEvent();
        
        this.initButtonEvents();
        
        this.initLineChange();
            
    },
    
    // GET GEOMETRY FROM STOP, LINE OR CITY
    getGeometry: function(args) {
    
        console.log("TisseoMap::getGeometry");
    
        var stopList     = [];
        var stopFeatures = [];
        var latLonProj   = new OpenLayers.Projection("EPSG:4326");
        
        // Recover specific stop List
        if(args.cityId)
            stopList = this.dataList.cities[args.cityId].stops;
        if(args.lineId)
            stopList = this.dataList.lines[args.lineId].stops;
        if(args.stopId)
            stopList = [args.stopId];
    
        // Recover default stop list
        var stopData = this.dataList.stops;
    
        var self = this;
    
        // Loop on all stops
        _.map(stopList, function(stop){ 
            var point  = new OpenLayers.Geometry.Point(
              parseFloat(stopData[stop].x),
              parseFloat(stopData[stop].y)
            );

            var pointLatLon = OpenLayers.Projection.transform(
                point.clone(), self.map.getProjectionObject(),
                latLonProj);
                
            stopFeatures.push(
              new OpenLayers.Feature.Vector(
                point, stopData[stop]
              )
            );
        });
        
        return stopFeatures; 
         
    }
  
});
