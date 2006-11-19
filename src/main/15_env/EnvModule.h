
#include "01_util/ModuleClass.h"

#include "15_env/Environment.h"

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

	<h3>questions</h3>

	@todo Answer to the questions

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
namespace synthese
{
	namespace env
	{
		class EnvModule : public util::ModuleClass
		{
			static Environment::Registry		_environments;

		public:
			
			void initialize();
			
			static Environment::Registry& getEnvironments ();
		};
	}
}

/** @} */