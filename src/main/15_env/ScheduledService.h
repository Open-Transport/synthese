
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

#include "Service.h"
#include <string>

#include "01_util/Registrable.h"
#include "ReservationRuleComplyer.h"


namespace synthese
{
	namespace env
	{

		/** Scheduled service.

		@ingroup m15
		*/
		class ScheduledService : 
			public synthese::util::Registrable<uid,ScheduledService>, 
			public ReservationRuleComplyer,
			public Service
		{
		private:


		public:

			ScheduledService (const uid& id,
					int serviceNumber,
					Path* path,
					const synthese::time::Schedule& departureSchedule);
			ScheduledService();

			~ScheduledService ();

		    
			//! @name Getters/Setters
			//@{
				uid		getId()	const;
				void	setPath(Path* path);
			//@}

			//! @name Query methods
			//@{
				bool isContinuous () const;

				/** Is this service reservable ?
				@param departureMoment Desired departure moment
				@param calculationMoment Calculation moment taken as reference 
				for reservation delay calculation
				@return true if service can be reserved, false otherwise.
				
				A service can be reserved if :
				- the path does not have any reservation rule
				- the reservation rule accepts condition
				*/
				bool isReservationPossible ( const synthese::time::DateTime& departureMoment, 
							const synthese::time::DateTime& calculationMoment ) const;
			//@}

		};
	}
}

#endif
