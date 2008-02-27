
/** ScheduledService class header.
	@file ScheduledService.h

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

#ifndef SYNTHESE_ENV_SCHEDULEDSERVICE_H
#define SYNTHESE_ENV_SCHEDULEDSERVICE_H

#include "15_env/NonPermanentService.h"

#include "01_util/Registrable.h"

#include <string>

namespace synthese
{
	namespace env
	{

		/** Scheduled service.
			@ingroup m15
		*/
		class ScheduledService : 
			public util::Registrable<uid,ScheduledService>, 
			public NonPermanentService
		{
		public:
			typedef std::vector<time::Schedule> Schedules;

		private:
			Schedules	_departureSchedules;	//!< Departure schedules
			Schedules	_arrivalSchedules;		//!< Arrival schedules
			std::string	_team;

		public:

			ScheduledService (const uid& id,
					int serviceNumber,
					Path* path
					);
			ScheduledService();

			~ScheduledService ();

		    
			//! @name Getters
			//@{
				virtual std::string getTeam() const;
			//@}

			//! @name Setters
			//@{
				void	setPath(Path* path);
				void	setDepartureSchedules(const Schedules& schedules);
				void	setArrivalSchedules(const Schedules& schedules);
				void	setTeam(const std::string& team);
			//@}

			//! @name Query methods
			//@{
				virtual uid		getId()	const;
				virtual bool isContinuous () const;
				
				/** Generation of the next departure of a service according to a schedule and a presence date time, in the day of the presence time only, according to the compliances.
					@param method Search departure or arrival :
						- ServicePointer::DEPARTURE_TO_ARRIVAL
						- ServicePointer::ARRIVAL_TO_DEPARTURE
					@param edge Edge
					@param presenceDateTime Goal  time
					@param computingTime Time of the computing
					@param controlIfTheServiceIsReachable service selection method :
						- true : the result is a usable service : its departure time must be in the future, and the reservation rules must be followed
						- false : the result is a runnable service : if the reservation on it is compulsory, then there must bu at least one reservation for the service
					@return A full ServicePointer to the service. If the service cannot be used at the specified date/time, then the ServicePointer points to a NULL service.
					@author Hugues Romain
					@date 2007
					@warning The service index is unknown in the generated ServicePointer.					
				*/
				virtual ServicePointer getFromPresenceTime(
					ServicePointer::DeterminationMethod method
					, const Edge* edge
					, const time::DateTime& presenceDateTime
					, const time::DateTime& computingTime
					, bool controlIfTheServiceIsReachable
					, bool inverted
				) const;
				
				virtual time::DateTime getLeaveTime(
					const ServicePointer& servicePointer
					, const Edge* edge
					) const;

				
				/** Gets a departure schedule for this service.
					@param rank Rank of the stop where to get the departure schedule
					@return time::Schedule The schedule at the specified stop rank
				*/
				virtual time::Schedule getDepartureSchedule (int rank = 0) const;

				const time::Schedule& getLastDepartureSchedule() const;

				virtual const time::Schedule& getLastArrivalSchedule() const;

				virtual time::Schedule getDepartureBeginScheduleToIndex(int rankInPath) const;
				virtual time::Schedule getDepartureEndScheduleToIndex(int rankInPath) const;
				virtual time::Schedule getArrivalBeginScheduleToIndex(int rankInPath) const;
				virtual time::Schedule getArrivalEndScheduleToIndex(int rankInPath) const;

				
			//@}

		};
	}
}

#endif
