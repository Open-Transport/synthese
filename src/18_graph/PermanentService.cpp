
/** PermanentService class implementation.
	@file PermanentService.cpp

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

#include "PermanentService.h"
#include "Edge.h"
#include "Path.h"
#include "AccessParameters.h"

#include <math.h>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;

	namespace graph
	{

		ServicePointer PermanentService::getFromPresenceTime(
			bool RTData,
			bool getDeparture,
			size_t userClass,
			const Edge& edge
			, const ptime& presenceDateTime
			, bool controlIfTheServiceIsReachable
			, bool inverted
		) const	{
			ServicePointer sp(RTData, userClass, *this, presenceDateTime);
			if(getDeparture)
			{
				sp.setDepartureInformations(edge, presenceDateTime, presenceDateTime, *edge.getFromVertex());
			}
			else
			{
				sp.setArrivalInformations(edge, presenceDateTime, presenceDateTime, *edge.getFromVertex());
			}
			sp.setServiceRange(posix_time::hours(24));
			return sp;
		}



		void PermanentService::completeServicePointer(
			ServicePointer& servicePointer,
			const Edge& edge,
			const AccessParameters& accessParameters
		) const	{
			double distance;
			if(servicePointer.getArrivalEdge() == NULL)
			{
				distance = edge.getMetricOffset() - servicePointer.getDepartureEdge()->getMetricOffset();
			}
			else
			{
				distance = servicePointer.getArrivalEdge()->getMetricOffset() - edge.getMetricOffset();
			}
			if(distance < 0)
			{
				distance = -distance;
			}

			posix_time::time_duration duration(
				_duration ?
				*_duration :
				posix_time::seconds(distance > 0 ? ceil(60 * distance / accessParameters.getApproachSpeed()) : 1)
			);
			if(servicePointer.getArrivalEdge() == NULL)
			{
				posix_time::ptime dt(servicePointer.getDepartureDateTime());
				dt += duration;
				servicePointer.setArrivalInformations(edge, dt, dt, *edge.getFromVertex());
			}
			else
			{
				posix_time::ptime dt(servicePointer.getArrivalDateTime());
				dt -= duration;
				servicePointer.setDepartureInformations(edge, dt, dt, *edge.getFromVertex());
			}
		}

		time_duration PermanentService::getDepartureBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return time_duration(0,0,0);
		}

		time_duration PermanentService::getDepartureEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return time_duration(23,59,59);
		}

		time_duration PermanentService::getArrivalBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return time_duration(0,0,0);
		}

		time_duration PermanentService::getArrivalEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return time_duration(23,59,59);
		}

	

		bool PermanentService::isContinuous() const
		{
			return false;
		}

		time_duration PermanentService::getDepartureSchedule(bool RTData, size_t rank) const
		{
			assert(rank != 0);
			return time_duration(0,0,0);
		}

		PermanentService::PermanentService(
			RegistryKeyType id,
			Path* path,
			boost::optional<boost::posix_time::time_duration> duration
		):	Registrable(id),
			Service(string(), path),
			_duration(duration)
		{
			clearRTData();
		}



		void PermanentService::_computeNextRTUpdate()
		{
			_nextRTUpdate = second_clock::local_time() + gregorian::days(1);
		}



		boost::optional<boost::posix_time::time_duration> PermanentService::getDuration() const
		{
			return _duration;
		}
	}
}
