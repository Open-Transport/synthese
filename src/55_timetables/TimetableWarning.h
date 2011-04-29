
/** TimetableWarning class header.
	@file TimetableWarning.h

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

#ifndef SYNTHESE_timetables_TimetableWarning_h__
#define SYNTHESE_timetables_TimetableWarning_h__

#include <string>

#include "Calendar.h"

namespace synthese
{
	namespace timetables
	{
		/** TimetableWarning class.
			Renvoi d'indicateur papier, caractérisée par:
				- un numéro
				- un libellé
				- un code de jour de circulation

			Le libellé est claculé en fonction du JC du renvoi, et de celui de la page d'indicateur (différence).
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
				const std::string& text
			);



			//! @name Getters
			//@{
				std::size_t					getNumber()			const;
				const calendar::Calendar&	getCalendar()		const;
				const std::string& getText() const;
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_TimetableWarning_h__
