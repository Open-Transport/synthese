
/** CommercialLine class implementation.
	@file CommercialLine.cpp

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

#include "CommercialLine.h"

#include "AccessParameters.h"
#include "Edge.h"
#include "DataSourceLinksField.hpp"
#include "Registry.h"
#include "GraphConstants.h"
#include "AllowedUseRule.h"
#include "JourneyPattern.hpp"
#include "NonPermanentService.h"
#include "JourneyPatternCopy.hpp"
#include "CalendarTemplate.h"
#include "ImportableTableSync.hpp"
#include "ParametersMap.h"
#include "TransportNetwork.h"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;
	using namespace vehicle;
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
		const string CommercialLine::DATA_LINE_TIMETABLE_ID("line_timetable_id");
		const string CommercialLine::DATA_LINE_NETWORK_ID = "network_id";
		const string CommercialLine::DATA_MAX_DISPLAY_DELAY = "max_display_delay";



		CommercialLine::CommercialLine(
			RegistryKeyType key
		):	util::Registrable(key),
			graph::PathGroup(key),
			_reservationContact(NULL),
			_calendarTemplate(NULL),
			_timetableId(0),
			_displayDurationBeforeFirstDeparture(not_a_date_time)
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
				boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
							*path->sharedServicesMutex
				);
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
				boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
							*path->sharedServicesMutex
				);
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
				boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
							*path->sharedServicesMutex
				);
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
					boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
								*subline->sharedServicesMutex
					);
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
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const {

			pm.insert(prefix + DATA_LINE_ID, getKey());
			pm.insert(prefix + "id", getKey()); // For StopAreasList and LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_SHORT_NAME, getShortName());
			pm.insert(prefix + "lineShortName", getShortName()); // For StopAreasList compatibility
			pm.insert(prefix + "shortName", getShortName()); // For LinesListFunction/StopPointsListFunction compatibility
			pm.insert(prefix + DATA_LINE_LONG_NAME, getLongName());
			pm.insert(prefix + "longName", getLongName()); // For LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_NAME, getName());
			pm.insert(prefix + "name", getName()); // For LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_STYLE, getStyle());
			pm.insert(prefix + "lineStyle", getStyle()); // For StopAreasList compatibility
			pm.insert(prefix + "style", getStyle()); // For LinesListFunction/StopPointsListFunction compatibility
			pm.insert(prefix + DATA_LINE_MAP_URL, getMapURL());
			pm.insert(prefix + DATA_LINE_DOC_URL, getDocURL());
			if(getNetwork())
			{
				pm.insert(prefix + DATA_LINE_NETWORK_ID, getNetwork()->getKey());
			}
			pm.insert(prefix + DATA_LINE_CREATOR_ID,
				getDataSourceLinks().size() == 1 ?
				lexical_cast<string>(getDataSourceLinks().begin()->second) :
				impex::DataSourceLinks::Serialize(getDataSourceLinks())
			);
			if(getColor())
			{
				pm.insert(prefix + DATA_LINE_COLOR, getColor()->toString());
				pm.insert(prefix + "color", getColor()->toString()); // For LinesListFunction compatibility
			}
			if(getNetwork())
			{
				getNetwork()->toParametersMap(pm, "line_network_");
			}
			pm.insert(prefix + DATA_LINE_IMAGE, getImage());
			pm.insert(prefix + "lineImage", getImage()); // For StopAreasList compatibility
			pm.insert(prefix + "image", getImage()); // For LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_TIMETABLE_ID, getTimetableId());
			if(!_displayDurationBeforeFirstDeparture.is_not_a_date_time())
			{
				pm.insert(prefix + DATA_MAX_DISPLAY_DELAY, _displayDurationBeforeFirstDeparture.total_seconds()/60);
			}
		}



		bool CommercialLine::usesTransportMode( const RollingStock& transportMode ) const
		{
			BOOST_FOREACH(const Path* path, _paths)
			{
				if(static_cast<const JourneyPattern*>(path)->getRollingStock() == &transportMode)
				{
					return true;
				}
			}
			return false;
		}



		void CommercialLine::registerService( const Service& service ) const
		{
			_servicesByNumber.insert(
				make_pair(
					service.getServiceNumber(),
					const_cast<Service*>(&service)
			)	);
		}



		void CommercialLine::unregisterService( const Service& service ) const
		{
			for(ServicesByNumber::iterator it(_servicesByNumber.lower_bound(service.getServiceNumber()));
				it != _servicesByNumber.upper_bound(service.getServiceNumber());
				++it
			){
				if(it->second == &service)
				{
					_servicesByNumber.erase(it);
					break;
				}
			}
		}



		CommercialLine::ServicesVector CommercialLine::getServices( const std::string& number ) const
		{
			ServicesVector result;
			for(ServicesByNumber::const_iterator it(_servicesByNumber.lower_bound(number));
				it != _servicesByNumber.upper_bound(number);
				++it
			){
				result.push_back(it->second);
			}
			return result;
		}



		CommercialLine::ServicesVector CommercialLine::getServices() const
		{
			ServicesVector result;
			BOOST_FOREACH(const ServicesByNumber::value_type& it, _servicesByNumber)
			{
				result.push_back(it.second);
			}
			return result;
		}



		TransportNetwork* CommercialLine::getNetwork() const
		{
			return getRoot();
		}



		bool CommercialLine::callsAtCity( const geography::City& city ) const
		{
			BOOST_FOREACH(Path* path, _paths)
			{
				if(static_cast<JourneyPattern*>(path)->callsAtCity(city))
				{
					return true;
				}
			}
			return false;
		}



		bool CommercialLine::runsAtDate( const boost::gregorian::date& day ) const
		{
			BOOST_FOREACH(const ServicesByNumber::value_type& it, _servicesByNumber)
			{
				if(	!dynamic_cast<NonPermanentService*>(it.second) ||
					static_cast<NonPermanentService*>(it.second)->isActive(day)
				){
					return true;
				}
			}
			return false;
		}



		bool CommercialLine::runsOnCalendar( const calendar::Calendar& cal ) const
		{
			BOOST_FOREACH(const ServicesByNumber::value_type& it, _servicesByNumber)
			{
				if(	!dynamic_cast<NonPermanentService*>(it.second) ||
					static_cast<NonPermanentService*>(it.second)->hasAtLeastOneCommonDateWith(cal)
				){
					return true;
				}
			}
			return false;
		}



		bool CommercialLine::runsSoon( const boost::posix_time::time_duration& when ) const
		{
			AccessParameters ap;
			ptime now(second_clock::local_time());
			ptime maxTime(now + when);
			boost::optional<Edge::DepartureServiceIndex::Value> fakeIndex;
			BOOST_FOREACH(Path* path, _paths)
			{
				if(path->getEdges().empty())
				{
					continue;
				}

				const Edge& edge(**path->getEdges().begin());
				ServicePointer nextService(
					edge.getNextService(
						ap,
						now,
						maxTime,
						false,
						fakeIndex
				)	);
				if(nextService.getService())
				{
					return true;
				}

				BOOST_FOREACH(JourneyPatternCopy* subline, static_cast<JourneyPattern*>(path)->getSubLines())
				{
					const Edge& edge(**subline->getEdges().begin());
					ServicePointer nextService(
						edge.getNextService(
							ap,
							now,
							maxTime,
							false,
							fakeIndex
					)	);
					if(nextService.getService())
					{
						return true;
					}
				}
			}

			return false;
		}
}	}
