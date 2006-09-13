#ifndef SYNTHESE_ENVLSSQL_MODULE_H
#define SYNTHESE_ENVLSSQL_MODULE_H


#include <string>

	/** @defgroup m17 17 Input : SQLite persistent data loader (with CDM)
		@{


		<h2>Conceptual data model</h2>

		<h3>Cities</h3>

		@copydoc synthese::envlssql::CityTableSync
		@dontinclude CityTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Continuous services</h3>

		@copydoc synthese::envlssql::ContinuousServiceTableSync
		@dontinclude ContinuousServiceTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Line stops</h3>

		@copydoc synthese::envlssql::LineStopTableSync
		@dontinclude LineStopTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Environments links</h3>

		@copydoc synthese::envlssql::EnvironmentLinkTableSync
		@dontinclude EnvironmentLinkTableSync.cpp
		@skip synthese::db::SQLiteTableSync
		@until }

		<h3>Environments</h3>

		@copydoc synthese::envlssql::EnvironmentTableSync
		@dontinclude EnvironmentTableSync.cpp
		@skip synthese::db::SQLiteTableSync
		@until }

		<h3>Transport network</h3>

		@copydoc synthese::envlssql::TransportNetworkTableSync
		@dontinclude TransportNetworkTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Addresses</h3>

		@copydoc synthese::envlssql::AddressTableSync
		@dontinclude AddressTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Alarms</h3>

		@copydoc synthese::envlssql::AlarmTableSync
		@dontinclude AlarmTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Axis</h3>

		@copydoc synthese::envlssql::AxisTableSync
		@dontinclude AxisTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Bike compliance</h3>

		@copydoc synthese::envlssql::BikeComplianceTableSync
		@dontinclude BikeComplianceTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Handicapped compliance</h3>

		@copydoc synthese::envlssql::HandicappedComplianceTableSync
		@dontinclude HandicappedComplianceTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Pedestrian compliance</h3>

		@copydoc synthese::envlssql::PedestrianComplianceTableSync
		@dontinclude PedestrianComplianceTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Connection places</h3>

		@copydoc synthese::envlssql::ConnectionPlaceTableSync
		@dontinclude ConnectionPlaceTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Fares</h3>

		@copydoc synthese::envlssql::FareTableSync
		@dontinclude FareTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Physical stops</h3>

		@copydoc synthese::envlssql::PhysicalStopTableSync
		@dontinclude PhysicalStopTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Place alias</h3>

		@copydoc synthese::envlssql::PlaceAliasTableSync
		@dontinclude PlaceAliasTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Public places</h3>

		@copydoc synthese::envlssql::PublicPlaceTableSync
		@dontinclude PublicPlaceTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Reservation rules</h3>

		@copydoc synthese::envlssql::ReservationRuleTableSync
		@dontinclude ReservationRuleTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Road chunks</h3>

		@copydoc synthese::envlssql::RoadChunkTableSync
		@dontinclude RoadChunkTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Roads</h3>

		@copydoc synthese::envlssql::RoadTableSync
		@dontinclude RoadTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Scheduled Services</h3>

		@copydoc synthese::envlssql::ScheduledServiceTableSync
		@dontinclude ScheduledServiceTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Service circulation dates</h3>

		@copydoc synthese::envlssql::ServiceDateTableSync
		@dontinclude ServiceDateTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h3>Lines</h3>

		@copydoc synthese::envlssql::LineTableSync
		@dontinclude LineTableSync.cpp
		@skip ComponentTableSync
		@until }

		<h2>Loader internals</h2>

		See :
			- class @ref synthese::envlssql::ComponentTableSync "ComponentTableSync"
			- class @ref synthese::envlssql::EnvironmentSyncException "EnvironmentSyncException"
			- module @ref m02
	*/

namespace synthese
{


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
	    static const std::string ADDRESSES_TABLE_COL_PLACEID ("place_id");  // NU
	    static const std::string ADDRESSES_TABLE_COL_ROADID ("road_id");  // NU
	    static const std::string ADDRESSES_TABLE_COL_METRICOFFSET ("metric_offset");  // U ??
	    static const std::string ADDRESSES_TABLE_COL_X ("x");  // U ??
	    static const std::string ADDRESSES_TABLE_COL_Y ("y");  // U ??



	    static const std::string ALARMS_TABLE_NAME ("t002_alarms");
	    static const std::string ALARMS_TABLE_COL_MESSAGE ("message");  
	    static const std::string ALARMS_TABLE_COL_PERIODSTART ("period_start"); 
	    static const std::string ALARMS_TABLE_COL_PERIODEND ("period_end"); 
	    static const std::string ALARMS_TABLE_COL_LEVEL ("level"); 



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
	    static const std::string CONNECTIONPLACES_TABLE_COL_CONNECTIONTYPE ("connection_type");
	    static const std::string CONNECTIONPLACES_TABLE_COL_ISCITYMAINCONNECTION ("is_city_main_connection");
	    static const std::string CONNECTIONPLACES_TABLE_COL_DEFAULTTRANSFERDELAY ("default_transfer_delay");
	    static const std::string CONNECTIONPLACES_TABLE_COL_TRANSFERDELAYS ("transfer_delays");
	    static const std::string CONNECTIONPLACES_TABLE_COL_ALARMID ("alarm_id");
	    
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
	    static const std::string LINES_TABLE_COL_USEINDEPARTUREBOARDS ("use_in_departure_boards");
	    static const std::string LINES_TABLE_COL_USEINTIMETABLES ("use_in_timetables");
	    static const std::string LINES_TABLE_COL_USEINROUTEPLANNING ("use_in_routeplanning");
	    static const std::string LINES_TABLE_COL_ROLLINGSTOCKID ("rolling_stock_id");
	    static const std::string LINES_TABLE_COL_FAREID ("fare_id");
	    static const std::string LINES_TABLE_COL_ALARMID ("alarm_id");
	    static const std::string LINES_TABLE_COL_BIKECOMPLIANCEID ("bike_compliance_id");
	    static const std::string LINES_TABLE_COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
	    static const std::string LINES_TABLE_COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
	    static const std::string LINES_TABLE_COL_RESERVATIONRULEID ("reservation_rule_id");
	    

	    /** Line stops table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string LINESTOPS_TABLE_NAME ("t010_line_stops");
	    static const std::string LINESTOPS_TABLE_COL_PHYSICALSTOPID ("physical_stop_id");
	    static const std::string LINESTOPS_TABLE_COL_LINEID ("line_id");
	    static const std::string LINESTOPS_TABLE_COL_RANKINPATH ("rank_in_path");
	    static const std::string LINESTOPS_TABLE_COL_ISDEPARTURE ("is_departure");
	    static const std::string LINESTOPS_TABLE_COL_ISARRIVAL ("is_arrival");
	    static const std::string LINESTOPS_TABLE_COL_METRICOFFSET ("metric_offset");
	    static const std::string LINESTOPS_TABLE_COL_SCHEDULEINPUT ("schedule_input");
	    static const std::string LINESTOPS_TABLE_COL_VIAPOINTS ("via_points");


	    /** Physical stops table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string PHYSICALSTOPS_TABLE_NAME ("t012_physical_stops");
	    static const std::string PHYSICALSTOPS_TABLE_COL_NAME ("name");
	    static const std::string PHYSICALSTOPS_TABLE_COL_PLACEID ("place_id");
	    static const std::string PHYSICALSTOPS_TABLE_COL_X ("x");
	    static const std::string PHYSICALSTOPS_TABLE_COL_Y ("y");

	    static const std::string PLACEALIASES_TABLE_NAME ("t011_place_aliases");
	    static const std::string PLACEALIASES_TABLE_COL_NAME ("name");
	    static const std::string PLACEALIASES_TABLE_COL_ALIASEDPLACEID ("aliased_place_id");
	    static const std::string PLACEALIASES_TABLE_COL_CITYID ("city_id");
	    static const std::string PLACEALIASES_TABLE_COL_ISCITYMAINCONNECTION ("is_city_main_connection");

	    static const std::string PUBLICPLACES_TABLE_NAME ("t013_public_places");
	    static const std::string PUBLICPLACES_TABLE_COL_NAME ("name");
	    static const std::string PUBLICPLACES_TABLE_COL_CITYID ("city_id");



	    /** Road chunks table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string ROADCHUNKS_TABLE_NAME ("t014_road_chunks");
	    static const std::string ROADCHUNKS_TABLE_COL_ADDRESSID ("address_id");
	    static const std::string ROADCHUNKS_TABLE_COL_RANKINPATH ("rank_in_path");
	    static const std::string ROADCHUNKS_TABLE_COL_ISDEPARTURE ("is_departure");
	    static const std::string ROADCHUNKS_TABLE_COL_ISARRIVAL ("is_arrival");
	    static const std::string ROADCHUNKS_TABLE_COL_VIAPOINTS ("via_points");  // list of ids


	    /** Roads table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string ROADS_TABLE_NAME ("t015_roads");
	    static const std::string ROADS_TABLE_COL_NAME ("name");
	    static const std::string ROADS_TABLE_COL_CITYID ("city_id");
	    static const std::string ROADS_TABLE_COL_ROADTYPE ("road_type");
	    static const std::string ROADS_TABLE_COL_FAREID ("fare_id");
	    static const std::string ROADS_TABLE_COL_ALARMID ("alarm_id");
	    static const std::string ROADS_TABLE_COL_BIKECOMPLIANCEID ("bike_compliance_id");
	    static const std::string ROADS_TABLE_COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
	    static const std::string ROADS_TABLE_COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
	    static const std::string ROADS_TABLE_COL_RESERVATIONRULEID ("reservation_rule_id");
	    // list of chunk ids


	    static const std::string SERVICEDATES_TABLE_NAME ("t005_service_dates");
	    static const std::string SERVICEDATES_TABLE_COL_SERVICEID ("service_id");
	    static const std::string SERVICEDATES_TABLE_COL_DATE("date");



	    static const std::string SCHEDULEDSERVICES_TABLE_NAME ("t016_scheduled_services");
	    static const std::string SCHEDULEDSERVICES_TABLE_COL_SERVICENUMBER ("service_number");
	    static const std::string SCHEDULEDSERVICES_TABLE_COL_SCHEDULES ("schedules");
	    static const std::string SCHEDULEDSERVICES_TABLE_COL_PATHID ("path_id");
	    static const std::string SCHEDULEDSERVICES_TABLE_COL_RANKINPATH ("rank_in_path");
	    static const std::string SCHEDULEDSERVICES_TABLE_COL_BIKECOMPLIANCEID ("bike_compliance_id");
	    static const std::string SCHEDULEDSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
	    static const std::string SCHEDULEDSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
	    static const std::string SCHEDULEDSERVICES_TABLE_COL_RESERVATIONRULEID ("reservation_rule_id");


	    static const std::string CONTINUOUSSERVICES_TABLE_NAME ("t017_continuous_services");
	    static const std::string CONTINUOUSSERVICES_TABLE_COL_SERVICENUMBER ("service_number");
	    static const std::string CONTINUOUSSERVICES_TABLE_COL_SCHEDULES ("schedules");
	    static const std::string CONTINUOUSSERVICES_TABLE_COL_PATHID ("path_id");
	    static const std::string CONTINUOUSSERVICES_TABLE_COL_RANKINPATH ("rank_in_path");
	    static const std::string CONTINUOUSSERVICES_TABLE_COL_RANGE ("range");
	    static const std::string CONTINUOUSSERVICES_TABLE_COL_MAXWAITINGTIME ("max_waiting_time");
	    static const std::string CONTINUOUSSERVICES_TABLE_COL_BIKECOMPLIANCEID ("bike_compliance_id");
	    static const std::string CONTINUOUSSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
	    static const std::string CONTINUOUSSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
	    //... 
	    
	    static const std::string PEDESTRIANCOMPLIANCES_TABLE_NAME ("t018_pedestrian_compliances");
	    static const std::string PEDESTRIANCOMPLIANCES_TABLE_COL_STATUS ("status");
	    static const std::string PEDESTRIANCOMPLIANCES_TABLE_COL_CAPACITY ("capacity");
	    
	    static const std::string HANDICAPPEDCOMPLIANCES_TABLE_NAME ("t019_handicapped_compliances");
	    static const std::string HANDICAPPEDCOMPLIANCES_TABLE_COL_STATUS ("status");
	    static const std::string HANDICAPPEDCOMPLIANCES_TABLE_COL_CAPACITY ("capacity");
	    
	    static const std::string BIKECOMPLIANCES_TABLE_NAME ("t020_bike_compliances");
	    static const std::string BIKECOMPLIANCES_TABLE_COL_STATUS ("status");
	    static const std::string BIKECOMPLIANCES_TABLE_COL_CAPACITY ("capacity");
	    
	    static const std::string TRANSPORTNETWORKS_TABLE_NAME ("t022_transport_networks");
	    static const std::string TRANSPORTNETWORKS_TABLE_COL_NAME ("name");

	    static const std::string RESERVATIONRULES_TABLE_NAME ("t021_reservation_rules");
	    static const std::string RESERVATIONRULES_TABLE_COL_TYPE ("reservation_type");
	    static const std::string RESERVATIONRULES_TABLE_COL_ONLINE ("online");
	    static const std::string RESERVATIONRULES_TABLE_COL_ORIGINISREFERENCE ("origin_is_reference");
	    static const std::string RESERVATIONRULES_TABLE_COL_MINDELAYMINUTES ("min_delay_minutes");
	    static const std::string RESERVATIONRULES_TABLE_COL_MINDELAYDAYS ("min_delay_days");
	    static const std::string RESERVATIONRULES_TABLE_COL_MAXDELAYDAYS ("max_delay_days");
	    static const std::string RESERVATIONRULES_TABLE_COL_HOURDEADLINE ("hour_deadline");
	    static const std::string RESERVATIONRULES_TABLE_COL_PHONEEXCHANGENUMBER ("phone_exchange_number");
	    static const std::string RESERVATIONRULES_TABLE_COL_PHONEEXCHANGEOPENINGHOURS ("phone_exchange_opening_hours");
	    static const std::string RESERVATIONRULES_TABLE_COL_DESCRIPTION ("description");
	    static const std::string RESERVATIONRULES_TABLE_COL_WEBSITEURL ("web_site_url");



	}


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

		/** @} */

#endif
