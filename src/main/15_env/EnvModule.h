
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
#include <set>
#include <utility>

#include "01_util/ModuleClass.h"
#include "01_util/UId.h"

#include "07_lex_matcher/LexicalMatcher.h"

#include "15_env/Types.h"

#include "12_security/Types.h"

namespace synthese
{
	/**	@defgroup m15Actions 15 Actions
		@ingroup m15

		@defgroup m15Pages 15 Pages
		@ingroup m15

		@defgroup m15Library 15 Interface library
		@ingroup m15

		@defgroup m15Functions 35 Functions
		@ingroup m15

		@defgroup m15LS 35 Table synchronizers
		@ingroup m15

		@defgroup m15Admin 35 Administration pages
		@ingroup m15

		@defgroup m15Rights 35 Rights
		@ingroup m15

		@defgroup m15Logs 35 DB Logs
		@ingroup m15

		@defgroup m15 35 Transport
		@ingroup m3
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

	/** 35 Transport network module namespace.
		The env name is due to historical reasons.
	*/
	namespace env
	{
		class Path;
		class AddressablePlace;
		class IncludingPlace;
		class Place;
		class Vertex;
		class NonPermanentService;
		

		/** 15 Transport network module class.
		*/
		class EnvModule : public util::ModuleClass
		{
		private:

			static lexmatcher::LexicalMatcher<uid> _citiesMatcher; //!< @todo To be moved in RoutePlanner

		public:
		
			void initialize();
			
			

			/** Fetches a addressable place given its id.
			All the containers storong objects typed (or subtyped) as AddressablePlace
			are inspected.
			*/
			static boost::shared_ptr<const AddressablePlace> fetchAddressablePlace (const uid& id);
			static boost::shared_ptr<AddressablePlace> fetchUpdateableAddressablePlace (const uid& id);

			static boost::shared_ptr<const IncludingPlace> fetchIncludingPlace (const uid& id);

			static boost::shared_ptr<const Place> fetchPlace (const uid& id);


 		    static boost::shared_ptr<const Vertex> fetchVertex (const uid& id);

			/** Retrieves all lines associated withb a given commercial line id.
			 */
			static LineSet fetchLines (const uid& commercialLineId);

			static CityList guessCity (const std::string& fuzzyName, int nbMatches = 10);
			static lexmatcher::LexicalMatcher<uid>& getCitiesMatcher ();


			static boost::shared_ptr<Path> fetchPath (const uid& id);
			static boost::shared_ptr<NonPermanentService> fetchService (const uid& id);

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
