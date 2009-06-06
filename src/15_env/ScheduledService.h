
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

#include "NonPermanentService.h"
#include "Types.h"
#include "Registry.h"

#include <string>

namespace synthese
{
	namespace env
	{

		/** Scheduled service.
			@ingroup m35
		*/
		class ScheduledService
		:	public NonPermanentService
		{
		public:
			typedef std::vector<time::Schedule> Schedules;

			/// Chosen registry class.
			typedef util::Registry<ScheduledService>	Registry;

		private:
			Schedules	_departureSchedules;	//!< Departure schedules
			Schedules	_arrivalSchedules;		//!< Arrival schedules
			std::string	_team;

		public:

			ScheduledService(
				util::RegistryKeyType id = UNKNOWN_VALUE,
				std::string serviceNumber = std::string(),
				graph::Path* path = NULL
			);

			~ScheduledService ();

		    
			//! @name Getters
			//@{
				virtual std::string getTeam() const;
				const Schedules& getDepartureSchedules() const;
				const Schedules& getArrivalSchedules() const;
			//@}

			//! @name Setters
			//@{
				void	setPath(graph::Path* path);
				void	setDepartureSchedules(const Schedules& schedules);
				void	setArrivalSchedules(const Schedules& schedules);
				void	setTeam(const std::string& team);
			//@}

			//! @name Query methods
			//@{
				virtual bool isContinuous () const;
				
				/** Generation of the next departure of a service according to a schedule and a presence date time, in the day of the presence time only, according to the compliances.
					@param method Search departure or arrival :
						- ServicePointer::DEPARTURE_TO_ARRIVAL
						- ServicePointer::ARRIVAL_TO_DEPARTURE
					@param edge Edge
					@param presenceDateTime Goal  time
					@param controlIfTheServiceIsReachable service selection method :
						- true : the result is a usable service : its departure time must be in the future, and the reservation rules must be followed
						- false : the result is a runnable service : if the reservation on it is compulsory, then there must bu at least one reservation for the service
					@return A full ServicePointer to the service. If the service cannot be used at the specified date/time, then the ServicePointer points to a NULL service.
					@author Hugues Romain
					@date 2007
					@warning The service index is unknown in the generated ServicePointer.					
				*/
				virtual graph::ServicePointer getFromPresenceTime(
					graph::AccessDirection method,
					graph::UserClassCode userClass
					, const graph::Edge* edge
					, const time::DateTime& presenceDateTime
					, bool controlIfTheServiceIsReachable
					, bool inverted
				) const;
				
				virtual time::DateTime getLeaveTime(
					const graph::ServicePointer& servicePointer
					, const graph::Edge* edge
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
	
		bool operator==(const ScheduledService& first, const ScheduledService& second);
	}
}

#endif
