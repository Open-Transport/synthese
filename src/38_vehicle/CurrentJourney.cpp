
/** CurrentJourney class implementation.
	@file CurrentJourney.cpp

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

#include "CurrentJourney.hpp"

#include "CommercialLine.h"
#include "ParametersMap.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace vehicle
	{
		const string CurrentJourney::ATTR_STOP_REQUESTED = "stop_requested";
		const string CurrentJourney::ATTR_LINE_NUMBER = "line_number";
		const string CurrentJourney::TAG_NEXT_STOP = "next_stop";
		const string CurrentJourney::TAG_COMMERCIAL_LINE = "commercial_line";
		const string CurrentJourney::TAG_TERMINUS_DEPARTURE_TIME = "terminus_departure_time";



		CurrentJourney::CurrentJourney():
			_line(NULL),
			_stopRequested(false),
			_terminusDepartureTime(boost::posix_time::not_a_date_time)
		{

		}



		void CurrentJourney::toParametersMap( util::ParametersMap& pm ) const
		{
			// Stop requested
			pm.insert(ATTR_STOP_REQUESTED, _stopRequested);

			// Line
			if(_line)
			{
				boost::shared_ptr<ParametersMap> linePM(new ParametersMap);
				_line->toParametersMap(*linePM);
				pm.insert(TAG_COMMERCIAL_LINE, linePM);
			}

			// Line number
			pm.insert(ATTR_LINE_NUMBER, _lineNumber);

			// Next stops
			BOOST_FOREACH(const NextStops::value_type& it, _nextStops)
			{
				boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);

				it.toParametersMap(*stopPM);

				pm.insert(TAG_NEXT_STOP, stopPM);
			}
			
			// Terminus departure time
			// only valid when status indicates VehiclePosition::TERMINUS_START
			if(! _terminusDepartureTime.is_not_a_date_time())
			{
				pm.insert(TAG_TERMINUS_DEPARTURE_TIME, _terminusDepartureTime);
			}
		}
}	}
