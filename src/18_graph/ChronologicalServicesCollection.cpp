
/** ChronologicalServicesCollection class implementation.
	@file ChronologicalServicesCollection.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "ChronologicalServicesCollection.hpp"

#include "Service.h"

using namespace boost::posix_time;

namespace synthese
{
	namespace graph
	{
		bool cmpService::operator ()(const Service *s1, const Service *s2) const
		{
			// Same objects
			if(s1 == s2)
			{
				return false;
			}

			// NULL after all
			if(!s1)
			{
				assert(false); // This should not happen
				return false;
			}

			// All before NULL
			if(!s2)
			{
				assert(false); // This should not happen
				return true;
			}

			// Services are not null : now comparison on schedule
			const time_duration departureSchedule1(
				s1->getDepartureSchedule(false, 0)
			);
			const time_duration departureSchedule2(
				s2->getDepartureSchedule(false, 0)
			);

			// Identical schedule objects : comparison on address
			if(departureSchedule1 == departureSchedule2)
			{
				return s1 < s2;
			}

			// Undefined departure schedule after all
			if(	departureSchedule1.is_not_a_date_time()
			){
				return false;
			}

			// All before undefined departure schedule
			if(	departureSchedule2.is_not_a_date_time()
			){
				return true;
			}

			// Comparison on valid departure schedules
			return departureSchedule1 < departureSchedule2;
		}



		/** Checks if the service could be hosted by this collection.
			@param service service to test
			@return bool true if the line theory would be respected
			@author Hugues Romain
			@date 2008
		*/
		bool ChronologicalServicesCollection::isCompatible( const graph::Service& service ) const
		{
			ServiceSet::const_iterator last_it;
			ServiceSet::const_iterator it;
			for(it = _services.begin();
				it != _services.end() && (*it)->getDepartureBeginScheduleToIndex(false, 0) < service.getDepartureEndScheduleToIndex(false, 0);
				last_it = it++);

			// Same departure time is forbidden
			if (it != _services.end() && (*it)->getDepartureBeginScheduleToIndex(false, 0) == service.getDepartureEndScheduleToIndex(false, 0))
			{
				return false;
			}

			// Check of the next service if existing
			if (it != _services.end() && !(*it)->respectsLineTheoryWith(service))
			{
				return false;
			}

			// Check of the previous service if existing
			if (it != _services.begin() && !(*last_it)->respectsLineTheoryWith(service))
			{
				return false;
			}

			return true;
		}



		ChronologicalServicesCollection::ChronologicalServicesCollection()
		{

		}
}	}
