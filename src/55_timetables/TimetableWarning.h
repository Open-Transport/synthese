
/** TimetableWarning class header.
	@file TimetableWarning.h

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

#ifndef SYNTHESE_timetables_TimetableWarning_h__
#define SYNTHESE_timetables_TimetableWarning_h__

#include <string>

#include "Calendar.h"

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace timetables
	{
		/** TimetableWarning class.
			Renvoi d'indicateur papier, caractérisée par:
				- un numéro
				- un libellé
				- un code de jour de circulation

			Le libellé est calculé en fonction du JC du renvoi, et de celui de la page d'indicateur (différence).
			@author Hugues Romain
			@date 2001
			@ingroup m55
		*/
		class TimetableWarning
		{
		private:
			// Variables
			std::size_t				_number;
			calendar::Calendar	_calendar;
			std::string		_text;
			const calendar::CalendarTemplate*	_calendarTemplate;

			static const std::string DATA_NUMBER;
			static const std::string DATA_TEXT;
			static const std::string DATA_FIRST_YEAR;
			static const std::string DATA_FIRST_MONTH;
			static const std::string DATA_FIRST_DAY;
			static const std::string DATA_LAST_YEAR;
			static const std::string DATA_LAST_MONTH;
			static const std::string DATA_LAST_DAY;
			static const std::string DATA_LAST_DATE;
			static const std::string DATA_FIRST_DATE;
			
			static const std::string TAG_DAY;
			static const std::string TAG_CALENDAR;
			static const std::string ATTR_DATE;

		public:
			/** Constructor.
				@param calendar
				@param number
				@author Hugues Romain
				@date 2008
			*/
			TimetableWarning(
				const calendar::Calendar& calendar,
				std::size_t number,
				const std::string& text,
				const calendar::CalendarTemplate* calendarTemplate
			);

			//! @name Getters
			//@{
				std::size_t					getNumber()			const;
				const calendar::Calendar&	getCalendar()		const;
				const std::string& getText() const;
			//@}

			/// @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Export of the warning to a parameters map
				/// @param pm the parameters map to populate
				/// @param withDates Exports each date of the calendar supporting the warning
				void toParametersMap(
					util::ParametersMap& pm,
					bool withDates
				) const;
			//@}
		};
}	}

#endif // SYNTHESE_timetables_TimetableWarning_h__
