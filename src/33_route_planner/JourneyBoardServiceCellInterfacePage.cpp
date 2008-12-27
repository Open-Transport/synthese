
/** JourneyBoardServiceCellInterfacePage class implementation.
	@file JourneyBoardServiceCellInterfacePage.cpp

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

#include "33_route_planner/JourneyBoardServiceCellInterfacePage.h"

#include "17_messages/SentAlarm.h"

#include "15_env/ReservationRule.h"
#include "15_env/ServiceUse.h"
#include "15_env/Service.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/BikeCompliance.h"
#include "15_env/Line.h"
#include "15_env/RollingStock.h"
#include "15_env/CommercialLine.h"
#include "15_env/ContinuousService.h"
#include "15_env/PhysicalStop.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace messages;
	using namespace interfaces;
	using namespace env;
	using namespace util;
	using namespace time;

	template<> const string util::FactorableTemplate<InterfacePage,routeplanner::JourneyBoardServiceCellInterfacePage>::FACTORY_KEY("journey_board_service_cell");

	namespace routeplanner
	{
		void JourneyBoardServiceCellInterfacePage::display(
			std::ostream& stream
			, const ServiceUse& serviceUse
			, int continuousServiceRange
			, logic::tribool handicappedFilterStatus
			, logic::tribool bikeFilterStatus
			, const SentAlarm* alarm
			, bool color
			, const server::Request* request /*= NULL */
		) const	{
			// Continuous service
			DateTime lastDepartureDateTime(serviceUse.getDepartureDateTime());
			DateTime lastArrivalDateTime(serviceUse.getArrivalDateTime());
			if (continuousServiceRange)
			{
				lastArrivalDateTime += continuousServiceRange;
				lastDepartureDateTime += continuousServiceRange;
			}

			// Line extraction
			const Line* line(static_cast<const Line*>(serviceUse.getService()->getPath()));
			const CommercialLine* commercialLine(line->getCommercialLine());
			const ContinuousService* continuousService(dynamic_cast<const ContinuousService*>(serviceUse.getService()));

			// Build of the parameters vector
			ParametersVector pv;
			pv.push_back( serviceUse.getDepartureDateTime().getHour().toString() ); // 0
			pv.push_back( continuousServiceRange ? lastDepartureDateTime.getHour().toString() : string() );
			pv.push_back( serviceUse.getArrivalDateTime().getHour().toString() );
			pv.push_back( continuousServiceRange ? lastArrivalDateTime.getHour().toString() : string() );
			pv.push_back( line->getRollingStock() ? Conversion::ToString(line->getRollingStock()->getKey()) : string()  ); // 4
			pv.push_back( line->getRollingStock() ? line->getRollingStock()->getName() : string() ); // 5
			pv.push_back( line->getRollingStock() ? line->getRollingStock()->getArticle() : string()  ); // 6
			pv.push_back( line->getDirection().empty() ? line->getDestination()->getConnectionPlace()->getFullName() : line->getDirection() ); // 7
			pv.push_back( Conversion::ToString( handicappedFilterStatus ) );
			pv.push_back( Conversion::ToString( serviceUse.getService()->getHandicappedCompliance ()->getCapacity () ) );
			pv.push_back( Conversion::ToString( bikeFilterStatus) );
			pv.push_back( Conversion::ToString( serviceUse.getService()->getBikeCompliance ()->getCapacity())); // 11
			pv.push_back( commercialLine->getShortName() ); // 12
			pv.push_back( commercialLine->getLongName() ); // 13
			pv.push_back( continuousService ? Conversion::ToString(continuousService->getMaxWaitingTime()) : string() ); // 14
			pv.push_back( string() );
			pv.push_back( string() );
			pv.push_back( string() );
			pv.push_back( string() );	//18
			pv.push_back( alarm ? alarm->getLongMessage() : string());
			pv.push_back( alarm ? Conversion::ToString( alarm->getLevel() ) : string() );
			pv.push_back( Conversion::ToString( color ) ); // 21

			VariablesMap vm;
			InterfacePage::_display( stream, pv, vm, static_cast<const void*>(commercialLine), request );
		}



		JourneyBoardServiceCellInterfacePage::JourneyBoardServiceCellInterfacePage()
		:	Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
