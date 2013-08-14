
/** HourPeriod class header.
	@file HourPeriod.h

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

#ifndef SYNTHESE_TIME_HOURPERIOD_H
#define SYNTHESE_TIME_HOURPERIOD_H

#include <string>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace pt_website
	{
		/** ptime period in a day.
			@ingroup m56

			These objects describe a period of time in a day.
		*/
		class HourPeriod
		{
		private:
			boost::posix_time::time_duration	_startHour;	//!< Period start hour
			boost::posix_time::time_duration	_endHour;	//!< Period end hour
			std::string	_caption;	//!< Period caption

		public:
			HourPeriod(
				const std::string& caption
				, const boost::posix_time::time_duration& startHour
				, const boost::posix_time::time_duration& endHour
			);
//			HourPeriod();
//			HourPeriod(const HourPeriod& period);
			~HourPeriod ();

			bool operator==(const HourPeriod& op) const;

			//! @name Getters/Setters
			//@{
				const std::string& getCaption () const;
				const boost::posix_time::time_duration& getBeginHour() const;
				const boost::posix_time::time_duration& getEndHour() const;
			//@}
		};
	}
}

#endif
