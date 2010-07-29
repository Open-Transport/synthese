
/** RoutePlannerInterfacePage class header.
	@file RoutePlannerInterfacePage.h

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

#ifndef SYNTHESE_RoutePlannerInterfacePage_H__
#define SYNTHESE_RoutePlannerInterfacePage_H__

#include "GraphTypes.h"

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace transportwebsite
	{
		class HourPeriod;
		class Site;
		class WebPage;
	}

	namespace server
	{
		class Request;
	}

	namespace geography
	{
		class Place;
		class NamedPlace;
	}

	namespace graph
	{
		class Journey;
		class AccessParameters;
	}

	namespace ptrouteplanner
	{
		class PTRoutePlannerResult;
	
		//////////////////////////////////////////////////////////////////////////
		/// 53.11 Interface Page : route planner result display.
		///	@ingroup m53Pages refPages
		/// @author Hugues Romain
		///
		///	Parameters :
		///		- 0 : date (internal format)
		///		- 2 : origin city text
		///		- 3 : handicapped filter
		///		- 4 : origin place text
		///		- 5 : bike filter
		///		- 6 : destination city text
		///		- 7 : destination place id
		///		- 8 : destination place text
		///		- 9 : period (id)
		///		- date (text format)
		///		- 11 : period (text format)
		///		- 12 : solutions number
		///		- 13 : accessibility code
		///		- 14 : site id
		///		- 15 : empty result because of same places
		///		- 16 : user full name
		///		- 17 : user phone
		///		- 18 : user ID
		///
		/// Parameters sent to the web page :
		/// <h2>Main page</h2>
		///	<ul>
		///		<li>Content : </li>
		///		<ul>
		///			<li>lines : row containing lines<li>
		///			<li>schedules : row containing schedules</li>
		///			<li>warnings : row containing warnings</li>
		///			<li>durations : row containing durations</li>
		///			<li>reservations : row containing reservations</li>
		///			<li>boards : journey boards</li>
		///		</ul>
		///		<li>Informations :</li>
		///		<ul>
		///			<li>solutions_number : number of solutions</li>
		///		</ul>
		///	</ul>
		///
		/// <h2>Schedule sheet cell</h2>
		///
		///	- is_first_row : isItFirst row
		///	- is_last_row : isItLast row
		///	- column_number : columnNumber
		///	- is_foot : isItFoot row
		///	- first_time : firstArrivalTime
		///	- last_time : lastArrivalTime
		///	- is_continuous_service
		///	- is_first_writing
		///	- is_last_writing
		///	- is_first_foot
		///
		///
		/// <h2>Schedule sheet row</h2>
		/// Parameters sent to the web page :
		///	<ul>
		///		<li>roid : id of the stop</li>
		///		<li>place_name : city and stop name</li>
		///		<li>cells : cells content</li>
		///		<li>is_odd_row : indicates if the line rank is odd or even to allow use of alternate styles.
		///			Values :</li>
		///		<ul>
		///			<li>false/0 = odd</li>
		///			<li>true/1 = even</li>
		///		</ul>
		///		<li>is_origin_row : 0|1 is origin row</li>
		///		<li>is_destination_row : 0|1 is destination row</li>
		///	</ul>
		///
		/// <h2>Line cell</h2>
		///
		///	Parameters sent to the web page :
		///	<ul>
		///		<li>column_number : Column number</li>
		///		<li>content : content</li>
		/// </ul>
		///
		/// <h2>Journey boards</h2>
		///
		/// @copydoc JourneyBoardInterfacePage
		///
		class RoutePlannerInterfacePage
		{
		public:
			static const std::string DATA_LINES;
			static const std::string DATA_SCHEDULES;
			static const std::string DATA_WARNINGS;
			static const std::string DATA_DURATIONS;
			static const std::string DATA_RESERVATIONS;
			static const std::string DATA_BOARDS;
			static const std::string DATA_SOLUTIONS_NUMBER;
			static const std::string DATA_MAPS_LINES;
			static const std::string DATA_MAPS;

			//! @name Cells
			//@{
				static const std::string DATA_COLUMN_NUMBER;
				static const std::string DATA_CONTENT;
			//@}

			static const std::string DATA_IS_FIRST_ROW;
			static const std::string DATA_IS_LAST_ROW;
			static const std::string DATA_IS_FOOT;
			static const std::string DATA_FIRST_TIME;
			static const std::string DATA_LAST_TIME;
			static const std::string DATA_IS_CONTINUOUS_SERVICE;
			static const std::string DATA_IS_FIRST_WRITING;
			static const std::string DATA_IS_LAST_WRITING;
			static const std::string DATA_IS_FIRST_FOOT;

			//! @name Rows
			//@{
				static const std::string DATA_CELLS;
				static const std::string DATA_IS_ODD_ROW;
				static const std::string DATA_IS_ORIGIN_ROW;
				static const std::string DATA_IS_DESTINATION_ROW;
				static const std::string DATA_PLACE_NAME;
			//@}

			static const std::string DATA_INTERNAL_DATE;
			static const std::string DATA_IS_HOME;
			static const std::string DATA_ORIGIN_CITY_TEXT;
			static const std::string DATA_HANDICAPPED_FILTER;
			static const std::string DATA_ORIGIN_PLACE_TEXT;
			static const std::string DATA_BIKE_FILTER;
			static const std::string DATA_DESTINATION_CITY_TEXT;
			static const std::string DATA_DESTINATION_PLACE_ID;
			static const std::string DATA_DESTINATION_PLACE_TEXT;
			static const std::string DATA_PERIOD_ID;
			static const std::string DATA_DATE;
			static const std::string DATA_PERIOD;
			static const std::string DATA_ACCESSIBILITY_CODE;
			static const std::string DATA_SITE_ID;
			static const std::string DATA_IS_SAME_PLACES;
			static const std::string DATA_USER_FULL_NAME;
			static const std::string DATA_USER_PHONE;
			static const std::string DATA_USER_ID;

		private:
			

			/** Display of schedule sheet cell.
				@param stream Stream to write on
				@param page web page to use
				@param request current request
				@param isItFirstRow (0) Is the cell the first departure or arrival ?
				@param isItLastRow (1) Is the cell the last departure or arrival ?
				@param columnNumber (2) Rank of the column from left to right
				@param isItFootLine (3) Is the cell on a pedestrian junction ?
				@param firstTime (4) Start of continuous service, Time else
				@param lastTime (5) End of continuous service, Time else
				@param isItContinuousService (6) Is the cell on a continuous service ?
				@param isFirstWriting (7) Is it the first time that we write on the column ?
				@param isLastWriting (8) Is it the last time that we write on the column ?
			*/
			static void DisplayScheduleCell(
				std::ostream& stream,
				boost::shared_ptr<const transportwebsite::WebPage> page,
				const server::Request& request,
				bool isItFirstRow
				, bool isItLastRow
				, size_t columnNumber
				, bool isItFootLine
				, const boost::posix_time::time_duration& firstTime
				, const boost::posix_time::time_duration& lastTime
				, bool isItContinuousService
				, bool isFirstWriting
				, bool isLastWriting
				, bool isFirstFoot				
			);



			/** Display of schedule sheet lines list cell.
				@param stream Stream to write on
				@param page page to use for the display
				@param lineMarkerPage page to use for the display of each line
				@param request current request
				@param columnNumber Column rank from left to right
				@param journey The journey to describe
			*/
			static void DisplayLinesCell(
				std::ostream& stream,
				boost::shared_ptr<const transportwebsite::WebPage> page,
				boost::shared_ptr<const transportwebsite::WebPage> lineMarkerPage,
				const server::Request& request,
				std::size_t columnNumber,
				const graph::Journey& journey
			);



			//////////////////////////////////////////////////////////////////////////
			/// Display on a web page.
			/// @param stream Stream to write on
			/// @param page page to display on
			/// @param request current request
			/// @param place place corresponding to the displayed row
			/// @param cells cells content
			/// @param alternateColor (1) indicates if the line rank is odd or even. Values :
			///		- false/0 = odd
			///		- true/1 = even
			///	@param place (Place* object) : Corresponding place of the line
			///	@param site Displayed site
			//////////////////////////////////////////////////////////////////////////
			static void DisplayRow(
				std::ostream& stream,
				boost::shared_ptr<const transportwebsite::WebPage> page,
				const server::Request& request,
				const geography::NamedPlace& place,
				const std::string& cells,
				bool alternateColor,
				bool isOrigin,
				bool isDestination
			);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Display of a result on a webpage
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			/// @param page page to use for display
			/// @param request current request
			/// @param object result to display
			//////////////////////////////////////////////////////////////////////////
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const transportwebsite::WebPage> page,
				boost::shared_ptr<const transportwebsite::WebPage> schedulesRowPage,
				boost::shared_ptr<const transportwebsite::WebPage> schedulesCellPage,
				boost::shared_ptr<const transportwebsite::WebPage> linesRowPage,
				boost::shared_ptr<const transportwebsite::WebPage> lineMarkerPage,
				boost::shared_ptr<const transportwebsite::WebPage> boardPage,
				boost::shared_ptr<const transportwebsite::WebPage> warningPage,
				boost::shared_ptr<const transportwebsite::WebPage> reservationPage,
				boost::shared_ptr<const transportwebsite::WebPage> durationPage,
				boost::shared_ptr<const transportwebsite::WebPage> textDurationPage,
				boost::shared_ptr<const transportwebsite::WebPage> mapPage,
				boost::shared_ptr<const transportwebsite::WebPage> mapLinePage,
				boost::shared_ptr<const transportwebsite::WebPage> dateTimePage,
				boost::shared_ptr<const transportwebsite::WebPage> stopCellPage,
				boost::shared_ptr<const transportwebsite::WebPage> serviceCellPage,
				boost::shared_ptr<const transportwebsite::WebPage> junctionPage,
				boost::shared_ptr<const transportwebsite::WebPage> mapStopCellPage,
				boost::shared_ptr<const transportwebsite::WebPage> mapServiceCellPage,
				boost::shared_ptr<const transportwebsite::WebPage> mapJunctionPage,
				const server::Request& request,
				const ptrouteplanner::PTRoutePlannerResult& object,
				const boost::gregorian::date& date,
				size_t periodId,
				const geography::Place* originPlace,
				const geography::Place* destinationPlace,
				const transportwebsite::HourPeriod* period,
				const graph::AccessParameters& accessParameters
			);



		};
	}
}

#endif // SYNTHESE_RoutePlannerInterfacePage_H__
