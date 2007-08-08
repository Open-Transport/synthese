
/** HourPeriod class header.
	@file HourPeriod.h

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

#ifndef SYNTHESE_TIME_HOURPERIOD_H
#define SYNTHESE_TIME_HOURPERIOD_H

#include "04_time/Hour.h"

#include <string>

namespace synthese
{
	namespace transportwebsite
	{
		/** Hour period in a day.
			@ingroup m04

			These objects describe a period of time in a day.
		*/
		class HourPeriod
		{
		private:
			time::Hour	_startHour;	//!< Period start hour
			time::Hour	_endHour;	//!< Period end hour
			std::string	_caption;	//!< Period caption

		public:
			HourPeriod(
				const std::string& caption
				, const time::Hour& startHour
				, const time::Hour& endHour
			);
//			HourPeriod();
//			HourPeriod(const HourPeriod& period);
			~HourPeriod ();

			//! @name Getters/Setters
			//@{
				const std::string& getCaption () const;
				const time::Hour& getBeginHour() const;
				const time::Hour& getEndHour() const;
			//@}
		};
	}
}

#endif
