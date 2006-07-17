#ifndef SYNTHESE_ENVLSSQL_MODULE_H
#define SYNTHESE_ENVLSSQL_MODULE_H


#include <string>

namespace synthese
{

	/** @defgroup m17 17 Database wrappers


		@{
	*/

	/** 17_env_ls_sql namespace */
	namespace envlssql
	{

	    static const std::string TABLE_COL_ID ("id");

	    static const std::string ENVIRONMENTS_TABLE_NAME ("t000_environments");

	    static const std::string ENVIRONMENT_LINKS_TABLE_NAME ("t001_environment_links");
	    static const std::string ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID ("environment_id");
	    static const std::string ENVIRONMENT_LINKS_TABLE_COL_LINKTARGETID ("link_target_id");



	    /** Addresses table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string ADDRESSES_TABLE_NAME ("t002_addresses");
	    static const std::string ADDRESSES_TABLE_COL_CONNECTIONPLACEID ("connection_place_id");  // NU
	    static const std::string ADDRESSES_TABLE_COL_RANKINCONNECTIONPLACE ("rank_in_connection_place"); // NU
	    static const std::string ADDRESSES_TABLE_COL_ROADID ("road_id");  // NU
	    static const std::string ADDRESSES_TABLE_COL_METRICOFFSET ("metric_offset");  // U ??
	    static const std::string ADDRESSES_TABLE_COL_X ("x");  // U ??
	    static const std::string ADDRESSES_TABLE_COL_Y ("y");  // U ??


	    /** Axes table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string AXES_TABLE_NAME ("t004_axes");
	    static const std::string AXES_TABLE_COL_NAME ("name");
	    static const std::string AXES_TABLE_COL_FREE ("free");
	    static const std::string AXES_TABLE_COL_ALLOWED ("allowed");

	    /** Cities table :
		- on insert : insert entry in associator
		- on update : update entry in associator
		- on delete : X
	     */
	    static const std::string CITIES_TABLE_NAME ("t006_cities");
	    static const std::string CITIES_TABLE_COL_NAME ("name");


	    /** Connection places table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string CONNECTIONPLACES_TABLE_NAME ("t007_connection_places");
	    static const std::string CONNECTIONPLACES_TABLE_COL_NAME ("name");
	    static const std::string CONNECTIONPLACES_TABLE_COL_CITYID ("city_id");

	    
	    static const std::string FARES_TABLE_NAME ("t008_fares");
	    static const std::string FARES_TABLE_COL_NAME ("name");
	    static const std::string FARES_TABLE_COL_FARETYPE ("fare_type");
	    

	    /** Documents table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    // static const std::string DOCUMENTS_TABLE_NAME ("tbl_documents");


	    /** Lines table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string LINES_TABLE_NAME ("t009_lines");
	    static const std::string LINES_TABLE_COL_TRANSPORTNETWORKID ("transport_network_id");
	    static const std::string LINES_TABLE_COL_AXISID ("axis_id");
// 	    static const std::string LINES_TABLE_COL_CALENDARID ("calendar_id");   // Non car calculé
	    static const std::string LINES_TABLE_COL_NAME ("name");
	    static const std::string LINES_TABLE_COL_SHORTNAME ("short_name");
	    static const std::string LINES_TABLE_COL_LONGNAME ("long_name");
	    static const std::string LINES_TABLE_COL_COLOR ("color");
	    static const std::string LINES_TABLE_COL_STYLE ("style");
	    static const std::string LINES_TABLE_COL_IMAGE ("image");
	    static const std::string LINES_TABLE_COL_TIMETABLENAME ("timetable_name");
	    static const std::string LINES_TABLE_COL_DIRECTION ("direction");
	    static const std::string LINES_TABLE_COL_ISWALKINGLINE ("is_walking_line");
	    static const std::string LINES_TABLE_COL_DEPARTUREBOARDDISPLAY ("departure_board_display");
	    static const std::string LINES_TABLE_COL_TIMETABLEDISPLAY ("timetable_display");
	    // list of services
	    // list of edges

	    

	    /** Line stops table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string LINESTOPS_TABLE_NAME ("t010_line_stops");
	    static const std::string LINESTOPS_TABLE_COL_FROMPHYSICALSTOPID ("from_physical_stop_id");
	    static const std::string LINESTOPS_TABLE_COL_LINEID ("line_id");
	    static const std::string LINESTOPS_TABLE_COL_METRICOFFSET ("metric_offset");
	    static const std::string LINESTOPS_TABLE_COL_SCHEDULEINPUT ("schedule_input");
	    // next in path
	    // previsous conn departure
	    // folloing conn ...
	    // via points



	    /** Physical stops table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string PHYSICALSTOPS_TABLE_NAME ("t012_physical_stops");
	    static const std::string PHYSICALSTOPS_TABLE_COL_CONNECTIONPLACEID ("connection_place_id");
	    static const std::string PHYSICALSTOPS_TABLE_COL_RANKINCONNECTIONPLACE ("rank_in_connection_place");
	    static const std::string PHYSICALSTOPS_TABLE_COL_X ("x");
	    static const std::string PHYSICALSTOPS_TABLE_COL_Y ("y");


	    /** Road chunks table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string ROADCHUNKS_TABLE_NAME ("t014_road_chunks");
	    static const std::string ROADCHUNKS_TABLE_COL_FROMADDRESSID ("from_address_id");
	    static const std::string ROADCHUNKS_TABLE_COL_VIAPOINTS ("via_points");  // list of ids


	    /** Roads table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string ROADS_TABLE_NAME ("t015_roads");
	    static const std::string ROADS_TABLE_COL_NAME ("name");
	    static const std::string ROADS_TABLE_COL_CITY ("city_id");
	    static const std::string ROADS_TABLE_COL_ROADTYPE ("road_type");
	    // list of chunk ids


	    static const std::string SCHEDULEDSERVICES_TABLE_NAME ("t016_scheduled_services");


	    static const std::string CONTINUOUSSERVICES_TABLE_NAME ("t017_continuous_services");

	    //... 
	    
	    

	}

	/** @} */

	/*
	  questions:

	  1) next connection/departure/arrival in path : est ce que ca a un sens pour les road chunks ?
	  si non, splitter roadchunk et linestop;

              oui ca en a un; on garde et on factorise
	  
	  2) format de stockage en base des schedules/services. table a part pour les schedules ?

	      les services sont stockés dans une table à part. les horaires d'un service sont une propriété
	      (un champ) de cette table. le calendrier du service est stocké sous forme d'une liste de dates
	      séparées par des virgules.
	  
	  3) est ce que le service a besoin de connaitre son path (line/road) ? si oui calculateur fonctionne
	  que sur des services ? si non virer l'attribut.

	      a priori oui meme si on sait pas pourquoi ...

	  4) le service connait son calendrier. pourquoi la ligne a-t-elle aussi un calendrier ???

	      pour des raisons d'optim : le calendrier porté par la ligne est le ou logique de tous les
	      calendriers des services de la ligne

	*/


}

#endif
