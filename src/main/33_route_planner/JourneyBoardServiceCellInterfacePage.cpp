
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

namespace synthese
{
	using namespace messages;
	using namespace interfaces;
	using namespace env;

	namespace routeplanner
	{
		void JourneyBoardServiceCellInterfacePage::display( std::ostream& stream
			, const synthese::time::Hour& firstDepartureTime , const synthese::time::Hour& lastDepartureTime
			, const synthese::time::Hour& firstArrivalTime , const synthese::time::Hour& lastArrivalTime
			, int rollingStockId, const std::string& rollingStockName, const std::string& rollingStockFullDescription
			, const std::string& destinationName , boost::logic::tribool handicappedFilterStatus
			, int handicappedPlacesNumber, boost::logic::tribool bikeFilterStatus, int bikePlacesNumber
			, bool isReservationCompulsory, bool isReservationOptional, const synthese::time::DateTime maxBookingDate
			, const synthese::env::ReservationRule* reservationRule, const std::string& syntheseOnlineBookingURL
			, const SentAlarm* alarm , bool color, const synthese::env::Path* line
			, const server::Request* request /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back( firstDepartureTime.toInternalString() );
			pv.push_back( lastDepartureTime.toInternalString() );
			pv.push_back( firstArrivalTime.toInternalString() );
			pv.push_back( lastArrivalTime.toInternalString() );
			pv.push_back( synthese::util::Conversion::ToString( rollingStockId ) );
			pv.push_back( synthese::util::Conversion::ToString( handicappedFilterStatus ) );
			pv.push_back( synthese::util::Conversion::ToString( handicappedPlacesNumber ) );
			pv.push_back( synthese::util::Conversion::ToString( bikeFilterStatus ) );
			pv.push_back( synthese::util::Conversion::ToString( bikePlacesNumber ) );
			pv.push_back( synthese::util::Conversion::ToString( isReservationCompulsory ) );
			pv.push_back( synthese::util::Conversion::ToString( isReservationOptional ) );
			pv.push_back( maxBookingDate.toInternalString() );
			pv.push_back( reservationRule->getPhoneExchangeNumber() );
			pv.push_back( reservationRule->getPhoneExchangeOpeningHours() );
			pv.push_back( reservationRule->getWebSiteUrl() );
			pv.push_back( syntheseOnlineBookingURL );
			pv.push_back( alarm != NULL ? alarm->getLongMessage() : "" );
			pv.push_back( alarm != NULL ? synthese::util::Conversion::ToString( alarm->getLevel() ) : "" );
			pv.push_back( synthese::util::Conversion::ToString( color ) );

			InterfacePage::display( stream, pv, VariablesMap(), (const void*) line, request );
		}
	}
}
