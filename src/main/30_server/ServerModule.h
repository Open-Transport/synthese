
#ifndef SYNTHESE_ServerModule_H__
#define SYNTHESE_ServerModule_H__

#include <boost/filesystem/path.hpp>

#include "02_db/DbModuleClass.h"


namespace synthese
{
	/**	@defgroup m30Actions 30 Actions
		@ingroup m30

		@defgroup m30Pages 30 Pages
		@ingroup m30

		@defgroup m30Values 30 Values
		@ingroup m30

		@defgroup m30Functions 30 Functions
		@ingroup m30

		@defgroup m30LS 30 Table synchronizers
		@ingroup m30

		@defgroup m30Admin 30 Administration pages
		@ingroup m30

		@defgroup m30Rights 30 Rights
		@ingroup m30

		@defgroup m30Logs 30 DB Logs
		@ingroup m30
	
		@defgroup m30 30 Server
		@{
	*/

	/** 30 Server module namespace.
	*/
	namespace server
	{
		class Session;

		/** Server module class.
		*/
		class ServerModule : public db::DbModuleClass
		{
		public:
			typedef std::map<std::string, Session*> SessionMap;

		private:

			static SessionMap				_sessionMap;

		public:

			void preInit ();

			/** Standard module initializer, launched as the ones from others modules at the server opening.
			*/
			void initialize ();

			static SessionMap& getSessions();

			/** Called whenever a parameter registered by this module is changed
			 */
			static void ParameterCallback (const std::string& name, 
						       const std::string& value);

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
	}
	/** @} */
}

#endif // SYNTHESE_ServerModule_H__
