
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

#include "04_time/module.h"

#include <math.h>

using namespace std;

namespace synthese
{
	using namespace time;
	using namespace util;
	using namespace graph;

	namespace env
	{

		ServicePointer PermanentService::getFromPresenceTime(
			bool RTData,
			AccessDirection method,
			UserClassCode userClass
			, const Edge* edge
			, const DateTime& presenceDateTime
			, bool controlIfTheServiceIsReachable
			, bool inverted
		) const	{
			ServicePointer sp(RTData, method,userClass,edge);
			sp.setActualTime(presenceDateTime);
			sp.setOriginDateTime(DateTime(presenceDateTime.getDate(), Hour(TIME_MIN)));
			sp.setService(this);
			sp.setServiceRange(MINUTES_PER_DAY);
			return sp;
		}

		time::DateTime PermanentService::getLeaveTime(
			const ServicePointer& servicePointer,
			const Edge* edge
		) const	{
			double distance((servicePointer.getMethod() == DEPARTURE_TO_ARRIVAL)
				? edge->getMetricOffset() - servicePointer.getEdge()->getMetricOffset()
				: servicePointer.getEdge()->getMetricOffset() - edge->getMetricOffset()
			);

			assert(distance > 0);

			int duration(ceil(distance * 0.015));
			DateTime dt(servicePointer.getActualDateTime());
			if (servicePointer.getMethod() == DEPARTURE_TO_ARRIVAL)
				dt += duration;
			else
				dt -= duration;
			return dt;
		}

		time::Schedule PermanentService::getDepartureBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return Schedule(Hour(TIME_MIN),0);
		}

		time::Schedule PermanentService::getDepartureEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return Schedule(Hour(TIME_MAX),0);
		}

		time::Schedule PermanentService::getArrivalBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return Schedule(Hour(TIME_MIN),0);
		}

		time::Schedule PermanentService::getArrivalEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return Schedule(Hour(TIME_MAX),0);
		}

	

		bool PermanentService::isContinuous() const
		{
			return false;
		}

		Schedule PermanentService::getDepartureSchedule(bool RTData, size_t rank) const
		{
			assert(rank != 0);
			return Schedule(Hour(TIME_MIN),0);
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
