
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

		public:

			ScheduledService (const uid& id,
					int serviceNumber,
					Path* path
					);
			ScheduledService();

			~ScheduledService ();

		    
			//! @name Getters
			//@{
				uid		getId()	const;
			//@}

			//! @name Setters
			//@{
				void	setPath(Path* path);
				void	setDepartureSchedules(const Schedules& schedules);
				void	setArrivalSchedules(const Schedules& schedules);
			//@}

			//! @name Query methods
			//@{
				bool isContinuous () const;
				
				virtual ServicePointer getFromPresenceTime(
					ServicePointer::DeterminationMethod method
					, const Edge* edge
					, const time::DateTime& presenceDateTime
					, const time::DateTime& computingTime
					) const;
				
				virtual time::DateTime getLeaveTime(
					const ServicePointer& servicePointer
					, const Edge* edge
					) const;

				virtual time::Schedule getDepartureSchedule () const;
				virtual const time::Schedule& getLastArrivalSchedule() const;

				virtual time::Schedule getDepartureBeginScheduleToIndex(const Edge* edge) const;
				virtual time::Schedule getDepartureEndScheduleToIndex(const Edge* edge) const;
				virtual time::Schedule getArrivalBeginScheduleToIndex(const Edge* edge) const;
				virtual time::Schedule getArrivalEndScheduleToIndex(const Edge* edge) const;
			//@}

		};
	}
}

#endif
