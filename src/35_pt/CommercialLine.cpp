
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
#include "ImportableTableSync.hpp"
#include "ParametersMap.h"

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
	using namespace impex;

	namespace util
	{
		template<> const std::string Registry<pt::CommercialLine>::KEY("CommercialLine");
	}

	namespace pt
	{
		const string CommercialLine::DATA_LINE_SHORT_NAME("line_short_name");
		const string CommercialLine::DATA_LINE_LONG_NAME("line_long_name");
		const string CommercialLine::DATA_LINE_NAME("lineName");
		const string CommercialLine::DATA_LINE_COLOR("line_color");
		const string CommercialLine::DATA_LINE_STYLE("line_style");
		const string CommercialLine::DATA_LINE_IMAGE("line_image");
		const string CommercialLine::DATA_LINE_ID("line_id");
		const string CommercialLine::DATA_LINE_CREATOR_ID("creatorId");
		const string CommercialLine::DATA_LINE_MAP_URL("line_map_url");
		const string CommercialLine::DATA_LINE_DOC_URL("line_doc_url");
		const string CommercialLine::DATA_LINE_TIMETABLE_ID("timetable_id");



		CommercialLine::CommercialLine(
			RegistryKeyType key
		):	util::Registrable(key),
			graph::PathGroup(key),
			_network(NULL),
			_reservationContact(NULL),
			_calendarTemplate(NULL),
			_timetableId(0)
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



		void CommercialLine::toParametersMap(
			ParametersMap& pm,
			std::string prefix /*= std::string() */
		) const {
			pm.insert(prefix + DATA_LINE_ID, getKey());
			pm.insert(prefix + "id", getKey()); // For StopAreasList and LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_SHORT_NAME, getShortName());
			pm.insert(prefix + "lineShortName", getShortName()); // For StopAreasList compatibility
			pm.insert(prefix + "shortName", getShortName()); // For LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_LONG_NAME, getLongName());
			pm.insert(prefix + "longName", getLongName()); // For LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_NAME, getName());
			pm.insert(prefix + "name", getName()); // For LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_STYLE, getStyle());
			pm.insert(prefix + "lineStyle", getStyle()); // For StopAreasList compatibility
			pm.insert(prefix + "style", getStyle()); // For LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_MAP_URL, getMapURL());
			pm.insert(prefix + DATA_LINE_DOC_URL, getDocURL());
			pm.insert(prefix + DATA_LINE_CREATOR_ID,
				getDataSourceLinks().size() == 1 ?
				lexical_cast<string>(getDataSourceLinks().begin()->second) :
				ImportableTableSync::SerializeDataSourceLinks(getDataSourceLinks())
			);
			if(getColor())
			{
				pm.insert(prefix + DATA_LINE_COLOR, getColor()->toString());
				pm.insert(prefix + "color", getColor()->toString()); // For LinesListFunction compatibility
			}
			pm.insert(prefix + DATA_LINE_IMAGE, getImage());
			pm.insert(prefix + "lineImage", getImage()); // For StopAreasList compatibility
			pm.insert(prefix + "image", getImage()); // For LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_TIMETABLE_ID, getTimetableId());
		}
}	}
