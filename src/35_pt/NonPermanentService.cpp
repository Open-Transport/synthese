
/** NonPermanentService class implementation.
	@file NonPermanentService.cpp

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

#include "NonPermanentService.h"
#include "JourneyPattern.hpp"
#include "PTUseRule.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace calendar;
	using namespace graph;
	using namespace pt;

	namespace pt
	{
		NonPermanentService::NonPermanentService(
			RegistryKeyType key
		):	Registrable(key),
			Service(),
			Calendar()
		{}



		NonPermanentService::NonPermanentService(
			string serviceNumber,
			Path* path
		):	Registrable(0),
			Service(serviceNumber, path),
			Calendar()
		{}



		void NonPermanentService::setInactive(
			const date& d
		){
			// Mark the date in service calendar
			Calendar::setInactive(d);

		    /// @todo Implement it :
			// see in each date if there is at least an other service which runs.
			// If not unmark the date and see the following one.
		}


		void NonPermanentService::setActive(
			const date& d
		){
			// Mark the date in service calendar
			Calendar::setActive(d);
			updatePathCalendar();
		}



		void NonPermanentService::updatePathCalendar()
		{
			static_cast<JourneyPattern*>(getPath())->clearCalendarCache();
		}



		calendar::Calendar& NonPermanentService::operator|=( const calendar::Calendar& op )
		{
			Calendar::operator|=(op);
			updatePathCalendar();
			return *this;
		}

		const JourneyPattern* NonPermanentService::getRoute() const
		{
			return static_cast<const JourneyPattern*>(getPath());
		}

		
}	}
