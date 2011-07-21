window.app = function() {
    /*
     * Private namespace
     */
     
    /**
     * Property: maps
     * {Array} array keeping a reference to the three app.Gis objects instanciated
     */
    var maps = new Array(3);
    
    /**
     * Property: solutionId
     * {Integer} Solution index to display (defaults to the first one)
     */
    var solutionId = 0;
    
    /**
     * Method: refreshDisplay
     * refreshes solution ID indicator
     */
    var refreshDisplay = function() {
        document.getElementById("resultNb").innerHTML = solutionId + 1; 
    };

    /*
     * Public namespace
     */
    return {
        /**
         * Property: setup
         * {Object} Setup namespace
         */
        setup: null,
        
        /**
         * Property: bootstrap
         * {Object} Initialization data we got from server at startup
         */
        bootstrap: null,
        
        /**
         * Method: displayError
         *     Shows a window, with warning text inside
         */
        displayError: function(msg){
            alert(msg);
        },
    
        /**
         * Method: init
         *
         */
        init: function() {
            if (!window.bootstrap)
                return;
            // bootstrap data got from HTML page
            this.bootstrap = OpenLayers.Util.extend({}, bootstrap);
            
            // initialize gis objects
            var G = app.Gis;
            var type = G.type;
            
            // ID is 0 for start and stop maps because the starting and end point do not vary with solutions
            maps[type.START] = new G({div: 'startMap', type: type.START, id: 0}); 
            // commented because it's not available at startup (map div has no dimension)
            //maps[type.ROUTES] = new G({div: 'routesMap', type: type.ROUTES, id: solutionId});
            maps[type.STOP] = new G({div: 'stopMap', type: type.STOP, id: 0});
        },
        
        /**
         * Method: displaySolution
         * display maps corresponding to given routing solution (between 0 and N-1)
         */
        displaySolution: function(id) {
            if ((id > this.bootstrap.routes.length-1) || (id < 0)) {
                return;
            }
            solutionId = id;
            refreshDisplay();
            if (!maps[app.Gis.type.ROUTES]) {
                maps[app.Gis.type.ROUTES] = new app.Gis({
                    div: 'routesMap', 
                    type:app.Gis.type.ROUTES, 
                    id: solutionId
                });
            } else {
                maps[app.Gis.type.ROUTES].refresh(solutionId);
            }
        },
        
        /**
         * Method: prevSolution
         * display maps corresponding to previous routing solution
         */
        prevSolution: function() {
            if (solutionId === 0) {
                return;
            }
            solutionId -= 1;
            refreshDisplay();
            maps[app.Gis.type.ROUTES].refresh(solutionId);
        },
        
        /**
         * Method: nextSolution
         * display maps corresponding to next routing solution
         */
        nextSolution: function() {
            if (solutionId == this.bootstrap.routes.length-1) {
                return;
            }
            solutionId += 1;
            refreshDisplay();
            maps[app.Gis.type.ROUTES].refresh(solutionId);
        },
        
        /**
         * Method: zoomToThisPlace
         * sets map center and zooms to a connection zone (for instance)
         * takes any number of "point strings" as arguments (but at least one),
         * eg: '1.35475,43.62688','1.38702,43.64676'
         */
        zoomToThisPlace: function() {
            maps[app.Gis.type.ROUTES].zoomToThisPlace(arguments);
        },
        
        /**
         * Method: destroy
         *
         */
        destroy: function() {
            for (var i=0, len=maps.length; i<len; i++) {
                if (maps[i]) {
                    maps[i].destroy();
                    maps[i] = null;
                }
            }
        }
    };
}();
