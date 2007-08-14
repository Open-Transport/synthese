
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

#include "11_interfaces/InterfacePage.h"
#include "04_time/DateTime.h"

#include <boost/logic/tribool.hpp>

namespace synthese
{
	namespace messages
	{
		class SentAlarm;
	}

	namespace env
	{
		class ReservationRule;
		class ServiceUse;
	}

	namespace routeplanner
	{
		/** Journey board cell for use of a service.
			@code journey_board_service_cell @endcode
			@ingroup m33Pages refPages

			Parameters :
			 - 0 : First start time
			 - 1 : Last start time (will be empty if not continuous service)
			@param firstArrivalTime (2) First start time
			@param lastArrivalTime (3) Last start time (will be empty if not continuous service)
			@param rollingStockId (4) ID of used rolling stock
			@param rollingStockName (5) Display name of used rolling stock
			@param rollingStockFullDescription (6) HTML description of used Rolling stock
			@param destinationName (7) Destination name of the vehicle
			@param handicappedFilterStatus (8)
			@param handicappedPlacesNumber (9)
			@param bikeFilterStatus (10)
			@param bikePlacesNumber (11)
			@param isReservationCompulsory (12)
			@param isReservationOptional (13)
			@param maxBookingDate (14)
			@param reservationRule (15/16/17) Reservation rule (15=tel number, 16=times of opening, 17=web URL)
			@param syntheseOnlineBookingURL (18) URL to run to book on the service. Empty if online booking unavailable
			@param alarm (19/20) Alert (19=message, 20=level)
			@param color (21) Odd or even color

		*/
		class JourneyBoardServiceCellInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Display.
				@param stream Stream to write on
				@param firstDepartureTime (0) First start time
				@param lastDepartureTime (1) Last start time (will be empty if not continuous service)
				@param firstArrivalTime (2) First start time
				@param lastArrivalTime (3) Last start time (will be empty if not continuous service)
				@param rollingStockId (4) ID of used rolling stock
				@param rollingStockName (5) Display name of used rolling stock
				@param rollingStockFullDescription (6) HTML description of used Rolling stock
				@param destinationName (7) Destination name of the vehicle
				@param handicappedFilterStatus (8)
				@param handicappedPlacesNumber (9)
				@param bikeFilterStatus (10)
				@param bikePlacesNumber (11)
				@param isReservationCompulsory (12)
				@param isReservationOptional (13)
				@param maxBookingDate (14)
				@param reservationRule (15/16/17) Reservation rule (15=tel number, 16=times of opening, 17=web URL)
				@param syntheseOnlineBookingURL (18) URL to run to book on the service. Empty if online booking unavailable
				@param alarm (19/20) Alert (19=message, 20=level)
				@param color (21) Odd or even color
				@param line (Path* object)
				@param site Displayed site
			*/
			void display(
				std::ostream& stream
				, const env::ServiceUse& serviceUse
				, const time::Hour& firstDepartureTime
				, const time::Hour& lastDepartureTime
				, const time::Hour& firstArrivalTime
				, const time::Hour& lastArrivalTime
				, int rollingStockId
				, const std::string& rollingStockName
				, const std::string& rollingStockFullDescription
				, const std::string& destinationName
				, boost::logic::tribool handicappedFilterStatus
				, boost::logic::tribool bikeFilterStatus
				, bool isReservationCompulsory
				, bool isReservationOptional
				, const time::DateTime maxBookingDate
				, const env::ReservationRule* reservationRule
				, const std::string& syntheseOnlineBookingURL
				, const messages::SentAlarm* alarm
				, bool color
				, const server::Request* request = NULL
			) const;

		};
	}
}

#endif // SYNTHESE_JourneyBoardServiceCellInterfacePage_H__
