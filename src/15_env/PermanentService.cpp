
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

#include <math.h>
#include <boost/date_time/posix_time/ptime.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace env
	{

		ServicePointer PermanentService::getFromPresenceTime(
			bool RTData,
			AccessDirection method,
			UserClassCode userClass
			, const Edge* edge
			, const ptime& presenceDateTime
			, bool controlIfTheServiceIsReachable
			, bool inverted
		) const	{
			ServicePointer sp(RTData, method,userClass,edge);
			sp.setActualTime(presenceDateTime);
			sp.setTheoreticalTime(presenceDateTime);
			sp.setOriginDateTime(presenceDateTime);
			sp.setService(this);
			sp.setServiceRange(posix_time::hours(24));
			return sp;
		}

		ptime PermanentService::getLeaveTime(
			const ServicePointer& servicePointer,
			const Edge* edge
		) const	{
			double distance((servicePointer.getMethod() == DEPARTURE_TO_ARRIVAL)
				? edge->getMetricOffset() - servicePointer.getEdge()->getMetricOffset()
				: servicePointer.getEdge()->getMetricOffset() - edge->getMetricOffset()
			);

			assert(distance >= 0);

			posix_time::time_duration duration(posix_time::seconds(distance > 0 ? ceil(distance * 0.9) : 1));
			posix_time::ptime dt(servicePointer.getActualDateTime());
			if (servicePointer.getMethod() == DEPARTURE_TO_ARRIVAL)
				dt += duration;
			else
				dt -= duration;
			return dt;
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
			Path* path
		):	Registrable(id),
			Service(string(), path)
		{
			if (path != NULL)
				path->setAllDays(true);
		}
	}
}
