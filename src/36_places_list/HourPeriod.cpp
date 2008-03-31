
/** HourPeriod class implementation.
	@file HourPeriod.cpp

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

#include "HourPeriod.h"

using namespace std;

namespace synthese
{
	using namespace time;

	namespace transportwebsite
	{
		HourPeriod::HourPeriod (
			const string& caption
			, const Hour& startHour
			, const Hour& endHour
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



		const time::Hour& HourPeriod::getBeginHour() const
		{
			return _startHour;
		}



		const time::Hour& HourPeriod::getEndHour() const
		{
			return _endHour;
		}
	}
}
