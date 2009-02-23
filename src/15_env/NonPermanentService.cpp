
/** NonPermanentService class implementation.
	@file NonPermanentService.cpp

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

#include "NonPermanentService.h"
#include "Path.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace time;
	using namespace graph;

	namespace env
	{
		NonPermanentService::NonPermanentService(
			RegistryKeyType key
		):	Registrable(key),
			Service(),
			Calendar()
		{
		}



		NonPermanentService::NonPermanentService(
			string serviceNumber,
			Path* path
		):	Service(serviceNumber, path),
			Registrable(UNKNOWN_VALUE),
			Calendar()
		{
		}



		void NonPermanentService::setInactive(
			const Date& date
		){
			// Mark the date in service calendar
			Calendar::setInactive(date);
		
		    /// @todo Implement it : 
			// see in each date if there is at least an other service which runs.
			// If not unmark the date and see the following one.
		}
		
		
		void NonPermanentService::setActive(
			const Date& date
		){
			// Mark the date in service calendar
			Calendar::setActive(date);
			
			Date newDate(date);
			for(int i(getDepartureSchedule().getDaysSinceDeparture());
				i<= getLastArrivalSchedule().getDaysSinceDeparture();
				++i, newDate++
			){
				getPath()->setActive(newDate);
			}
		
			//environment.updateMinMaxDatesInUse (newDate, marked);
		}
	}
}
