
/** EnvModule class header.
	@file EnvModule.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_EnvModule_H__
#define SYNTHESE_EnvModule_H__

#include "01_util/ModuleClass.h"

#include "15_env/Environment.h"
#include "15_env/City.h"
#include "15_env/PhysicalStop.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/CommercialLine.h"
#include "15_env/Line.h"
#include "15_env/LineStop.h"
#include "15_env/Axis.h"
#include "15_env/Fare.h"
#include "15_env/BikeCompliance.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/PedestrianCompliance.h"
#include "15_env/ScheduledService.h"
#include "15_env/ContinuousService.h"

/** @defgroup m15 15 Environment.
@{
	SQLite persistent data loader (with CDM)

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
		class Path;

		class EnvModule : public util::ModuleClass
		{
		private:

			static Environment::Registry			_environments;
			static City::Registry					_cities;
			static ConnectionPlace::Registry		_connectionPlaces;
			static PhysicalStop::Registry			_physicalStops;
			static CommercialLine::Registry			_commercialLines;
			static Line::Registry					_lines;
			static Axis::Registry					_axes;
			static Fare::Registry					_fares;
			static BikeCompliance::Registry			_bikeCompliances;
			static HandicappedCompliance::Registry	_handicappedCompliances;
			static PedestrianCompliance::Registry	_pedestrianCompliances;
			static ReservationRule::Registry		_reservationRules;
			static LineStop::Registry				_lineStops;
			static ScheduledService::Registry		_scheduledServices;
			static ContinuousService::Registry		_continuousServices;


		public:
			~EnvModule();
			
			void initialize();
			
			static Environment::Registry&			getEnvironments();
			static City::Registry&					getCities();
			static ConnectionPlace::Registry&		getConnectionPlaces();
			static PhysicalStop::Registry&			getPhysicalStops();
			static CommercialLine::Registry&		getCommercialLines();
			static Line::Registry&					getLines();
			static Axis::Registry&					getAxes();
			static Fare::Registry&					getFares();
			static BikeCompliance::Registry&		getBikeCompliances();
			static HandicappedCompliance::Registry&	getHandicappedCompliances();
			static PedestrianCompliance::Registry&	getPedestrianCompliances();
			static ReservationRule::Registry&		getReservationRules();
			static LineStop::Registry&				getLineStops();
			static ScheduledService::Registry&		getScheduledServices();
			static ContinuousService::Registry&		getContinuousServices();


			static Path* fetchPath (const uid& id);
			static Service* fetchService (const uid& id);

			static std::map<uid, std::string>		getCommercialLineLabels(bool withAll=false);

		};
	}
}

/** @} */

#endif // SYNTHESE_EnvModule_H__
