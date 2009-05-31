
/** JourneyBoardInterfacePage class header.
	@file JourneyBoardInterfacePage.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_JourneyBoardInterfacePage_H__
#define SYNTHESE_JourneyBoardInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

#include <boost/logic/tribool.hpp>

namespace synthese
{
	namespace graph
	{
		class Journey;
	}

	namespace geography
	{
		class Place;
	}

	namespace server
	{
		class Request;
	}

	namespace routeplanner
	{
		/** JourneyBoardInterfacePage Interface Page Class.
			@ingroup m53Pages refPages

			Available data :
			 - 0 : index of the journey board
			 - 1 : Handicapped filter
			 - 2 : Bike filter
			 - 3 : Departure time
			 - 4 : Departure place name to display (can be the asked road if it is actually a crossing)
			 - 5 : Arrival time
			 - 6 : Arrival place name (can be the asked road if it is actually a crossing)
			 - 7 : Duration
			 - 8 : Departure date
			 - 9 : Reservation possible
			 - 10 : Reservation compulsory
			 - 11 : Reservation delay (minutes)
			 - 12 : Reservation deadline
			 - 13 : Reservation phone number(s) with opening hours
			 - 14 : Online reservation is available
			 - 15 : Departure time (internal format)
			 - 16 : Is it the last journey board ?
			 - 17 : Last departure time if continuous service (empty = single service)
			 - 18 : Last arrival time if continuous service (empty = single service)
			 - 19 : Departure place longitude
			 - 20 : Departure place latitude
			 - 21 : Arrival place longitude
			 - 22 : Arrival place latitude
		*/
		class JourneyBoardInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage,JourneyBoardInterfacePage>
		{
		public:
			static const std::string DATA_RANK;
			static const std::string DATA_HANDICAPPED_FILTER;
			static const std::string DATA_BIKE_FILTER;
			static const std::string DATA_DEPARTURE_TIME;
			static const std::string DATA_DEPARTURE_PLACE_NAME;
			static const std::string DATA_ARRIVAL_TIME;
			static const std::string DATA_ARRIVAL_PLACE_NAME;
			static const std::string DATA_DEPARTURE_DATE;
			static const std::string DATA_RESERVATION_AVAILABLE;
			static const std::string DATA_RESERVATION_COMPULSORY;
			static const std::string DATA_RESERVATION_DELAY;
			static const std::string DATA_RESERVATION_DEADLINE;
			static const std::string DATA_RESERVATION_PHONE_NUMBER;
			static const std::string DATA_ONLINE_RESERVATION;
			static const std::string DATA_DEPARTURE_TIME_INTERNAL_FORMAT;
			static const std::string DATA_IS_THE_LAST_JOURNEY_BOARD;
			static const std::string DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_TIME;
			static const std::string DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_TIME;
			static const std::string DATA_DEPARTURE_PLACE_LONGITUDE;
			static const std::string DATA_DEPARTURE_PLACE_LATITUDE;
			static const std::string DATA_ARRIVAL_PLACE_LONGITUDE;
			static const std::string DATA_ARRIVAL_PLACE_LATITUDE;


			JourneyBoardInterfacePage();

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.

				@param departurePlace Asked departure place
				@param arrivalPlace Asked arrival place
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& variables
				, int n
				, const graph::Journey* journey
				, const geography::Place* departurePlace
				, const geography::Place* arrivalPlace
				, boost::logic::tribool handicappedFilter
				, boost::logic::tribool bikeFilter
				, bool isTheLast
				, const server::Request* request = NULL
			) const;
		};
	}
}

#endif // SYNTHESE_JourneyBoardInterfacePage_H__
