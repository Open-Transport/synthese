
#ifndef SYNTHESE_ServerModule_H__
#define SYNTHESE_ServerModule_H__

#include <boost/filesystem/path.hpp>

#include "01_util/ModuleClass.h"

#include "30_server/Site.h"
#include "30_server/ServerConfig.h"

/** @defgroup m30 Server
	
	@{
*/
namespace synthese
{
	namespace db
	{
		class SQLiteThreadExec;
	}

	namespace server
	{
		class Session;

		/** SYNTHESE Server module.
		*/
		class ServerModule : public util::ModuleClass
		{
		public:
			typedef std::map<std::string, Session*> SessionMap;

		private:
			static Site::Registry			_sites;
			static ServerConfig				_config;
			static SessionMap				_sessionMap;
			static db::SQLiteThreadExec*	_sqliteThreadExec;

		public:
			/** Standard module initializer, launched as the ones from others modules at the server opening.
			*/
			void initialize();

			static Site::Registry& getSites();
			static ServerConfig& getConfig();
			static SessionMap& getSessions();
			static db::SQLiteThreadExec* getSQLiteThread();

			static void startServer();
		};

		/// @todo Move constants below in corresponding Request class

		//! Function codes TO BE TRANSFORMED AS REQUEST SUBCLASSES
		static const std::string FUNCTION_HOME ("ac");
		static const std::string FUNCTION_SCHEDULE_SHEET ("fh");
		static const std::string FUNCTION_STOP_DESCRIPTION ("fa");
		static const std::string FUNCTION_STOP_LIST ("lpa");
		static const std::string FUNCTION_SCHEDULE_SHEET_VALIDATION ("vfh");
		static const std::string FUNCTION_STATION_DEPARTURE_TABLE ("tdg");
		static const std::string FUNCTION_DEPARTURE_TABLE ("td");
		static const std::string FUNCTION_TINY_DEPARTURE_TABLE ("mtd");
		static const std::string FUNCTION_RESERVATION_FORM ("fres");
		static const std::string FUNCTION_RESERVATION_VALIDATION ("vres");
		static const std::string FUNCTION_RESERVATION_CANCELLING ("ares");

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
	}
}
/** @} */

#endif // SYNTHESE_ServerModule_H__

