/** 70_server module wide definitions
	@file module.h
*/

#ifndef SYNTHESE_SERVER_MODULE_H
#define SYNTHESE_SERVER_MODULE_H

#include <string>


namespace synthese
{

	/** @defgroup m70 70 synthese server



	@{
	*/

	/** 70_server namespace */
	namespace server
	{


	    //! Function codes
	    static const std::string FUNCTION_HOME ("ac");
	    static const std::string FUNCTION_SCHEDULE_SHEET ("fh");
	    static const std::string FUNCTION_STOP_DESCRIPTION ("fa");
	    static const std::string FUNCTION_STOP_LIST ("lpa");
	    static const std::string FUNCTION_CITY_LIST ("lc");
	    static const std::string FUNCTION_SCHEDULE_SHEET_VALIDATION ("vfh");
	    static const std::string FUNCTION_STATION_DEPARTURE_TABLE ("tdg");
	    static const std::string FUNCTION_DEPARTURE_TABLE ("td");
	    static const std::string FUNCTION_TINY_DEPARTURE_TABLE ("mtd");
	    static const std::string FUNCTION_RESERVATION_FORM ("fres");
	    static const std::string FUNCTION_RESERVATION_VALIDATION ("vres");
	    static const std::string FUNCTION_RESERVATION_CANCELLING ("ares");

	    static const std::string PARAMETER_FUNCTION ("fonction");
	    static const std::string PARAMETER_SITE ("site");
	    static const std::string PARAMETER_SEARCH ("rec");
	    static const std::string PARAMETER_CITY ("com");
	    static const std::string PARAMETER_CITY_NUMBER ("ncom");
	    static const std::string PARAMETER_DIRECTION ("sens");
	    static const std::string PARAMETER_DATE ("date");
	    static const std::string PARAMETER_PERIOD ("per");
	    static const std::string PARAMETER_TAXIBUS ("tax");
	    static const std::string PARAMETER_BIKE ("vel");
	    static const std::string PARAMETER_HANDICAPPED ("han");
	    static const std::string PARAMETER_PRICE ("tar");
	    static const std::string PARAMETER_DEPARTURE_CITY ("comd");
	    static const std::string PARAMETER_ARRIVAL_CITY ("coma");

	    static const std::string PARAMETER_DEPARTURE_STOP ("ad");
	    static const std::string PARAMETER_ARRIVAL_STOP ("aa");

	    static const std::string PARAMETER_DEPARTURE_CITY_NUMBER ("ncomd");
	    static const std::string PARAMETER_ARRIVAL_CITY_NUMBER ("ncoma");
	    static const std::string PARAMETER_STOP_NUMBER ("npa");
	    static const std::string PARAMETER_DEPARTURE_STOP_NUMBER ("npad");
	    static const std::string PARAMETER_ARRIVAL_STOP_NUMBER ("npaa");

	    static const std::string PARAMETER_DEPARTURE_WORDING_NUMBER ("ndd");
	    static const std::string PARAMETER_ARRIVAL_WORDING_NUMBER ("nda");
	    static const std::string PARAMETER_PROPOSAL_COUNT ("np");
	    static const std::string PARAMETER_LINE_CODE ("lig");
	    static const std::string PARAMETER_SERVICE_NUMBER  ("serv");
	    static const std::string PARAMETER_RESERVATION_CODE ("res");
	    static const std::string PARAMETER_CLIENT_NAME ("Nom");
	    static const std::string PARAMETER_CLIENT_FIRST_NAME ("Prenom");
	    static const std::string PARAMETER_CLIENT_ADDRESS ("Adresse");
	    static const std::string PARAMETER_CLIENT_EMAIL ("Email");
	    static const std::string PARAMETER_CLIENT_PHONE ("Telephone");
	    static const std::string PARAMETER_CLIENT_REGISTRATION_NUMBER ("NumAbonne");
	    static const std::string PARAMETER_DEPARTURE_ADDRESS ("AdresseDepart");
	    static const std::string PARAMETER_ARRIVAL_ADDRESS ("AdresseArrivee");

	    static const std::string PARAMETER_RESERVATION_COUNT ("NbPlaces");
	    static const std::string PARAMETER_DEPARTURE_TABLE_CODE ("tb");

	    static const int UNKNOWN_PARAMETER_VALUE = -1;

	    static const std::string CONFIG_TABLE_NAME ("t999_config");
	    static const std::string CONFIG_TABLE_COL_PARAMNAME ("param_name");
	    static const std::string CONFIG_TABLE_COL_PARAMVALUE ("param_value");

	    static const std::string CONFIG_TABLE_COL_PARAMVALUE_PORT ("port");
	    static const std::string CONFIG_TABLE_COL_PARAMVALUE_NBTHREADS ("nb_threads");
	    static const std::string CONFIG_TABLE_COL_PARAMVALUE_LOGLEVEL ("log_level");
	    static const std::string CONFIG_TABLE_COL_PARAMVALUE_DATADIR ("data_dir");
	    static const std::string CONFIG_TABLE_COL_PARAMVALUE_TEMPDIR ("temp_dir");
	    static const std::string CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPDIR ("http_temp_dir");
	    static const std::string CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPURL ("http_temp_url");
	    static const std::string CONFIG_TABLE_COL_PARAMVALUE_TRIGGERSENABLED ("triggers_enabled");

	    static const std::string TRIGGERS_ENABLED_CLAUSE (
		"(SELECT " + CONFIG_TABLE_COL_PARAMVALUE
		+ " FROM " + CONFIG_TABLE_NAME + " WHERE " + CONFIG_TABLE_COL_PARAMNAME
		+ "=" + CONFIG_TABLE_COL_PARAMVALUE_TRIGGERSENABLED + ")");

	}

	/** @} */

}

#endif
