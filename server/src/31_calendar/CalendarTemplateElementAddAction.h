
/** CalendarTemplateElementAddAction class header.
	@file CalendarTemplateElementAddAction.h
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

#ifndef SYNTHESE_CalendarTemplateElementAddAction_H__
#define SYNTHESE_CalendarTemplateElementAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "Registry.h"
#include "CalendarTemplateElement.h"

#include <boost/optional.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;

		//////////////////////////////////////////////////////////////////////////
		/// 31.15 Action : Adds an element to an existing calendar template.
		/// @ingroup m31Actions refActions
		/// @author Hugues Romain
		/// @date 2010
		//////////////////////////////////////////////////////////////////////////
		/// Key : CalendarTemplateElementAddAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamci : id of the calendar which will belong the element</li>
		///		<li>actionParamrk : rank where insert the element in the calendar elements list</li>
		///		<li>actionParamnd : first day to iterate on</li>
		///		<li>actionParamxd : last day to iterate on</li>
		///		<li>actionParamin : step between days when iterating to the first to the last day</li>
		///		<li>actionParampo : operation to do on each iterated day : </li>
		///		<ul>
		///			<li>+ : adds the day</li>
		///			<li>- : removes the day</li>
		///			<li>* : and operator</li>
		///		</ul>
		///		<li>actionParamii : id of a calendar to read when iterating : if the day is not checked in the included calendar then the iteration is skipped (0 = no inclusion)</li>
		///	</ul>
		class CalendarTemplateElementAddAction
			: public util::FactorableTemplate<server::Action, CalendarTemplateElementAddAction>
		{
		public:
			static const std::string PARAMETER_MIN_DATE;
			static const std::string PARAMETER_MAX_DATE;
			static const std::string PARAMETER_INTERVAL;
			static const std::string PARAMETER_RANK;
			static const std::string PARAMETER_INCLUDE_ID;
			static const std::string PARAMETER_POSITIVE;
			static const std::string PARAMETER_CALENDAR_ID;

		private:
			boost::shared_ptr<CalendarTemplate>	_calendar;
			boost::gregorian::date	_minDate;
			boost::gregorian::date	_maxDate;
			boost::gregorian::date_duration	_interval;
			size_t			_rank;
			boost::shared_ptr<const CalendarTemplate>	_include;
			CalendarTemplateElementOperation	_positive;

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

			CalendarTemplateElementAddAction();

			//! @name Setters
			//@{
				void setCalendar(boost::shared_ptr<CalendarTemplate> value){ _calendar = value;}
			//@}

			//! @name Modifiers
			//@{
				void setDate(boost::gregorian::date value);
			//@}

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_CalendarTemplateElementAddAction_H__
