
//////////////////////////////////////////////////////////////////////////////////////////
/// TimetableGenerateFunction class header.
///	@file TimetableGenerateFunction.h
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
#include "PlacesList.hpp"

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

		//////////////////////////////////////////////////////////////////////////
		/// 55.15 Function : Display timetable generation result.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Timetable_content_generator
		//////////////////////////////////////////////////////////////////////////
		/// @author Hugues Romain
		///	@date 2009-2010
		///	@ingroup m55Functions refFunctions
		class TimetableGenerateFunction:
			public util::FactorableTemplate<server::Function,TimetableGenerateFunction>
		{
		public:
			static const std::string PARAMETER_CALENDAR_ID;
			static const std::string PARAMETER_STOP_PREFIX;
			static const std::string PARAMETER_CITY_PREFIX;
			static const std::string PARAMETER_DAY;
			static const std::string PARAMETER_WAYBACK_FILTER;
			static const std::string PARAMETER_IGNORE_PAST_DATES;
			static const std::string PARAMETER_AUTO_INTERMEDIATE_STOPS;

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
			static const std::string DATA_AT_LEAST_A_RESERVATION_RULE;
			static const std::string DATA_CONTENT;
			static const std::string DATA_TIMETABLE_RANK;

			static const std::string DATA_SERVICES_IN_COLS_LINES_ROW;
			static const std::string DATA_SERVICES_IN_COLS_SCHEDULES_ROWS;
			static const std::string DATA_SERVICES_IN_COLS_TRANSFERS_ROWS_BEFORE_SCHEDULES;
			static const std::string DATA_SERVICES_IN_COLS_TRANSFERS_ROWS_AFTER_SCHEDULES;
			static const std::string DATA_SERVICES_IN_COLS_ROLLING_STOCK_ROW;
			static const std::string DATA_SERVICES_IN_COLS_RESERVATIONS_ROW;
			static const std::string DATA_SERVICES_IN_COLS_NOTES_ROW;
			static const std::string DATA_SERVICES_IN_COLS_SERVICES_ROW;

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
			static const std::string DATA_SERVICE_NUMBER;

			static const std::string TYPE_LINE;
			static const std::string TYPE_TIME;
			static const std::string TYPE_NOTE;
			static const std::string TYPE_BOOKING;
			static const std::string TYPE_ROLLING_STOCK;
			static const std::string TYPE_SERVICE_NUMBER;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const Timetable>	_timetable;
				boost::shared_ptr<const calendar::CalendarTemplate>	_calendarTemplate;
				boost::shared_ptr<const pt::CommercialLine> _commercialLine;
				boost::shared_ptr<const pt::JourneyPattern> _line;
				boost::shared_ptr<const cms::Webpage> _page;
				boost::shared_ptr<const cms::Webpage> _notePage;
				boost::shared_ptr<const cms::Webpage> _noteCalendarPage;
				boost::shared_ptr<const cms::Webpage> _rowPage;
				boost::shared_ptr<const cms::Webpage> _cellPage;
				boost::shared_ptr<const cms::Webpage> _pageForSubTimetable;
				boost::shared_ptr<TimetableResult::Warnings> _warnings;
				boost::optional<bool> _waybackFilter;
				boost::optional<bool> _ignorePastDates;
				std::vector<boost::shared_ptr<Timetable> > _containerContent;
				std::size_t _timetableRank;
			//@}

			typedef algorithm::PlacesList<const pt::StopArea*, const pt::JourneyPattern*> PlacesListConfiguration;

			static void AddLineDirectionToTimetable(
				Timetable& timetable,
				const pt::CommercialLine& line,
				bool wayBack
			);

			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Timetable_content_generator#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2009
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Timetable_content_generator#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2009
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			TimetableGenerateFunction();

			//! @name Setters
			//@{
				void setTimetable(boost::shared_ptr<const Timetable> value){ _timetable = value; }
				void setLine(boost::shared_ptr<const pt::JourneyPattern> value){ _line = value; }
				void setCalendarTemplate(boost::shared_ptr<const calendar::CalendarTemplate> value){ _calendarTemplate = value; }
				void setIgnorePastDates(bool value){ _ignorePastDates = value; }
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
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Timetable_content_generator#XML-Response
			//////////////////////////////////////////////////////////////////////////
			/// @param stream Stream to display the content on.
			/// @author Hugues Romain
			/// @date 2009
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



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
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Timetable_content_generator#Main-template
			//////////////////////////////////////////////////////////////////////////
			///	@param stream Stream to write on
			/// @param page template to use for the display
			///	@param object Timetable parameters
			///	@param generator Used timetable generator
			/// @param result Timetable result
			///	@param request Source request
			/// @param rank rank of the timetable in a collection
			///	@author Hugues Romain
			///	@date 2009
			void _display(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				boost::shared_ptr<const cms::Webpage> pageForSubTimetable,
				boost::shared_ptr<const cms::Webpage> notePage,
				const server::Request& request,
				const timetables::Timetable& object,
				const timetables::TimetableGenerator& generator,
				const timetables::TimetableResult& result,
				std::size_t rank
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Timetable note display.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Timetable_content_generator#Note-template
			//////////////////////////////////////////////////////////////////////////
			///	@param stream Stream to write on
			///	@param request Source request
			///	@param object the note
			/// @author Hugues Romain
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



			/** Display of the row containing the service numbers.
				@param stream Stream to write on
				@param request Source request
				@param services Service of each column

				Parameters sent to the display template :
					- type : servicenumber
					- cells_content : lines cell (generated by DisplayServiceNumbersCell)
			*/
			void _displayServiceNumbersRow(
				std::ostream& stream,
				const server::Request& request,
				const TimetableResult::RowServicesVector& services
			) const;



			/** Display of the row containing the booking rules.
				@param stream Stream to write on
				@param request Source request
				@param notes Note of each column (NULL = no note)

				Parameters sent to the display template :
				- type : note
				- cells_content : lines cell (generated by DisplayNoteCell)
			*/
			void _displayBookingRow(
				std::ostream& stream,
				const server::Request& request,
				const TimetableResult::RowServicesVector& services
			) const;




			/** Display of a cell of the booking  row of the timetable.
				@param stream Stream to write on
				@param request Source request
				@param colRank Column number
				@param column column object

				Parameters sent to the display template :
					- type : note
					- cell_rank : column number
					- note_number : note number (empty = no note)
					- note_text : note text
					- transport_mode : id of transport mode
			*/
			void _displayBookingCell(
				std::ostream& stream,
				const server::Request& request,
				std::size_t colRank,
				const TimetableResult::RowServicesVector::value_type& service
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
				std::size_t colRank,
				const TimetableColumn& column
			) const;



			/** Display of a cell of the service numbers row of the timetable.
				@param stream Stream to write on
				@param request Source request
				@param colRank Column number
				@param service SchedulesBasedService object

				Parameters sent to the display template :
					- type : note
					- cell_rank : column number
					- service_number : service number
			*/
			void _displayServiceNumbersCell(
				std::ostream& stream,
				const server::Request& request,
				std::size_t colRank,
				const pt::SchedulesBasedService* service
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
				const vehicle::RollingStock* object,
				std::size_t colRank
			) const;
		};
}	}

#endif // SYNTHESE_TimetableGenerateFunction_H__
