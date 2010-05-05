
/** TimetableServiceRowInterfacePage class header.
	@file TimetableServiceRowInterfacePage.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_TimetableServiceRowInterfacePage_H__
#define SYNTHESE_TimetableServiceRowInterfacePage_H__

#include "TimetableColumn.h"

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace transportwebsite
	{
		class WebPage;
	}

	namespace server
	{
		class Request;
	}

	namespace pt
	{
		class RollingStock;
	}
	
	namespace timetables
	{
		//////////////////////////////////////////////////////////////////////////
		/// 55.11 Interface : Display of a time table with services by row.
		///	@ingroup m55Pages refPages
		///	@author Hugues Romain
		///	@date 2009 2010
		//////////////////////////////////////////////////////////////////////////
		/// <h2>Row display</h2>
		/// @copydoc TimetableServiceRowInterfacePage::Display
		///
		/// <h2>Cell display</h2>
		/// @copydoc TimetableServiceRowInterfacePage::DisplayCell
		class TimetableServiceRowInterfacePage
		{
			static const std::string DATA_DEPARTURE_HOUR;
			static const std::string DATA_DEPARTURE_MINUTES;
			static const std::string DATA_LAST_DEPARTURE_HOUR;
			static const std::string DATA_NOTE_NUMBER;
			static const std::string DATA_NOTE_TEXT;
			static const std::string DATA_CELLS_CONTENT;
			static const std::string DATA_DEPARTURE_QUAY;
			static const std::string DATA_LINE_SHORT_NAME;
			static const std::string DATA_LINE_STYLE;
			static const std::string DATA_LINE_PICTURE;
			static const std::string DATA_ROLLING_STOCK_ID;
			static const std::string DATA_RANK;
			static const std::string DATA_FOLLOWING_SERVICES_WITH_SAME_DEPARTURE_HOUR;
			static const std::string DATA_LINE_DESTINATION_CITY_ID;
			static const std::string DATA_LINE_DESTINATION_CITY_NAME;
			static const std::string DATA_LINE_DESTINATION_PLACE_ID;
			static const std::string DATA_LINE_DESTINATION_PLACE_NAME;
			static const std::string DATA_TEXT_LINE_DESTINATION;
			static const std::string DATA_LINE_ID;
			static const std::string DATA_ROUTE_ID;

			static const std::string DATA_HOURS;
			static const std::string DATA_MINUTES;
			static const std::string DATA_CITY_NAME;
			static const std::string DATA_STOP_NAME;
			static const std::string DATA_STOP_ALIAS;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Display of the row.
			///	@param stream Stream to write on
			/// @param page the page to use for the display
			/// @param cellPage the page to use for the display of each cell of the row
			///	@param request Source request
			///	@param object Column to display as a row
			///	@param lastSchedule Schedule of the previous row
			///	@param rank Rank of the row in the table
			///	@param followingServicesWithSameHour 1 if the next service leaves the stop at the same hour (minutes ca n be different)
			//////////////////////////////////////////////////////////////////////////
			/// Parameters sent to the webpage :
			/// <ul>
			///		<li>0 : Departure hour</li>
			///		<li>1 : Departure minutes</li>
			///		<li>2 : Last departure hour</li>
			///		<li>3 : Note number</li>
			///		<li>4 : Note text</li>
			///		<li>5 : Cells content</li>
			///		<li>6 : Departure quay</li>
			///		<li>7 : Line short name</li>
			///		<li>8 : Line style</li>
			///		<li>9 : Line picture</li>
			///		<li>10 : Rolling stock id</li>
			///		<li>11 : Rank</li>
			///		<li>12 : Following services with same departure hour</li>
			///		<li>13 : Line destination city id</li>
			///		<li>14 : Line destination city name</li>
			///		<li>15 : Line destination place id</li>
			///		<li>16 : Line destination place name</li>
			///		<li>17 : Text line destination</li>
			///		<li>18 : Line id</li>
			///		<li>19 : Route id</li>
			///	</ul>
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const transportwebsite::WebPage> page,
				boost::shared_ptr<const transportwebsite::WebPage> cellPage,
				const server::Request& request,
				const timetables::TimetableColumn& object,
				const boost::posix_time::time_duration& lastSchedule,
				std::size_t rank,
				std::size_t followingServicesWithSameHour
			);



			//////////////////////////////////////////////////////////////////////////
			/// Display of a cell.
			///	@param stream Stream to write on
			/// @param page page to use for the display
			///	@param request Source request
			///	@param object Cell object
			/// @param rollingStock the rolling stock of the row
			/// @param rank cell rank
			//////////////////////////////////////////////////////////////////////////
			/// If the stop corresponding to the cell is served by the service 
			/// corresponding to the row, the following parameters are sent to the page :
			///	<ul>
			///		<li>hour : Hour of the stop</li>
			///		<li>minutes : Minutes of the stop</li>
			///		<li>stop_name : Stop name</li>
			///		<li>city_name : City name</li>
			///		<li>stop_alias : Stop alias</li>
			///		<li>rank : Rank of the cell in the row</li>
			///		<li>rolling_stock_id : Rolling stock ID</li>
			///	</ul>
			static void DisplayCell(
				std::ostream& stream,
				boost::shared_ptr<const transportwebsite::WebPage> page,
				const server::Request& request,
				const TimetableColumn::Content::value_type& object,
				const pt::RollingStock* rollingStock,
				std::size_t rank
			);
		};
	}
}

#endif // SYNTHESE_TimetableServiceRowInterfacePage_H__
