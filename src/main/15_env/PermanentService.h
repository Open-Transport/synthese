
/** PermanentService class header.
	@file PermanentService.h

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

#ifndef SYNTHESE_env_PermanentService_h__
#define SYNTHESE_env_PermanentService_h__

#include "15_env/Service.h"

namespace synthese
{
	namespace env
	{
		/** PermanentService class.
			@ingroup m15
		*/
		class PermanentService : public Service
		{
		public:
			PermanentService(Path* path);
			
			/** Returns the departure schedule for this service.

			@return If this service is continuous, the first departure schedule
			(first course) is returned. Otherwise, it is the "normal" 
			departure schedule.
			*/
			virtual time::Schedule getDepartureSchedule () const;

			virtual bool isContinuous () const;

			virtual uid		getId()	const;


			/** Is this service providen a given day ?
			@param originDate Departure date of the service from its origin (warning: do not test the customer departure date which can be one or more days later; use getOriginDateTime to compute the origin date)
			@return true if the service runs at the specified date according to its Calendar
			*/
			virtual bool isProvided(const time::Date& originDate) const;

			virtual time::Schedule getDepartureBeginScheduleToIndex(const Edge* edge) const;
			virtual time::Schedule getDepartureEndScheduleToIndex(const Edge* edge) const;
			virtual time::Schedule getArrivalBeginScheduleToIndex(const Edge* edge) const;
			virtual time::Schedule getArrivalEndScheduleToIndex(const Edge* edge) const;

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
			) const;

			virtual time::DateTime getLeaveTime(
				const ServicePointer& servicePointer
				, const Edge* edge
				) const;

		};
	}
}

#endif // SYNTHESE_env_PermanentService_h__
