
/** CommercialLine class implementation.
	@file CommercialLine.cpp

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

#include "CommercialLine.h"
#include "Registry.h"
#include "GraphConstants.h"
#include "AllowedUseRule.h"
#include "JourneyPattern.hpp"
#include "NonPermanentService.h"
#include "JourneyPatternCopy.hpp"
#include "CalendarTemplate.h"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;
	using namespace calendar;

	namespace util
	{
		template<> const std::string Registry<pt::CommercialLine>::KEY("CommercialLine");
	}

	namespace pt
	{
		CommercialLine::CommercialLine(RegistryKeyType key)
		:	util::Registrable(key),
			graph::PathGroup(key),
			_network(NULL),
			_reservationContact(NULL),
			_calendarTemplate(NULL)
		{
			// Default use rules
			RuleUser::Rules rules(getRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			setRules(rules);
		}



		bool CommercialLine::isOptionalReservationPlace(
			const StopArea* place
		) const	{
			return _optionalReservationPlaces.find(place) != _optionalReservationPlaces.end();
		}



		void CommercialLine::addConcurrencyRule( const NonConcurrencyRule* rule )
		{
			recursive_mutex::scoped_lock lock(_nonConcurrencyRulesMutex);

			NonConcurrencyRules::iterator it(_nonConcurrencyRules.find(rule));
			if(it != _nonConcurrencyRules.end()) return;

			_nonConcurrencyRules.insert(rule);

			BOOST_FOREACH(const Path* path, _paths)
			{
				BOOST_FOREACH(const Service* service, path->getServices())
				{
					service->clearNonConcurrencyCache();
				}
			}
		}



		void CommercialLine::removeConcurrencyRule( const NonConcurrencyRule* rule )
		{
			recursive_mutex::scoped_lock lock(_nonConcurrencyRulesMutex);

			NonConcurrencyRules::iterator it(_nonConcurrencyRules.find(rule));
			if(it == _nonConcurrencyRules.end()) return;
			
			_nonConcurrencyRules.erase(it);

			BOOST_FOREACH(const Path* path, _paths)
			{
				BOOST_FOREACH(const Service* service, path->getServices())
				{
					service->clearNonConcurrencyCache();
				}
			}
		}



		bool CommercialLine::respectsCalendarTemplate(
			date_duration duration
		) const	{
			if(!_calendarTemplate)
			{
				return true;
			}

			date now(day_clock::local_day());
			date endDate(now + duration);
			Calendar period(now, endDate);

			Calendar targetCalendar(_calendarTemplate->getResult(period));

			return (getRunDays(period) & targetCalendar) == targetCalendar;
		}



		calendar::Calendar CommercialLine::getRunDays( const calendar::Calendar& mask ) const
		{
			Calendar result;
			BOOST_FOREACH(const Path* path, _paths)
			{
				BOOST_FOREACH(const Service* service, path->getServices())
				{
					if(dynamic_cast<const NonPermanentService*>(service))
					{
						result |= (*dynamic_cast<const NonPermanentService*>(service) & mask);
					}
					else
					{
						return mask;
					}
				}

				BOOST_FOREACH(const JourneyPatternCopy* subline, static_cast<const JourneyPattern*>(path)->getSubLines())
				{
					BOOST_FOREACH(const Service* service, subline->getServices())
					{
						if(dynamic_cast<const NonPermanentService*>(service))
						{
							result |= (*dynamic_cast<const NonPermanentService*>(service) & mask);
						}
						else
						{
							return mask;
						}
					}
				}
			}
			return result;
		}
	}
}
