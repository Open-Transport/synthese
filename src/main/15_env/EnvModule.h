
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

#include <vector>
#include <utility>

#include "01_util/ModuleClass.h"

#include "07_lex_matcher/LexicalMatcher.h"

#include "15_env/Address.h"
#include "15_env/PlaceAlias.h"
#include "15_env/PublicPlace.h"
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
#include "15_env/ReservationRule.h"
#include "15_env/TransportNetwork.h"
#include "15_env/RoadChunk.h"
#include "15_env/Road.h"
#include "15_env/Types.h"

#include "12_security/Types.h"

namespace synthese
{
	/**	@defgroup m15Actions 15 Actions
		@ingroup m15

		@defgroup m15Pages 15 Pages
		@ingroup m15

		@defgroup m15Values 15 Values
		@ingroup m15

		@defgroup m15Functions 15 Functions
		@ingroup m15

		@defgroup m15LS 15 Table synchronizers
		@ingroup m15

		@defgroup m15Admin 15 Administration pages
		@ingroup m15

		@defgroup m15Rights 15 Rights
		@ingroup m15

		@defgroup m15Logs 15 DB Logs
		@ingroup m15

		@defgroup m15 15 Transport
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

	/** 15 Transport network module namespace.
		The env name is due to historical reasons.
	*/
	namespace env
	{
		class Path;

		/** 15 Transport network module class.
		*/
		class EnvModule : public util::ModuleClass
		{
		private:

			static Address::Registry				_addresses;
			static City::Registry					_cities;
			static ConnectionPlace::Registry		_connectionPlaces;
			static PlaceAlias::Registry				_placeAliases;
			static PublicPlace::Registry			_publicPlaces;
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
			static TransportNetwork::Registry		_networks;
			static RoadChunk::Registry				_roadChunks;
			static Road::Registry					_roads;

			static lexmatcher::LexicalMatcher<uid> _citiesMatcher; //!< @todo To be moved in RoutePlanner

		public:
		
			void initialize();
			
			

			static Address::Registry&				getAddresses();
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
			static TransportNetwork::Registry&		getTransportNetworks();
			static PlaceAlias::Registry&			getPlaceAliases();
			static PublicPlace::Registry&			getPublicPlaces();
			static RoadChunk::Registry&				getRoadChunks();
			static Road::Registry&					getRoads();
			
			

			/** Fetches a addressable place given its id.
			All the containers storong objects typed (or subtyped) as AddressablePlace
			are inspected.
			*/
			static boost::shared_ptr<const AddressablePlace> fetchAddressablePlace (const uid& id);

			static boost::shared_ptr<const IncludingPlace> fetchIncludingPlace (const uid& id);

			static boost::shared_ptr<const Place> fetchPlace (const uid& id);


			static boost::shared_ptr<const Vertex> fetchVertex (const uid& id);
			//Vertex* fetchVertex (const uid& id);

			static CityList guessCity (const std::string& fuzzyName, int nbMatches = 10);
			synthese::lexmatcher::LexicalMatcher<uid>& getCitiesMatcher ();


			static boost::shared_ptr<Path> fetchPath (const uid& id);
			static boost::shared_ptr<Service> fetchService (const uid& id);

			static std::vector<std::pair<uid, std::string> >	getCommercialLineLabels(
				const security::RightsOfSameClassMap& rights 
				, bool totalControl 
				, security::RightLevel neededLevel 
				, bool withAll=false
				);

			static void getNetworkLinePlaceRightParameterList(security::ParameterLabelsVector& m);

		};
	}
	/** @} */
}

#endif // SYNTHESE_EnvModule_H__
