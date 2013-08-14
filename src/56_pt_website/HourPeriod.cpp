
/** HourPeriod class implementation.
	@file HourPeriod.cpp

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

#include "HourPeriod.h"

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	namespace pt_website
	{
		HourPeriod::HourPeriod (
			const string& caption
			, const time_duration& startHour
			, const time_duration& endHour
		)	: _startHour(startHour)
			, _endHour(endHour)
			, _caption(caption)
		{}

/*		HourPeriod::HourPeriod()
			: _startHour(TIME_UNKNOWN)
			, _endHour(TIME_UNKNOWN)
		{}

		HourPeriod::HourPeriod( const HourPeriod& period )
			: _caption(period._caption)
			, _startHour(period._startHour)
			, _endHour(period._endHour)
		{

		}
*/
		HourPeriod::~HourPeriod ()
		{}



		const string& HourPeriod::getCaption() const
		{
			return _caption;
		}



		const time_duration& HourPeriod::getBeginHour() const
		{
			return _startHour;
		}



		const time_duration& HourPeriod::getEndHour() const
		{
			return _endHour;
		}

		bool HourPeriod::operator==( const HourPeriod& op ) const
		{
			return _startHour == op._startHour &&
				_endHour == op._endHour &&
				_caption == op._caption;
		}
	}
}
