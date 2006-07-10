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

	    typedef enum { ENVIRONMENT_CLASS = 0, 
			   AXIS_CLASS,
			   CITY_CLASS,
			   LINE_CLASS,
			   LINESTOP_CLASS,
			   CONNECTIONPLACE_CLASS,
			   PHYSICALSTOP_CLASS } ComponentClass;


	    static const std::string TABLE_COL_ID ("id");

	    static const std::string ENVIRONMENTS_TABLE_NAME ("tbl_environments");
	    static const std::string ENVIRONMENTS_TABLE_COL_LINKTABLE ("link_table");

	    static const std::string ENVLINKS_TABLE_COL_LINKCLASS ("link_class");
	    static const std::string ENVLINKS_TABLE_COL_LINKID ("link_id");



	    /** Addresses table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string ADDRESSES_TABLE_NAME ("tbl_addresses");
	    static const std::string ADDRESSES_TABLE_COL_CONNECTIONPLACEID ("connection_place_id");  // NU
	    static const std::string ADDRESSES_TABLE_COL_RANKINCONNECTIONPLACE ("connection_place_rank"); // NU
	    static const std::string ADDRESSES_TABLE_COL_ROADID ("road_id");  // NU
	    static const std::string ADDRESSES_TABLE_COL_METRICOFFSET ("metric_offset");  // U ??


	    /** Axes table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string AXES_TABLE_NAME ("tbl_axes");


	    /** Cities table :
		- on insert : insert entry in associator
		- on update : update entry in associator
		- on delete : X
	     */
	    static const std::string CITIES_TABLE_NAME ("tbl_cities");
	    static const std::string CITIES_TABLE_COL_NAME ("name");


	    /** Lines table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string LINES_TABLE_NAME ("tbl_lines");


	    /** Line stops table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string LINESTOPS_TABLE_NAME ("tbl_line_stops");


	    /** Connection places table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string CONNECTIONPLACES_TABLE_NAME ("tbl_connection_places");


	    /** Physical stops table :
		- on insert : 
		- on update : 
		- on delete : X
	     */
	    static const std::string PHYSICALSTOPS_TABLE_NAME ("tbl_physical_stops");


	}

	/** @} */

}

#endif
