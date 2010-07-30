
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

#include <string>
#include <ostream>
#include <boost/shared_ptr.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace server
	{
		class Request;
	}

	namespace graph
	{
		class ServicePointer;
		class Journey;
		class Vertex;
	}

	namespace geography
	{
		class Place;
	}

	namespace messages
	{
		class SentAlarm;
	}

	namespace pt
	{
		class ReservationContact;
		class StopPoint;
	}

	namespace road
	{
		class Road;
	}

	namespace pt_journeyplanner
	{
		/** JourneyBoardInterfacePage Interface Page Class.
			@ingroup m53Pages refPages

			<h2>Journey board page</h2>

			Method : JourneyBoardInterfacePage::Display

			Available data :
			<ul>
			 - rank : index of the journey board
			 - is_the_last_journey_board : Is it the last journey board ?
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
			 - 17 : Last departure time if continuous service (empty = single service)
			 - 18 : Last arrival time if continuous service (empty = single service)
			 - 19 : Departure place longitude
			 - 20 : Departure place latitude
			 - 21 : Arrival place longitude
			 - 22 : Arrival place latitude
			 <li>content</li>
			</ul>

			<h2>Stop cell</h2>

			<ul>
				- 0 isItArrival : true if the stop is used as an arrival, false else
				- 1 : Alert message
				- 2 : Alert level
				- 3 isItTerminus : true if the stop is the terminus of the used line, false else
				- 4 : Stop name
				- 5 Odd or even color
				- 6 Fist time
				- 7 Last time (empty if continuous service)
				- 8 WGS84 longitude
				- 9 WGS84 latitude
				- 10 is last leg
			</ul>

			<h2>Service cell</h2>
			<ul>
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
				- 12 : JourneyPattern short name
				- 13 : JourneyPattern full description
				- 14 : Waiting duration
				- 15 : JourneyPattern style
				- 16 : JourneyPattern picto url
				- 17 : JourneyPattern id
				- 18 : (nothing)
				- 19 : Alarm message
				- 20 : Alarm level
				- 21 : Odd or even color
			</ul>

			<h2>Junction cell</h2>
			<ul>
				- 0 : vertex WGS84 longitude
				- 1 : vertex WGS84 latitude
				- 2 : Vertex is in a named place
				- 3 : Odd or even row in the journey board
				- 4 : Road name
				- 5 : Length of the junction
			</ul>
		*/
		class JourneyBoardInterfacePage
		{
		public:
			//! @name Board
			//@{
				static const std::string DATA_RANK;
				static const std::string DATA_HANDICAPPED_FILTER;
				static const std::string DATA_BIKE_FILTER;
				static const std::string DATA_DEPARTURE_TIME;
				static const std::string DATA_DEPARTURE_PLACE_NAME;
				static const std::string DATA_ARRIVAL_TIME;
				static const std::string DATA_ARRIVAL_PLACE_NAME;
				static const std::string DATA_DURATION;
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
				static const std::string DATA_CONTENT;
			//@}

			//! @name Cells
			//@{
				static const std::string DATA_ODD_ROW;
				static const std::string DATA_LONGITUDE;
				static const std::string DATA_LATITUDE;
				static const std::string DATA_ALARM_LEVEL;
				static const std::string DATA_ALARM_MESSAGE;
			//@}

			//! @name Stop cells
			//@{
				static const std::string DATA_IS_ARRIVAL;
				static const std::string DATA_IS_TERMINUS;
				static const std::string DATA_STOP_NAME;
				static const std::string DATA_FIRST_TIME;
				static const std::string DATA_LAST_TIME;
				static const std::string DATA_IS_LAST_LEG;
			//@}

			//! @name Junction cells
			//@{
				static const std::string DATA_REACHED_PLACE_IS_NAMED;
				static const std::string DATA_ROAD_NAME;
				static const std::string DATA_LENGTH;
			//@}

			//! @name Service cells
			//@{
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
				static const std::string DATA_LINE_SHORT_NAME;
				static const std::string DATA_LINE_LONG_NAME;
				static const std::string DATA_LINE_STYLE;
				static const std::string DATA_LINE_IMAGE;
				static const std::string DATA_LINE_ID;
				static const std::string DATA_CONTINUOUS_SERVICE_WAITING;
			//@}

		private:
			/** Display of stop cell.
				@param stream Stream to write on
				@param isItArrival (0) true if the stop is used as an arrival, false else
				@param alarm (1/2) Alert (1=message, 2=level)
				@param isItTerminus (3) true if the stop is the terminus of the used line, false else
				@param place Place to display
				@param color (5) Odd or even color
				@param time Time
				@param continuousServiceRange Continuous service range (if <= 0 then no continuous service displayed)
				@param site Displayed site
			*/
			static void DisplayStopCell(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				bool isItArrival,
				const messages::SentAlarm* alarm,
				bool isItTerminus,
				const pt::StopPoint& physicalStop,
				bool color,
				const boost::posix_time::ptime& time,
				boost::posix_time::time_duration continuousServiceRange,
				bool isLastLeg
			);


			/** Display of junction cell.
				@param stream Stream to display on
				@param page page to use to display
				@param request current request
				@param vertex Reached vertex
				@param alarm Alarm to display for the road use
				@param color Odd or even row in the journey board
				@param road Used road
				@param distance Length of the junction
			*/
			static void DisplayJunctionCell(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				const graph::Vertex& vertex,
				const messages::SentAlarm* alarm,
				bool color,
				const road::Road* road,
				double distance
			);



			/** Display of service cell.
				@param stream Stream to write on
				@param page page to use to display
				@param request current request
				@param serviceUse The service use to display
				@param continuousServiceRange Continuous service range
				@param handicappedFilterStatus (8)
				@param bikeFilterStatus (10)
				@param alarm (19/20) Alarm
				@param color (21) Odd or even color
			*/
			static void DisplayServiceCell(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				const graph::ServicePointer& serviceUse,
				boost::posix_time::time_duration continuousServiceRange,
				boost::logic::tribool handicappedFilterStatus,
				boost::logic::tribool bikeFilterStatus,
				const messages::SentAlarm* alarm,
				bool color
			);

		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.

				@param stream Stream to write on
				@param page page to use to display
				@param request current request
				@param departurePlace Asked departure place
				@param arrivalPlace Asked arrival place
			*/
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				boost::shared_ptr<const cms::Webpage> durationPage,
				boost::shared_ptr<const cms::Webpage> datePage,
				boost::shared_ptr<const cms::Webpage> stopCellPage,
				boost::shared_ptr<const cms::Webpage> serviceCellPage,
				boost::shared_ptr<const cms::Webpage> junctionPage,
				const server::Request& request,
				std::size_t n
				, const graph::Journey& journey
				, const geography::Place& departurePlace
				, const geography::Place& arrivalPlace
				, boost::logic::tribool handicappedFilter
				, boost::logic::tribool bikeFilter
				, bool isTheLast
			);
		};
	}
}

#endif // SYNTHESE_JourneyBoardInterfacePage_H__
