
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

using namespace std;

namespace synthese
{
	using namespace messages;
	using namespace interfaces;
	using namespace env;
	using namespace util;

	namespace routeplanner
	{
		void JourneyBoardServiceCellInterfacePage::display(
			std::ostream& stream
			, const ServiceUse& serviceUse
			, const synthese::time::Hour& firstDepartureTime , const synthese::time::Hour& lastDepartureTime
			, const synthese::time::Hour& firstArrivalTime , const synthese::time::Hour& lastArrivalTime
			, int rollingStockId, const std::string& rollingStockName, const std::string& rollingStockFullDescription
			, const std::string& destinationName
			, boost::logic::tribool handicappedFilterStatus
			, boost::logic::tribool bikeFilterStatus
			, bool isReservationCompulsory, bool isReservationOptional, const synthese::time::DateTime maxBookingDate
			, const synthese::env::ReservationRule* reservationRule, const std::string& syntheseOnlineBookingURL
			, const SentAlarm* alarm , bool color
			, const server::Request* request /*= NULL */ ) const
		{
			const Line* line(static_cast<const Line*>(serviceUse.getService()->getPath()));
			ParametersVector pv;
			pv.push_back( firstDepartureTime.toString() );
			pv.push_back( lastDepartureTime.toString() );
			pv.push_back( firstArrivalTime.toString() );
			pv.push_back( lastArrivalTime.toString() );
			pv.push_back( Conversion::ToString( rollingStockId ) );
			pv.push_back( Conversion::ToString( handicappedFilterStatus ) );
			pv.push_back( Conversion::ToString( serviceUse.getService()->getHandicappedCompliance ()->getCapacity () ) );
			pv.push_back( Conversion::ToString( bikeFilterStatus) );
			pv.push_back( Conversion::ToString( serviceUse.getService()->getBikeCompliance ()->getCapacity()));
			pv.push_back( Conversion::ToString( isReservationCompulsory ) );
			pv.push_back( Conversion::ToString( isReservationOptional ) );
			pv.push_back( maxBookingDate.toInternalString() );
			pv.push_back( reservationRule == NULL ? string() : reservationRule->getPhoneExchangeNumber() );
			pv.push_back( reservationRule == NULL ? string() : reservationRule->getPhoneExchangeOpeningHours() );
			pv.push_back( reservationRule == NULL ? string() : reservationRule->getWebSiteUrl() );
			pv.push_back( syntheseOnlineBookingURL );
			pv.push_back( alarm ? alarm->getLongMessage() : string());
			pv.push_back( alarm ? Conversion::ToString( alarm->getLevel() ) : string() );
			pv.push_back( Conversion::ToString( color ) );

			VariablesMap vm;
			InterfacePage::display( stream, pv, vm, static_cast<const void*>(line), request );
		}
	}
}
