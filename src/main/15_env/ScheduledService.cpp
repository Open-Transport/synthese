
/** ScheduledService class implementation.
	@file ScheduledService.cpp

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

#include "ScheduledService.h"

#include "15_env/Path.h"
#include "15_env/Edge.h"
#include "15_env/ReservationRule.h"

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace util
	{
		template<> typename Registrable<uid,env::ScheduledService>::Registry Registrable<uid,env::ScheduledService>::_registry;
	}

	namespace env
	{

		ScheduledService::ScheduledService (const uid& id,
							int serviceNumber,
							Path* path
							)
			: synthese::util::Registrable<uid,ScheduledService> (id)
			, NonPermanentService(serviceNumber, path)
		{

		}

		ScheduledService::ScheduledService()
			: Registrable<uid,ScheduledService>()
			, NonPermanentService()
		{
		}



		ScheduledService::~ScheduledService ()
		{
		}



		    
		bool 
		ScheduledService::isContinuous () const
		{
			return false;
		}


		void ScheduledService::setPath(Path* path )
		{
			Service::setPath(path);
		}

		uid ScheduledService::getId() const
		{
			return getKey();
		}

		ServicePointer ScheduledService::getFromPresenceTime(
			ServicePointer::DeterminationMethod method
			, const Edge* edge
			, const time::DateTime& presenceDateTime
			, const time::DateTime& computingTime
			, bool controlIfTheServiceIsReachable
		) const {

			// Initializations
			ServicePointer ptr(method, edge);
			ptr.setService(this);
			DateTime actualTime(presenceDateTime);
			Schedule schedule;
			int edgeIndex(edge->getRankInPath());

			// Actual time
			if (method == ServicePointer::DEPARTURE_TO_ARRIVAL)
			{
				schedule = _departureSchedules.at(edgeIndex);
				if (presenceDateTime.getHour() > schedule.getHour())
					return ServicePointer();
			}
			if (method == ServicePointer::ARRIVAL_TO_DEPARTURE)
			{
				schedule = _arrivalSchedules.at(edgeIndex);
				if (presenceDateTime.getHour() < schedule.getHour())
					return ServicePointer();
			}
			actualTime.setHour(schedule.getHour());
			ptr.setActualTime(actualTime);
			
			// Origin departure time
			DateTime originDateTime(actualTime);
			int duration = schedule - _departureSchedules.at(0);
			originDateTime -= duration;
			ptr.setOriginDateTime(originDateTime);

			// Date control
			if (!isProvided(originDateTime.getDate()))
				return ServicePointer();

			// Reservation control
			if (controlIfTheServiceIsReachable)
			{
				if (!ptr.isReservationRuleCompliant(computingTime))
					return ServicePointer();
			}
			else
			{
				/// @todo Implement the reservation control
			}
			

			return ptr;
		}

		time::DateTime ScheduledService::getLeaveTime(
			const ServicePointer& servicePointer
			, const Edge* edge
		) const	{
			int edgeIndex(edge->getRankInPath());
			Schedule schedule(
				(servicePointer.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL)
				? _arrivalSchedules.at(edgeIndex)
				: _departureSchedules.at(edgeIndex)
				);
			DateTime actualDateTime(servicePointer.getOriginDateTime());
			actualDateTime += (schedule - _departureSchedules.at(0));
			return actualDateTime;
		}

		void ScheduledService::setDepartureSchedules( const Schedules& schedules )
		{
			_departureSchedules = schedules;
		}

		void ScheduledService::setArrivalSchedules( const Schedules& schedules )
		{
			_arrivalSchedules = schedules;
		}

		Schedule ScheduledService::getDepartureSchedule() const
		{
			return _departureSchedules.at(0);
		}

		Schedule ScheduledService::getDepartureBeginScheduleToIndex( const Edge* edge ) const
		{
			return _departureSchedules.at(edge->getRankInPath());
		}

		Schedule ScheduledService::getDepartureEndScheduleToIndex( const Edge* edge ) const
		{
			return _departureSchedules.at(edge->getRankInPath());
		}

		Schedule ScheduledService::getArrivalBeginScheduleToIndex( const Edge* edge ) const
		{
			return _arrivalSchedules.at(edge->getRankInPath());
		}

		Schedule ScheduledService::getArrivalEndScheduleToIndex( const Edge* edge ) const
		{
			return _arrivalSchedules.at(edge->getRankInPath());
		}

		const time::Schedule& ScheduledService::getLastArrivalSchedule() const
		{
			Schedules::const_iterator it(_arrivalSchedules.end() - 1);
			return *it;
		}

		const time::Schedule& ScheduledService::getLastDepartureSchedule() const
		{
			for (Path::Edges::const_reverse_iterator it(getPath()->getEdges().rbegin()); it != getPath()->getEdges().rend(); ++it)
				if ((*it)->isDeparture())
					return _departureSchedules[(*it)->getRankInPath()];
		}

		void ScheduledService::setTeam( const std::string& team )
		{
			_team = team;
		}

		std::string ScheduledService::getTeam() const
		{
			return _team;
		}
	}
}
