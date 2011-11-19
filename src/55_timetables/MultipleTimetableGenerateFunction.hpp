
//////////////////////////////////////////////////////////////////////////////////////////
/// MultipleTimetableGenerateFunction class header.
///	@file MultipleTimetableGenerateFunction.hpp
///	@author Hugues Romain
///	@date 2010
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

#ifndef SYNTHESE_MultipleTimetableGenerateFunction_H__
#define SYNTHESE_MultipleTimetableGenerateFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace timetables
	{
		class Timetable;

		//////////////////////////////////////////////////////////////////////////
		///	55.15 Function : MultipleTimetableGenerateFunction.
		///	@ingroup m55Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : MultipleTimetableGenerateFunction
		///
		/// Parameters :
		/// For each time table x (first is 0)
		///	<ul>
		///		<li>roid_x : id of the timetable to generate</li>
		///		<li>cid_x : id of the calendar to use (if a calendar is already defined in the timetable object, then the calendar is applied as a mask)</li>
		///	</ul>
		class MultipleTimetableGenerateFunction:
			public util::FactorableTemplate<server::Function,MultipleTimetableGenerateFunction>
		{
		public:
			static const std::string PARAMETER_OBJECT_ID;
			static const std::string PARAMETER_CALENDAR_ID;

			static const std::string DATA_CONTENT;

		protected:
			typedef std::vector<std::pair<boost::shared_ptr<const Timetable>, boost::shared_ptr<const calendar::CalendarTemplate> > > Timetables;
			boost::optional<bool> _ignorePastDates;

			//! \name Page parameters
			//@{
				Timetables _timetables;
				boost::shared_ptr<const cms::Webpage> _page;
				boost::shared_ptr<const cms::Webpage> _pageForSubTimetable;
				boost::shared_ptr<const cms::Webpage> _notePage;
				boost::shared_ptr<const cms::Webpage> _noteCalendarPage;
				boost::shared_ptr<const cms::Webpage> _rowPage;
				boost::shared_ptr<const cms::Webpage> _cellPage;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual void run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2010
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2010
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_MultipleTimetableGenerateFunction_H__
