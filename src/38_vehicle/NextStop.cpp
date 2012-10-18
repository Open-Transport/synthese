
/** NextStop class implementation.
	@file NextStop.cpp

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

#include "NextStop.hpp"

#include "ParametersMap.h"
#include "StopPoint.hpp"

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace vehicle
	{
		const string NextStop::TAG_STOP = "stop";
		const string NextStop::ATTR_STOP_NAME = "stop_name";
		const string NextStop::ATTR_ARRIVAL_TIME = "arrival_time";



		NextStop::NextStop():
			_stop(NULL)
		{

		}



		void NextStop::toParametersMap( util::ParametersMap& pm ) const
		{
			// Stop
			if(_stop)
			{
				shared_ptr<ParametersMap> stopPM(new ParametersMap);
				_stop->toParametersMap(*stopPM, true);
				pm.insert(TAG_STOP, stopPM);
			}

			// Stop name
			pm.insert(ATTR_STOP_NAME, _stopName);

			// Arrival time
			if(!_arrivalTime.is_not_a_date_time())
			{
				pm.insert(ATTR_ARRIVAL_TIME, to_iso_extended_string(_arrivalTime));
			}
		}
}	}
