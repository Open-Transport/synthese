
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

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace pt
	{
		class Line;
	}

	namespace transportwebsite
	{
		class WebPage;
	}

	namespace timetables
	{
		class Timetable;

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
				<li>roid : id of a Line object</li>
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
				<li>roid : id of a stop area (PublicStopZoneConnectionPlace) or stop zone (PhysicalStop) object</li>
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

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const Timetable>	_timetable;
				boost::shared_ptr<const calendar::CalendarTemplate>	_calendarTemplate;
				boost::shared_ptr<const pt::Line> _line;
				boost::shared_ptr<const transportwebsite::WebPage> _page;
				boost::shared_ptr<const transportwebsite::WebPage> _notePage;
				boost::shared_ptr<const transportwebsite::WebPage> _noteCalendarPage;
				boost::shared_ptr<const transportwebsite::WebPage> _rowPage;
				boost::shared_ptr<const transportwebsite::WebPage> _cellPage;
				boost::shared_ptr<const transportwebsite::WebPage> _pageForSubTimetable;
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

			void setTimetable(boost::shared_ptr<const Timetable> value) { _timetable = value; }
			void setLine(boost::shared_ptr<const pt::Line> value) { _line = value; }

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
		};
	}
}

#endif // SYNTHESE_TimetableGenerateFunction_H__
