
/** JourneyBoardServiceCellInterfacePage class header.
	@file JourneyBoardServiceCellInterfacePage.h

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

#ifndef SYNTHESE_JourneyBoardServiceCellInterfacePage_H__
#define SYNTHESE_JourneyBoardServiceCellInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

#include <boost/logic/tribool.hpp>

namespace synthese
{
	namespace messages
	{
		class SentAlarm;
	}

	namespace env
	{
		class ReservationContact;
	}
	
	namespace graph
	{
		class ServiceUse;
	}

	namespace routeplanner
	{
		/** Journey board cell for use of a service.
			@code journey_board_service_cell @endcode
			@ingroup m53Pages refPages

			Parameters :
				- 0 : First departure time
				- 1 : Last departure time (will be empty if not continuous service)
				- 2 : First arrival time
				- 3 : Last arrival time (will be empty if not continuous service)
				- 4 : rollingStockId (4) ID of used rolling stock
				- 5 : rollingStockName ((5) Display name of used rolling stock
				- 6 : Rolling stock article
				- 7 : destinationName (7) Destination name of the vehicle
				- 8 : handicappedFilterStatus (8)
				- 9 : handicappedPlacesNumber (9)
				- 10 : bikeFilterStatus (10)
				- 11 : bikePlacesNumber (11)
				- 12 : Line short name
				- 13 : Line full description
				- 14 : Waiting duration
				- 15 : Line style
				- 16 : Line picto url
				- 17 : Line id
				- 18 : (nothing)
				- 19 : Alarm message
				- 20 : Alarm level
				- 21 : Odd or even color

			Object :
				- CommercialLine
		*/
		class JourneyBoardServiceCellInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage,JourneyBoardServiceCellInterfacePage>
		{
		public:
			static const std::string DATA_FIRST_DEPARTURE_TIME;
			static const std::string DATA_LAST_DEPARTURE_TIME;
			static const std::string DATA_FIRST_ARRIVAL_TIME;
			static const std::string DATA_LAST_ARRIVAL_TIME;
			static const std::string DATA_ROLLINGSTOCK_ID;
			static const std::string DATA_ROLLINGSTOCK_NAME;
			static const std::string DATA_ROLLINGSTOCK_ARTICLE;
			static const std::string DATA_DESTINATION_NAME;
			static const std::string DATA_HANDICAPPED_FILTER_STATUS;
			static const std::string DATA_HANDICAPPED_PLACES_NUMBER;
			static const std::string DATA_BIKE_FILTER_STATUS;
			static const std::string DATA_BIKE_PLACES_NUMBER;
			static const std::string DATA_LINE_TEXT;
			static const std::string DATA_LINE_IMAGE;
			static const std::string DATA_LINE_ID;
			static const std::string DATA_ALARM_MESSAGE;
			static const std::string DATA_ALARM_LEVEL;
			static const std::string DATA_ODD_ROW;

			JourneyBoardServiceCellInterfacePage();

			/** Display.
				@param stream Stream to write on
				@param serviceUse The service use to display
				@param continuousServiceRange Continuous service range
				@param handicappedFilterStatus (8)
				@param bikeFilterStatus (10)
				@param alarm (19/20) Alarm
				@param color (21) Odd or even color
				@param request Source request
			*/
			void display(
				std::ostream& stream
				, const graph::ServiceUse& serviceUse
				, boost::posix_time::time_duration continuousServiceRange
				, boost::logic::tribool handicappedFilterStatus
				, boost::logic::tribool bikeFilterStatus
				, const messages::SentAlarm* alarm
				, bool color
				, const server::Request* request = NULL
			) const;

		};
	}
}

#endif // SYNTHESE_JourneyBoardServiceCellInterfacePage_H__
