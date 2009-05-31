
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

#include "JourneyBoardServiceCellInterfacePage.h"

#include "SentAlarm.h"

#include "ReservationContact.h"
#include "ServiceUse.h"
#include "Service.h"
#include "Line.h"
#include "RollingStock.h"
#include "CommercialLine.h"
#include "ContinuousService.h"
#include "PhysicalStop.h"
#include "PublicTransportStopZoneConnectionPlace.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace messages;
	using namespace interfaces;
	using namespace env;
	using namespace util;
	using namespace time;
	using namespace graph;

	template<> const string util::FactorableTemplate<InterfacePage,routeplanner::JourneyBoardServiceCellInterfacePage>::FACTORY_KEY("journey_board_service_cell");

	namespace routeplanner
	{
		const string JourneyBoardServiceCellInterfacePage::DATA_FIRST_DEPARTURE_TIME("first_departure_time");
		const string JourneyBoardServiceCellInterfacePage::DATA_LAST_DEPARTURE_TIME("last_departure_time");
		const string JourneyBoardServiceCellInterfacePage::DATA_FIRST_ARRIVAL_TIME("first_arrival_time");
		const string JourneyBoardServiceCellInterfacePage::DATA_LAST_ARRIVAL_TIME("last_arrival_time");
		const string JourneyBoardServiceCellInterfacePage::DATA_ROLLINGSTOCK_ID("rolling_stock_id");
		const string JourneyBoardServiceCellInterfacePage::DATA_ROLLINGSTOCK_NAME("rolling_stock_name");
		const string JourneyBoardServiceCellInterfacePage::DATA_ROLLINGSTOCK_ARTICLE("rolling_stock_article");
		const string JourneyBoardServiceCellInterfacePage::DATA_DESTINATION_NAME("destination_name");
		const string JourneyBoardServiceCellInterfacePage::DATA_HANDICAPPED_FILTER_STATUS("handicapped_filter_status");
		const string JourneyBoardServiceCellInterfacePage::DATA_HANDICAPPED_PLACES_NUMBER("handicapped_places_number");
		const string JourneyBoardServiceCellInterfacePage::DATA_BIKE_FILTER_STATUS("bike_filter_status");
		const string JourneyBoardServiceCellInterfacePage::DATA_BIKE_PLACES_NUMBER("bike_places_number");
		const string JourneyBoardServiceCellInterfacePage::DATA_LINE_TEXT("line_text");
		const string JourneyBoardServiceCellInterfacePage::DATA_LINE_IMAGE("line_image");
		const string JourneyBoardServiceCellInterfacePage::DATA_LINE_ID("line_id");
		const string JourneyBoardServiceCellInterfacePage::DATA_ALARM_MESSAGE("alarm_message");
		const string JourneyBoardServiceCellInterfacePage::DATA_ALARM_LEVEL("alarm_level");
		const string JourneyBoardServiceCellInterfacePage::DATA_ODD_ROW("is_odd_row");


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
			pv.push_back( Conversion::ToString(
					serviceUse.getUseRule().getCapacity ()
			)	);
			pv.push_back( Conversion::ToString( bikeFilterStatus) );
			pv.push_back( Conversion::ToString(
					serviceUse.getUseRule().getCapacity()
			)	); // 11
			pv.push_back( commercialLine->getShortName() ); // 12
			pv.push_back( commercialLine->getLongName() ); // 13
			pv.push_back( continuousService ? Conversion::ToString(continuousService->getMaxWaitingTime()) : string() ); // 14
			pv.push_back( commercialLine->getStyle() ); //15
			pv.push_back( commercialLine->getImage() );
			pv.push_back( lexical_cast<string>(commercialLine->getKey()) ); // 17
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
