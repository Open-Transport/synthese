
/** TimetableInterfacePage class header.
	@file TimetableInterfacePage.h
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

#ifndef SYNTHESE_TimetableInterfacePage_H__
#define SYNTHESE_TimetableInterfacePage_H__

#include "Timetable.h"

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
	
	namespace timetables
	{
		class TimetableGenerator;
		class TimetableResult;

		/** 55.11 Interface page : Timetable.
			@ingroup m55Pages refPages
			@author Hugues Romain
			@date 2009

			The parameters structure depends on the type of the timetable generator.

			Common parameters :
			<ul>
				<li>0 : Timetable generator :</li>
				<ul>
					<li>container : the object is a fake timetable which "includes" other timetables</li>
					<li>calendar : a calendar showing each day of validity of each included timetable</li>
					<li>line_schema : the "thermometer" schema of the line(s) displayed in the timetable</li>
					<li>services_in_cols : real timetable with services in cols, places in rows</li>
					<li>services_in_rows : real timetable with services in rows, places in cols</li>
					<li>times_in_cols : real timetable with times in cols (can only display times from a specific place)</li>
					<li>times_in_rows : real timetable with times in rows (can only display times from a specific place)</li>
				</ul>
				<li>1 : Timetable title</li>
				<li>2 : Timetable notes</li>
				<li>3 : Calendar name</li>
			</ul>

			Specific parameters :

			For container :
			<ul>
			</ul>

			For calendar :
			<ul>
			</ul>

			For line schema :
			<ul>
			</ul>

			For services in cols :
			<ul>
				<li>4 : lines row content</li>
				<li>5 : time rows content</li>
				<li>6 : rolling stock row content</li>
				<li>7 : booking row content</li>
				<li>8 : note row content</li>
				<li>9 : at list a note</li>
			</ul>

			For services in rows :
			<ul>
				<li>3 : time row content</li>
			</ul>

			For times in cols :
			<ul>
			</li>

			For rows in cols :
			<ul>
			</li>

			Object : TimetableGenerator
		*/
		class TimetableInterfacePage
		{
		public:
			static const std::string DATA_GENERATOR_TYPE;
			static const std::string DATA_TITLE;
			static const std::string DATA_NOTES;
			static const std::string DATA_CALENDAR_NAME;
			static const std::string DATA_AT_LEAST_A_NOTE;
			static const std::string DATA_CONTENT;

			static const std::string DATA_SERVICES_IN_COLS_LINES_ROW;
			static const std::string DATA_SERVICES_IN_COLS_SCHEDULES_ROWS;
			static const std::string DATA_SERVICES_IN_COLS_ROLLING_STOCK_ROW;
			static const std::string DATA_SERVICES_IN_COLS_RESERVATIONS_ROW;
			static const std::string DATA_SERVICES_IN_COLS_NOTES_ROW;

			static const std::string DATA_SERVICES_IN_ROWS_SCHEDULES_ROWS;


			//////////////////////////////////////////////////////////////////////////
			/// Timetable display.
			///	@param stream Stream to write on
			///	@param object Timetable parameters
			///	@param generator Used timetable generator
			/// @param result Timetable result
			///	@param variables Execution variables
			///	@param request Source request
			///	@author Hugues Romain
			///	@date 2009
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const transportwebsite::WebPage> page,
				boost::shared_ptr<const transportwebsite::WebPage> notePage,
				boost::shared_ptr<const transportwebsite::WebPage> noteCalendarPage,
				boost::shared_ptr<const transportwebsite::WebPage> pageForSubTimetable,
				boost::shared_ptr<const transportwebsite::WebPage> rowPage,
				boost::shared_ptr<const transportwebsite::WebPage> cellPage,
				const server::Request& request,
				const timetables::Timetable& object,
				const timetables::TimetableGenerator& generator,
				const timetables::TimetableResult& result
			);



			//////////////////////////////////////////////////////////////////////////
			/// Converts timetable type into text code.
			/// @param value timetable type
			/// @return text code (empty if the content type is not valid)
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			static std::string GetTimetableTypeCode(Timetable::ContentType value);
		};
	}
}

#endif // SYNTHESE_TimetableInterfacePage_H__
