
/** ContinuousService class header.
	@file ContinuousService.h

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

#ifndef SYNTHESE_ENV_CONTINUOUSSERVICE_H
#define SYNTHESE_ENV_CONTINUOUSSERVICE_H

#include "NonPermanentService.h"
#include "Types.h"



#include <string>

namespace synthese
{
	namespace env
	{

		/** Continuous service.
			@ingroup m35
		*/
		class ContinuousService
		:	public NonPermanentService
		{
		public:
			typedef std::vector<std::pair<time::Schedule, time::Schedule> > Schedules;

		private:

			int			_range;				//!< Continuous service range (minutes).
			int			_maxWaitingTime;	//!< Max waiting waiting time before next service.
			Schedules	_departureSchedules;	//!< Departure schedules at each Edge
			Schedules	_arrivalSchedules;		//!< Arrival schedules at each edge
		    

		public:

			ContinuousService(
				util::RegistryKeyType id = UNKNOWN_VALUE,
				std::string serviceNumber = std::string(),
				Path* path = NULL,
				int range = 0,
				int maxWaitingTime = 0);

			~ContinuousService ();

		    
			//! @name Getters
			//@{
				int getMaxWaitingTime () const;
				int getRange () const;
			//@}

			//! @name Setters
			//@{
				void setMaxWaitingTime (int maxWaitingTime);
				void setRange (int range);
				void setDepartureSchedules(const Schedules& schedules);
				void setArrivalSchedules(const Schedules& schedules);
			//@}

			//! @name Query methods
			//@{
				bool isContinuous () const;

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
					AccessDirection method
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
					@return time::Schedule The first schedule of the continuous range, at the specified stop rank
				*/
				virtual time::Schedule getDepartureSchedule(int rank = 0) const;

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
