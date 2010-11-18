
/** TimetableServiceColRowInterfacePage class header.
	@file TimetableServiceColRowInterfacePage.hpp
	@author Hugues Romain
	@date 2010

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

#ifndef SYNTHESE_TimetableLineRowInterfacePage_H__
#define SYNTHESE_TimetableLineRowInterfacePage_H__

#include "TimetableResult.hpp"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace cms
	{
		class Webpage;
	}
	
	namespace timetables
	{
		class TimetableRow;

		//////////////////////////////////////////////////////////////////////////
		/// 55.11 Interface : Timetable with services by column.
		///	@ingroup m55Pages refPages
		///	@author Hugues Romain
		///	@date 2010
		///	@since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Each type of row is displayed by the same way :
		///	<ul>
		///		<li>A page for the row</li>
		///		<li>A page for each cell</li>
		///	</ul>
		///
		/// To allow the display to be done with the same page template, a parameter
		/// indicates to the web page template the type of the row :
		///	<ul>
		///		<li>line</li>
		///		<li>time</li>
		///		<li>note</li>
		///		<li>booking</li>
		///		<li>rollingstock</li>
		///	</ul>
		///
		///	<h2>JourneyPattern row</h2>
		///
		///	@copydoc TimetableServiceColInterfacePage::DisplayLinesRow 
		/// @copydoc TimetableServiceColInterfacePage::DisplayLineCell
		///
		/// <h2>Schedules row</h2>
		/// @copydoc TimetableServiceColInterfacePage::DisplaySchedulesRow
		/// @copydoc TimetableServiceColInterfacePage::DisplayScheduleCell
		///
		///	<h2>Notes row</h2>
		///
		/// @copydoc TimetableServiceColInterfacePage::DisplayNotesRow
		/// @copydoc TimetableServiceColInterfacePage::DisplayNoteCell
		///
		///	<h2>Transport mode row</h2>
		///
		/// @copydoc TimetableServiceColInterfacePage::DisplayRollingStocksRow
		/// @copydoc TimetableServiceColInterfacePage::DisplayRollingStockCell
		///
		///	<h2>Reservation rule row</h2>
		///
		/// Not yet implemented
		///
		class TimetableServiceColInterfacePage
		{
			static const std::string DATA_TYPE;
			static const std::string DATA_CELLS_CONTENT;
			static const std::string DATA_ROW_RANK;
			static const std::string DATA_ROW_RANK_IS_ODD;
			static const std::string DATA_CELL_RANK;
			static const std::string DATA_LINE_SHORT_NAME;
			static const std::string DATA_LINE_COLOR;
			static const std::string DATA_LINE_STYLE;
			static const std::string DATA_LINE_IMAGE;
			static const std::string DATA_CITY_ID;
			static const std::string DATA_CITY_NAME;
			static const std::string DATA_PLACE_ID;
			static const std::string DATA_PLACE_NAME;
			static const std::string DATA_HOURS;
			static const std::string DATA_MINUTES;
			static const std::string DATA_NOTE_NUMBER;
			static const std::string DATA_NOTE_TEXT;
			static const std::string DATA_ROLLING_STOCK_NAME;
			static const std::string DATA_ROLLING_STOCK_ALIAS;
			static const std::string DATA_IS_BEFORE_TRANSFER;
			static const std::string DATA_TRANSFER_DEPTH;
			static const std::string DATA_GLOBAL_RANK;
			static const std::string DATA_BLOCK_MAX_RANK;
			static const std::string DATA_IS_ARRIVAL;
			static const std::string DATA_IS_DEPARTURE;
			static const std::string DATA_STOP_NAME_26;
			static const std::string DATA_TRANSPORT_MODE_ID;

		public:
			static const std::string TYPE_LINE;
			static const std::string TYPE_TIME;
			static const std::string TYPE_NOTE;
			static const std::string TYPE_BOOKING;
			static const std::string TYPE_ROLLING_STOCK;

			//////////////////////////////////////////////////////////////////////////
			/// Display of the row containing the line numbers.
			///	@param stream Stream to write on
			///	@param page Page to use for the display
			///	@param cellPage Page to use for the display of each cell
			///	@param request Source request
			///	@param lines JourneyPattern of each column
			//////////////////////////////////////////////////////////////////////////
			/// Parameters sent to the page template :
			/// <ul>
			///		<li>type : line</li>
			///		<li>cells_content : lines cell (generated by DisplayLineCell)</li>
			///	</ul>
			static void DisplayLinesRow(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				boost::shared_ptr<const cms::Webpage> cellPage,
				const server::Request& request,
				const TimetableResult::RowLinesVector& lines
			);
			


			/** Display of a cell of the lines row of the timetable.
				@param stream Stream to write on
				@param page Page to use for the display
				@param request Source request
				@param object JourneyPattern to display
				@param rowRank Row number
				@param colRank Column number
				
				Parameters sent to the page template :
					- type : line
					- cell_rank : column number
					- roid : line id
					- line_name : line name
					- line_color : line color
					- line_style : line CSS class
					- line_image : line image url
			*/
			static void DisplayLineCell(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				const pt::CommercialLine& object,
				std::size_t colRank
			);

			static void DisplayEmptyLineCell(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				std::size_t colRank
			);


			/** Display of the row containing the schedules.
				@param stream Stream to write on
				@param page Page to use for the display
				@param cellPage Page to use for the display of each cell
				@param request Source request
				@param places informations about the row
				@param times Time of each column (not_a_date_time = stop not served)

				Parameters sent to the display template :
					- type : time
					- cells_content : lines cell (generated by DisplayScheduleCell)
					- row_rank : row number in the current block
					- row_rank_is_odd : row number is odd
					- city_id : city id
					- place_id : place id
					- city_name : city name
					- place_name : place name
					- stop_name_26 : place name alias (middle size)
					- global_rank : rank of the row in the whole table
					- is_before_transfer : 1 if the current block is a transfer before the main block
					- transfer_depth : 0 if the current block is the main block, depth of the transfer block else
					- is_arrival : 1|0
					- is_departure : 1|0
			*/
			static void DisplaySchedulesRow(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				boost::shared_ptr<const cms::Webpage> cellPage,
				const server::Request& request,
				const TimetableRow& place,
				const TimetableResult::RowTimesVector& times,
				std::size_t globalRank,
				bool isBeforeTransfer,
				std::size_t depth
			);



			/** Display of a cell of a times row of the timetable.
				@param stream Stream to write on
				@param page Page to use for the display
				@param request Source request
				@param object Time to display (not_a_date_time = stop not served)
				@param rowRank Row number
				@param colRank Column number
				
				Parameters sent to the display template :
					- type : time
					- cell_rank : column number
					- row_rank : row number
					- hours : hours (empty = stop is not served)
					- minutes : minutes (empty = stop is not served)
			*/
			static void DisplayScheduleCell(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				boost::posix_time::time_duration object,
				std::size_t rowRank,
				std::size_t colRank
			);



			/** Display of the row containing the note numbers.
				@param stream Stream to write on
				@param page Page to use for the display
				@param cellPage Page to use for the display of each cell
				@param request Source request
				@param notes Note of each column (NULL = no note)
				
				Parameters sent to the display template :
					- type : note
					- cells_content : lines cell (generated by DisplayNoteCell)
			*/
			static void DisplayNotesRow(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				boost::shared_ptr<const cms::Webpage> cellPage,
				const server::Request& request,
				const TimetableResult::RowNotesVector& notes,
				const TimetableResult::Columns& columns
			);

			

			/** Display of a cell of the notes row of the timetable.
				@param stream Stream to write on
				@param page Page to use for the display
				@param request Source request
				@param rowRank Row number
				@param column column object

				Parameters sent to the display template :
					- type : note
					- cell_rank : column number
					- note_number : note number (empty = no note)
					- note_text : note text
					- transport_mode : id of transport mode
			*/
			static void DisplayNoteCell(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				std::size_t rowRank,
				const TimetableColumn& column
			);



			/** Display of the row containing the rolling stock.
				@param stream Stream to write on
				@param page Page to use for the display
				@param cellPage Page to use for the display of each cell
				@param request Source request
				@param rollingStock Rolling stock used by each column (NULL = no information)

				Parameters sent to the display template :
					- type : rollingstock
					- cells_content : rolling stock cell (generated by DisplayRollingStockCell)
			*/
			static void DisplayRollingStockRow(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				boost::shared_ptr<const cms::Webpage> cellPage,
				const server::Request& request,
				const TimetableResult::RowRollingStockVector& rollingStock
			);


			
			//////////////////////////////////////////////////////////////////////////
			/// Display of a cell of the rolling stock row of the timetable.
			///	@param stream Stream to write on
			///	@param page Page to use for the display
			///	@param object Rolling stock to display (NULL = no information)
			///	@param colRank Column number
			//////////////////////////////////////////////////////////////////////////
			///	Parameters sent to the display template :
			///		- type : rollingstock
			///		- cell_rank : column number
			///		- roid : rolling stock id
			///		- rolling_stock_name : rolling stock short name
			///		- rolling_stock_alias : rolling stock long name
			static void DisplayRollingStockCell(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				const pt::RollingStock* object,
				std::size_t colRank
			);
		};
	}
}

#endif // SYNTHESE_TimetableLineRowInterfacePage_H__
