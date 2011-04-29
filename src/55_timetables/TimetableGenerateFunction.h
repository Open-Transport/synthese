
//////////////////////////////////////////////////////////////////////////////////////////
/// TimetableGenerateFunction class header.
///	@file TimetableGenerateFunction.h
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_TimetableGenerateFunction_H__
#define SYNTHESE_TimetableGenerateFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"
#include "Timetable.h"
#include "TimetableResult.hpp"

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace pt
	{
		class JourneyPattern;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace timetables
	{
		class TimetableGenerator;
		class TimetableWarning;

		/** 55.15 Function : Display timetable generation result.
			@author Hugues Romain
			@date 2009-2010
			@ingroup m55Functions refFunctions

			A timetable can be generated in several ways.

			The content and the rendering must be specified by the request.

			For more clarity, the content and rendering parameters are separated in this
			documentation.

			<h2>Content definition</h2>

			<h3>Usage 1 : loading a pre-configured timetable</h3>

			Parameters :
			<ul>
				<li>roid : id of the pre-configured timetable</li>
			</ul>

			<h3>Usage 2 : building the full timetable of a line route</h3>

			Parameters :
			<ul>
				<li>roid : id of a JourneyPattern object</li>
				<li>cid (optional) : id of the CalendarTemplate object (default : use day parameter)</li>
				<li>day (optional) : day (default : today)</li>
			</ul>

			The generated timetable is rendered as services_by_cols type.

			<h3>Usage 3 : building a timetable from a list of stops</h3>

			Parameters :
			<ul>
				<li>city1 : first city</li>
				<li>stop1 : first stop</li>
				<li>city2 : second city</li>
				<li>stop2 : second stop</li>
				<li>...</li>
				<li>cityn : nth city</li>
				<li>stopn : nth stop</li>
				<li>cid (optional) : id of the CalendarTemplate object (default : use day parameter)</li>
				<li>day (optional) : day (default : today)</li>
			</ul>

			The generated timetable is rendered as services_by_cols type.

			<h3>Usage 4 : building the full timetable of a stop (stop area or stop point)</h3>

			Parameters :
			<ul>
				<li>roid : id of a stop area (PublicStopZoneConnectionPlace) or stop zone (StopPoint) object</li>
				<li>cid (optional) : id of the CalendarTemplate object (default : use day parameter)</li>
				<li>day (optional) : day (default : today)</li>
			</ul>

			The generated timetable is rendered as services_by_rows type.

			<h2>Rendering definition</h2>

			Parameters :
			<ul>
				<li>page : id of the template to use for the main display of the timetable (parameters sent by TimetableInterfacePage::Display)</li>
				<li>note_page : id of the template to use for the display of a note (parameters sent by TimetableNoteInterfacePage::Display)</li>
				<li>note_calendar_page : id of the template to use for the display of a date in a note (parameters sent by CalendarDateInterfacePage::Display)</li>
				<li>row_page</li>
				<li>cell_page</li>
				<li>page_for_sub_timetable : id of the template to use for the main display of each timetable if the displayed object is a container (parameters sent by TimetableInterfacePage::Display)</li>
			</ul>
		*/
		class TimetableGenerateFunction:
			public util::FactorableTemplate<server::Function,TimetableGenerateFunction>
		{
		public:
			static const std::string PARAMETER_CALENDAR_ID;
			static const std::string PARAMETER_STOP_PREFIX;
			static const std::string PARAMETER_CITY_PREFIX;
			static const std::string PARAMETER_DAY;

			static const std::string PARAMETER_PAGE_ID;
			static const std::string PARAMETER_NOTE_PAGE_ID;
			static const std::string PARAMETER_NOTE_CALENDAR_PAGE_ID;
			static const std::string PARAMETER_ROW_PAGE_ID;
			static const std::string PARAMETER_CELL_PAGE_ID;
			static const std::string PARAMETER_PAGE_FOR_SUB_TIMETABLE_ID;

			static const std::string DATA_GENERATOR_TYPE;
			static const std::string DATA_TITLE;
			static const std::string DATA_NOTES;
			static const std::string DATA_CALENDAR_NAME;
			static const std::string DATA_AT_LEAST_A_NOTE;
			static const std::string DATA_CONTENT;
			static const std::string DATA_TIMETABLE_RANK;

			static const std::string DATA_SERVICES_IN_COLS_LINES_ROW;
			static const std::string DATA_SERVICES_IN_COLS_SCHEDULES_ROWS;
			static const std::string DATA_SERVICES_IN_COLS_ROLLING_STOCK_ROW;
			static const std::string DATA_SERVICES_IN_COLS_RESERVATIONS_ROW;
			static const std::string DATA_SERVICES_IN_COLS_NOTES_ROW;

			static const std::string DATA_SERVICES_IN_ROWS_SCHEDULES_ROWS;

			static const std::string DATA_NUMBER;
			static const std::string DATA_TEXT;
			static const std::string DATA_CALENDAR;
			static const std::string DATA_FIRST_YEAR;
			static const std::string DATA_FIRST_MONTH;
			static const std::string DATA_FIRST_DAY;
			static const std::string DATA_LAST_YEAR;
			static const std::string DATA_LAST_MONTH;
			static const std::string DATA_LAST_DAY;

			static const std::string DATA_TYPE;
			static const std::string DATA_CELLS_CONTENT;
			static const std::string DATA_ROW_RANK;
			static const std::string DATA_ROW_RANK_IS_ODD;
			static const std::string DATA_CELL_RANK;
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
			static const std::string DATA_SERVICE_ID;

			static const std::string TYPE_LINE;
			static const std::string TYPE_TIME;
			static const std::string TYPE_NOTE;
			static const std::string TYPE_BOOKING;
			static const std::string TYPE_ROLLING_STOCK;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const Timetable>	_timetable;
				boost::shared_ptr<const calendar::CalendarTemplate>	_calendarTemplate;
				boost::shared_ptr<const pt::JourneyPattern> _line;
				boost::shared_ptr<const cms::Webpage> _page;
				boost::shared_ptr<const cms::Webpage> _notePage;
				boost::shared_ptr<const cms::Webpage> _noteCalendarPage;
				boost::shared_ptr<const cms::Webpage> _rowPage;
				boost::shared_ptr<const cms::Webpage> _cellPage;
				boost::shared_ptr<const cms::Webpage> _pageForSubTimetable;
				boost::shared_ptr<TimetableResult::Warnings> _warnings;
				std::size_t _timetableRank;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues
			/// @date 2009
			server::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues
			/// @date 2009
			virtual void _setFromParametersMap(
				const server::ParametersMap& map
			);


		public:
			TimetableGenerateFunction();

			//! @name Setters
			//@{
				void setTimetable(boost::shared_ptr<const Timetable> value){ _timetable = value; }
				void setLine(boost::shared_ptr<const pt::JourneyPattern> value){ _line = value; }
				void setCalendarTemplate(boost::shared_ptr<const calendar::CalendarTemplate> value){ _calendarTemplate = value; }
				void setPage(boost::shared_ptr<const cms::Webpage> value){ _page = value; }
				void setRowPage(boost::shared_ptr<const cms::Webpage> value){ _rowPage = value; }
				void setCellPage(boost::shared_ptr<const cms::Webpage> value){ _cellPage = value; }
				void setTimetableRank(std::size_t value){ _timetableRank = value; }
				void setWarnings(boost::shared_ptr<TimetableResult::Warnings> value){ _warnings = value; }
				void setNotePage(boost::shared_ptr<const cms::Webpage> value){ _notePage = value; }
				void setNoteCalendarPage(boost::shared_ptr<const cms::Webpage> value){ _noteCalendarPage = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @author Hugues
			/// @date 2009
			virtual void run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @return true if the function can be run
			/// @author Hugues
			/// @date 2009
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues
			/// @date 2009
			virtual std::string getOutputMimeType() const;


			//////////////////////////////////////////////////////////////////////////
			/// Timetable display.
			///	@param stream Stream to write on
			///	@param object Timetable parameters
			///	@param generator Used timetable generator
			/// @param result Timetable result
			///	@param request Source request
			///	@author Hugues Romain
			///	@date 2009
			//////////////////////////////////////////////////////////////////////////
			/// <h3>Parameters sent to the display template</h3>
			/// The parameters structure depends on the type of the timetable generator,
			/// specified by the generator_type parameter :
			///	<ul>
			///		<li>container : the object is a fake timetable which "includes" other timetables</li>
			///		<li>calendar : a calendar showing each day of validity of each included timetable</li>
			///		<li>line_schema : the "thermometer" schema of the line(s) displayed in the timetable</li>
			///		<li>services_in_cols : real timetable with services in cols, places in rows</li>
			///		<li>services_in_rows : real timetable with services in rows, places in cols</li>
			///		<li>times_in_cols : real timetable with times in cols (can only display times from a specific place)</li>
			///		<li>times_in_rows : real timetable with times in rows (can only display times from a specific place)</li>
			///	</ul>
			///
			/// <h4>Parameters for container generator</h4>
			///	<ul>
			///		<li>title : Timetable title</li>
			///		<li>roid : Timetable id</li>
			///		<li>timetable_rank : Rank of the timetable</li>
			///	</ul>
			///
			/// <h4>Parameters for calendar generator</h4>
			/// Not yet implemented
			///	<ul>
			///		<li>title : Timetable title</li>
			///		<li>timetable_rank : Rank of the timetable</li>
			///	</ul>
			///
			/// <h4>Parameters for line schema generator</h4>
			/// Not yet implemented
			///	<ul>
			///		<li>title : Timetable title</li>
			///		<li>timetable_rank : Rank of the timetable</li>
			///	</ul>
			///
			/// <h4>Parameters for service schedules in columns</h4>
			///	<ul>
			///		<li>title : Timetable title</li>
			///		<li>notes : Timetable notes</li>
			///		<li>calendar_name : Calendar name</li>
			///		<li>lines_row : lines row content</li>
			///		<li>schedules_rows : time rows content</li>
			///		<li>rolling_stock_row : rolling stock row content</li>
			///		<li>reservation_row : booking row content</li>
			///		<li>notes_row : note row content</li>
			///		<li>at_least_a_note : at least a note</li>
			///		<li>timetable_rank : Rank of the timetable</li>
			///	</ul>
			///
			/// <h4>Parameters for service schedules in rows</h4>
			///	<ul>
			///		<li>title : Timetable title</li>
			///		<li>notes : Timetable notes</li>
			///		<li>calendar_name : Calendar name</li>
			///		<li>schedules_rows : time rows content</li>
			///		<li>timetable_rank : Rank of the timetable</li>
			///	</ul>
			///
			/// <h4>Parameters for departure schedules in rows</h4>
			///	Not yet implemented
			///
			/// <h4>Parameters for departure schedules in rows</h4>
			/// Not yet implemented
			void _display(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				const timetables::Timetable& object,
				const timetables::TimetableGenerator& generator,
				const timetables::TimetableResult& result,
				std::size_t rank
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Timetable note display.
			///	@param stream Stream to write on
			///	@param calendarDatePage page to use to generate the display of each date of the calendar. If not defined, the calendar will be empty.
			///	@param request Source request
			///	@param object the note
			/// @author Hugues Romain
			//////////////////////////////////////////////////////////////////////////
			///	<h3>Parameters sent to the display template</h3>
			///	<ul>
			///		<li>number : Note number</li>
			///		<li>text : Text</li>
			///		<li>calendar : Calendar representing the active days of the calendar, generated by a call to calendar::CalendarDateInterfacePage for each day between the first day of the first month containing an active date and the last day of the last month containing an active date.</li>
			///		<li>first-year : Year of the first day when the service runs</li>
			///		<li>first-month : Month of the first day when the service runs</li>
			///		<li>first-day : Number of the first day when the service runs</li>
			///		<li>last-year : Year of the last day when the service runs</li>
			///		<li>last-month : Month of the last day when the service runs</li>
			///		<li>last-day : Number of the last day when the service runs</li>
			///	</li>
			void _displayNote(
				std::ostream& stream,
				const server::Request& request,
				const timetables::TimetableWarning& object
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Converts timetable type into text code.
			/// @param value timetable type
			/// @return text code (empty if the content type is not valid)
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			static std::string GetTimetableTypeCode(Timetable::ContentType value);



			//////////////////////////////////////////////////////////////////////////
			/// Display of the row containing the line numbers.
			///	@param stream Stream to write on
			///	@param request Source request
			///	@param lines JourneyPattern of each column
			//////////////////////////////////////////////////////////////////////////
			/// Parameters sent to the page template :
			/// <ul>
			///		<li>type : line</li>
			///		<li>cells_content : lines cell (generated by DisplayLineCell)</li>
			///	</ul>
			void _displayLinesRow(
				std::ostream& stream,
				const server::Request& request,
				const TimetableResult::RowLinesVector& lines
			) const;



			/** Display of a cell of the lines row of the timetable.
				@param stream Stream to write on
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
			void _displayLineCell(
				std::ostream& stream,
				const server::Request& request,
				const pt::CommercialLine& object,
				std::size_t colRank
			) const;

			void _displayEmptyLineCell(
				std::ostream& stream,
				const server::Request& request,
				std::size_t colRank
			) const;


			/** Display of the row containing the schedules.
				@param stream Stream to write on
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
			void _displaySchedulesRow(
				std::ostream& stream,
				const server::Request& request,
				const TimetableRow& place,
				const TimetableResult::RowTimesVector& times,
				const TimetableResult::RowServicesVector& services,
				std::size_t globalRank,
				bool isBeforeTransfer,
				std::size_t depth
			) const;



			/** Display of a cell of a times row of the timetable.
				@param stream Stream to write on
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
			void _displayScheduleCell(
				std::ostream& stream,
				const server::Request& request,
				boost::posix_time::time_duration object,
				std::size_t rowRank,
				std::size_t colRank,
				const pt::SchedulesBasedService* service
			) const;



			/** Display of the row containing the note numbers.
				@param stream Stream to write on
				@param request Source request
				@param notes Note of each column (NULL = no note)

				Parameters sent to the display template :
					- type : note
					- cells_content : lines cell (generated by DisplayNoteCell)
			*/
			void _displayNotesRow(
				std::ostream& stream,
				const server::Request& request,
				const TimetableResult::RowNotesVector& notes,
				const TimetableResult::Columns& columns
			) const;



			/** Display of a cell of the notes row of the timetable.
				@param stream Stream to write on
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
			void _displayNoteCell(
				std::ostream& stream,
				const server::Request& request,
				std::size_t rowRank,
				const TimetableColumn& column
			) const;



			/** Display of the row containing the rolling stock.
				@param stream Stream to write on
				@param request Source request
				@param rollingStock Rolling stock used by each column (NULL = no information)

				Parameters sent to the display template :
					- type : rollingstock
					- cells_content : rolling stock cell (generated by DisplayRollingStockCell)
			*/
			void _displayRollingStockRow(
				std::ostream& stream,
				const server::Request& request,
				const TimetableResult::RowRollingStockVector& rollingStock
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of a cell of the rolling stock row of the timetable.
			///	@param stream Stream to write on
			///	@param object Rolling stock to display (NULL = no information)
			///	@param colRank Column number
			//////////////////////////////////////////////////////////////////////////
			///	Parameters sent to the display template :
			///		- type : rollingstock
			///		- cell_rank : column number
			///		- roid : rolling stock id
			///		- rolling_stock_name : rolling stock short name
			///		- rolling_stock_alias : rolling stock long name
			void _displayRollingStockCell(
				std::ostream& stream,
				const server::Request& request,
				const pt::RollingStock* object,
				std::size_t colRank
			) const;
		};
}	}

#endif // SYNTHESE_TimetableGenerateFunction_H__
