
/** TimetableUpdateAction class header.
	@file TimetableUpdateAction.h
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_TimetableUpdateAction_H__
#define SYNTHESE_TimetableUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "Timetable.h"

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace timetables
	{

		//////////////////////////////////////////////////////////////////////////
		/// 55.15 Action : pre-configured timetable properties update
		///	@ingroup m55Actions refActions
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Parameters :
		///	<ul>
		///		<li>actionParamti : timetable id</li>
		///		<li>actionParamci : base calendar id</li>
		///		<li>actionParamtt : title</li>
		///		<li>actionParamfo : format</li>
		///		<li>actionParamco : container id</li>
		///	</ul>
		class TimetableUpdateAction
			: public util::FactorableTemplate<server::Action, TimetableUpdateAction>
		{
		public:
			static const std::string PARAMETER_TIMETABLE_ID;
			static const std::string PARAMETER_BASE_CALENDAR_ID;
			static const std::string PARAMETER_TITLE;
			static const std::string PARAMETER_FORMAT;
			static const std::string PARAMETER_CONTAINER_ID;
			static const std::string PARAMETER_IGNORE_EMPTY_ROWS;
			static const std::string PARAMETER_FIELD_COMPRESSION;

		private:
			boost::shared_ptr<Timetable> _timetable;
			boost::shared_ptr<const Timetable> _container;
			boost::shared_ptr<const calendar::CalendarTemplate> _calendarTemplate;
			std::string _title;
			Timetable::ContentType _format;
			boost::optional<bool> _ignoreEmptyRows;
			boost::optional<bool> _compression;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			TimetableUpdateAction();

			virtual bool isAuthorized(const server::Session* session) const;

			void setTimetable(boost::shared_ptr<Timetable> value);
		};
	}
}

#endif // SYNTHESE_TimetableUpdateAction_H__
