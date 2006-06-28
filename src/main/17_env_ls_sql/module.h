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
			   LOGICALSTOP_CLASS,
			   PHYSICALSTOP_CLASS } ComponentClass;


	    static const std::string TABLE_COL_ID ("id");

	    static const std::string ENVIRONMENTS_TABLE_NAME ("tbl_environments");
	    static const std::string ENVIRONMENTS_TABLE_COL_LINKTABLE ("link_table");

	    static const std::string AXES_TABLE_NAME ("tbl_axes");

	    static const std::string CITIES_TABLE_NAME ("tbl_cities");
	    static const std::string CITIES_TABLE_COL_NAME ("name");

	    static const std::string LINES_TABLE_NAME ("tbl_lines");

	    static const std::string LINESTOPS_TABLE_NAME ("tbl_line_stops");

	    static const std::string LOGICALSTOPS_TABLE_NAME ("tbl_logical_stops");

	    static const std::string PHYSICALSTOPS_TABLE_NAME ("tbl_physical_stops");

	    static const std::string ENVLINKS_TABLE_COL_LINKCLASS ("link_class");
	    static const std::string ENVLINKS_TABLE_COL_LINKID ("link_id");


	}

	/** @} */

}

#endif
