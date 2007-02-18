
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

#include "15_env/ScheduledService.h"
#include "15_env/Path.h"
#include "15_env/ReservationRule.h"

namespace synthese
{
	using namespace util;

	namespace env
	{

		ScheduledService::ScheduledService (const uid& id,
							int serviceNumber,
							Path* path,
							const synthese::time::Schedule& departureSchedule)
			: synthese::util::Registrable<uid,ScheduledService> (id)
			, ReservationRuleComplyer (path) 
			, Service (serviceNumber, path, departureSchedule)
		{

		}

		ScheduledService::ScheduledService()
			: Registrable<uid,ScheduledService>()
			, ReservationRuleComplyer()
			, Service()
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



		bool 
		ScheduledService::isReservationPossible ( const synthese::time::DateTime& departureMoment, 
							const synthese::time::DateTime& calculationMoment ) const
		{
			if (getReservationRule () == 0) return true;

			return getReservationRule ()->isRunPossible 
			(this, calculationMoment, departureMoment );
		    
		}

		void ScheduledService::setPath(Path* path )
		{
			Service::setPath(path);
			ReservationRuleComplyer::setParent(path);
		}

		uid ScheduledService::getId() const
		{
			return getKey();
		}
	}
}
