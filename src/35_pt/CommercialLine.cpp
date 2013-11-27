
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

#include "alphanum.hpp"
#include "AccessParameters.h"
#include "CalendarTemplateTableSync.h"
#include "CommercialLineTableSync.h"
#include "DBConstants.h"
#include "Edge.h"
#include "ForbiddenUseRule.h"
#include "DataSourceLinksField.hpp"
#include "PTModule.h"
#include "PTUseRuleTableSync.h"
#include "Registry.h"
#include "ReservationContactTableSync.h"
#include "StopAreaTableSync.hpp"
#include "GraphConstants.h"
#include "AllowedUseRule.h"
#include "JourneyPattern.hpp"
#include "NonPermanentService.h"
#include "JourneyPatternCopy.hpp"
#include "ImportableTableSync.hpp"
#include "ParametersMap.h"
#include "TransportNetworkTableSync.h"
#include "TreeFolderTableSync.hpp"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace calendar;
	using namespace db;
	using namespace graph;
	using namespace impex;
	using namespace pt;
	using namespace tree;
	using namespace util;
	using namespace vehicle;

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
		const string CommercialLine::DATA_MAX_DISPLAY_DELAY = "max_display_delay";



		CommercialLine::CommercialLine(
			RegistryKeyType key
		):	util::Registrable(key),
			graph::PathGroup(key),
			_reservationContact(NULL),
			_calendarTemplate(NULL),
			_timetableId(0),
			_displayDurationBeforeFirstDeparture(not_a_date_time),
			_weightForSorting(0)
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



		bool CommercialLine::operator<(
			const CommercialLine& cl
		) const {
			if(getNetwork() == cl.getNetwork())
			{
				if(getWeightForSorting() == cl.getWeightForSorting())
				{
					// Handle empty short names
					if(getShortName().empty() && cl.getShortName().empty())
						return getKey() < cl.getKey();
					else
					{
						if(PTModule::getSortLettersBeforeNumbers())
							return (alphanum_text_first_comp<string>(getShortName(), cl.getShortName()) < 0);
						else
							return (alphanum_comp<string>(getShortName(), cl.getShortName()) < 0);
					}
				}
				else
					return (getWeightForSorting() > cl.getWeightForSorting());
			}
			else
				return (getNetwork()->getKey() < cl.getNetwork()->getKey());
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

			// Preparation of places with optional reservation
			stringstream optionalReservationPlaces;
			bool first(true);
			BOOST_FOREACH(const StopArea* place, getOptionalReservationPlaces())
			{
				if (first)
				{
					first = false;
				}
				else
				{
					optionalReservationPlaces << ",";
				}
				optionalReservationPlaces << place->getKey();
			}

			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + CommercialLineTableSync::COL_NETWORK_ID,
				_getParent() ? _getParent()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_NAME,
				getName()
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_SHORT_NAME,
				getShortName()
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_LONG_NAME,
				getLongName()
			);
			pm.insert(
				prefix + "xmlcolor",
				getColor() ? getColor()->toXMLColor() : string()
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_STYLE,
				getStyle()
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_IMAGE,
				getImage()
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_OPTIONAL_RESERVATION_PLACES,
				optionalReservationPlaces.str()
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_CREATOR_ID,
				synthese::DataSourceLinks::Serialize(getDataSourceLinks())
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_BIKE_USE_RULE,
				(	getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(getRule(USER_BIKE)) ?
					static_cast<const PTUseRule*>(getRule(USER_BIKE))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + CommercialLineTableSync::COL_HANDICAPPED_USE_RULE,
				(	getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
					static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + CommercialLineTableSync::COL_PEDESTRIAN_USE_RULE,
				(	getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN)) ?
					static_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + CommercialLineTableSync::COL_RESERVATION_CONTACT_ID,
				getReservationContact() ? getReservationContact()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_CALENDAR_TEMPLATE_ID,
				getCalendarTemplate() ? getCalendarTemplate()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_MAP_URL,
				getMapURL()
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_DOC_URL,
				getDocURL()
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_TIMETABLE_ID,
				getTimetableId()
			);
			pm.insert(
				prefix + CommercialLineTableSync::COL_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE,
				(	getDisplayDurationBeforeFirstDeparture().is_not_a_date_time() ?
					string() :
					lexical_cast<string>(getDisplayDurationBeforeFirstDeparture().total_seconds() / 60)
			)	);

			pm.insert(prefix + DATA_LINE_ID, getKey());
			pm.insert(prefix + DATA_LINE_SHORT_NAME, getShortName());
			pm.insert(prefix + "lineShortName", getShortName()); // For StopAreasList compatibility
			pm.insert(prefix + "shortName", getShortName()); // For LinesListFunction/StopPointsListFunction compatibility
			pm.insert(prefix + DATA_LINE_LONG_NAME, getLongName());
			pm.insert(prefix + "longName", getLongName()); // For LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_NAME, getName());
			pm.insert(prefix + "name", getName()); // For LinesListFunction compatibility
			pm.insert(prefix + DATA_LINE_STYLE, getStyle());
			pm.insert(prefix + "lineStyle", getStyle()); // For StopAreasList compatibility
			pm.insert(prefix + DATA_LINE_MAP_URL, getMapURL());
			pm.insert(prefix + DATA_LINE_DOC_URL, getDocURL());
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
				getNetwork()->toParametersMap(pm, withAdditionalParameters, withFiles, "line_network_");
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


		bool CommercialLine::loadFromRecord(
			const Record& record,
			util::Env& env
		){
			bool result(false);

			// Name
			if(record.isDefined(CommercialLineTableSync::COL_NAME))
			{
				string value(
					record.get<string>(CommercialLineTableSync::COL_NAME)
				);
				if(value != _name)
				{
					_name = value;
					result = true;
				}
			}

			// Short name
			if(record.isDefined(CommercialLineTableSync::COL_SHORT_NAME))
			{
				string value(
					record.get<string>(CommercialLineTableSync::COL_SHORT_NAME)
				);
				if(value != _shortName)
				{
					_shortName = value;
					result = true;
				}
			}

			// Long name
			if(record.isDefined(CommercialLineTableSync::COL_LONG_NAME))
			{
				string value(
					record.get<string>(CommercialLineTableSync::COL_LONG_NAME)
				);
				if(value != _longName)
				{
					_longName = value;
					result = true;
				}
			}

			// Map url
			if(record.isDefined(CommercialLineTableSync::COL_MAP_URL))
			{
				string value(
					record.get<string>(CommercialLineTableSync::COL_MAP_URL)
				);
				if(value != _mapURL)
				{
					_mapURL = value;
					result = true;
				}
			}

			// Doc url
			if(record.isDefined(CommercialLineTableSync::COL_DOC_URL))
			{
				string value(
					record.get<string>(CommercialLineTableSync::COL_DOC_URL)
				);
				if(value != _docURL)
				{
					_docURL = value;
					result = true;
				}
			}

			// Timetable id
			if(record.isDefined(CommercialLineTableSync::COL_TIMETABLE_ID))
			{
				RegistryKeyType value(
					record.getDefault<RegistryKeyType>(
						CommercialLineTableSync::COL_TIMETABLE_ID,
						0
				)	);
				if(value != _timetableId)
				{
					_timetableId = value;
					result = true;
				}
			}

			// Display duration before first departure
			if(record.isDefined(CommercialLineTableSync::COL_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE))
			{
				time_duration value(not_a_date_time);
				string str(
					record.get<string>(
						CommercialLineTableSync::COL_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE
				)	);
				if(!str.empty())
				{
					value = minutes(
						record.getDefault<long>(
							CommercialLineTableSync::COL_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE,
							0
					)	);
				}

				if(value != _displayDurationBeforeFirstDeparture)
				{
					_displayDurationBeforeFirstDeparture = value;
					result = true;
				}
			}

			// Color
			if(record.isDefined(CommercialLineTableSync::COL_COLOR))
			{
				optional<RGBColor> value;
				string color(record.get<string>(CommercialLineTableSync::COL_COLOR));
				if(!color.empty())
				{
					try
					{
						value = RGBColor::FromXMLColor(color);
					}
					catch(RGBColor::Exception&)
					{
						Log::GetInstance().warn("No such color "+ color +" in commercial line "+ lexical_cast<string>(getKey()));
					}
				}
				if(value != _color)
				{
					_color = value;
					result = true;
				}
			}

			// Style
			if(record.isDefined(CommercialLineTableSync::COL_STYLE))
			{
				string value(record.get<string>(CommercialLineTableSync::COL_STYLE));
				if(value != _style)
				{
					_style = value;
					result = true;
				}
			}

			if(record.isDefined(CommercialLineTableSync::COL_WEIGHT_FOR_SORTING))
			{
				int value(record.getDefault<int>(CommercialLineTableSync::COL_WEIGHT_FOR_SORTING, 0));
				if(value != _weightForSorting)
				{
					_weightForSorting = value;
					result = true;
				}
			}

			// Image
			if(record.isDefined(CommercialLineTableSync::COL_IMAGE))
			{
				string value(record.get<string>(CommercialLineTableSync::COL_IMAGE));
				if(value != _image)
				{
					_image = value;
					result = true;
				}
			}

//			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
//			{
				// Transport network
				if(record.isDefined(CommercialLineTableSync::COL_NETWORK_ID))
				{
					TreeFolderUpNode* value(NULL);
					try
					{
						RegistryKeyType parentId(
							record.getDefault<RegistryKeyType>(
								CommercialLineTableSync::COL_NETWORK_ID,
								0
						)	);
						if(decodeTableId(parentId) == TransportNetworkTableSync::TABLE.ID)
						{
							value = TransportNetworkTableSync::GetEditable(parentId, env).get();
						}
						else if(decodeTableId(parentId) == TreeFolder::CLASS_NUMBER)
						{
							value = TreeFolderTableSync::GetEditable(parentId, env).get();
						}
					}
					catch(ObjectNotFoundException<TransportNetwork>&)
					{
						Log::GetInstance().warn("No such network in commercial line "+ lexical_cast<string>(getKey()));
					}
					catch(ObjectNotFoundException<TreeFolderTableSync>&)
					{
						Log::GetInstance().warn("No such folder in commercial line "+ lexical_cast<string>(getKey()));
					}

					if(value != _getParent())
					{
						if(value)
						{
							_setParent(*value);
						}
						else
						{
							setNullParent();
						}
						result = true;
					}
				}

				// Calendar template
				if(record.isDefined(CommercialLineTableSync::COL_CALENDAR_TEMPLATE_ID))
				{
					CalendarTemplate* value(NULL);
					RegistryKeyType id(
						record.getDefault<RegistryKeyType>(
							CommercialLineTableSync::COL_CALENDAR_TEMPLATE_ID,
							0
					)	);
					if(id > 0)
					{
						try
						{
							value = CalendarTemplateTableSync::GetEditable(id, env).get();
						}
						catch(ObjectNotFoundException<CalendarTemplate>&)
						{
							Log::GetInstance().warn("No such calendar template in commercial line "+ lexical_cast<string>(getKey()));
						}
					}
					if(value != _calendarTemplate)
					{
						_calendarTemplate = value;
						result = true;
					}
				}

				// Places with optional reservation
				// Parse all optional reservation places separated by ,
				if(record.isDefined(CommercialLineTableSync::COL_OPTIONAL_RESERVATION_PLACES))
				{
					std::vector<std::string> stops;
					CommercialLine::PlacesSet placesWithOptionalReservation;
					string colORP(record.get<string>(CommercialLineTableSync::COL_OPTIONAL_RESERVATION_PLACES));
					boost::split(
						stops,
						colORP,
						boost::is_any_of(",")
					);
					BOOST_FOREACH(const string& stop, stops)
					{
						if(stop.empty()) continue;
						try
						{
							placesWithOptionalReservation.insert(
								StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(stop),env).get()
							);
						}
						catch(ObjectNotFoundException<StopArea>&)
						{
							Log::GetInstance().warn("No such place "+ stop +" in optional reservation places of commercial line "+ lexical_cast<string>(getKey()));
						}
					}
					if(placesWithOptionalReservation != _optionalReservationPlaces)
					{
						_optionalReservationPlaces = placesWithOptionalReservation;
						result = true;
					}
				}

				// Use rules
				RuleUser::Rules rules(getRules());

				// Bike compliance
				if(record.isDefined(CommercialLineTableSync::COL_BIKE_USE_RULE))
				{
					RegistryKeyType bikeComplianceId(
						record.getDefault<RegistryKeyType>(CommercialLineTableSync::COL_BIKE_USE_RULE, 0)
					);
					if(bikeComplianceId > 0)
					{
						rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(bikeComplianceId, env).get();
					}
					else
					{
						rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = NULL;
					}
				}

				// Handicapped compliance
				if(record.isDefined(CommercialLineTableSync::COL_HANDICAPPED_USE_RULE))
				{
					RegistryKeyType handicappedComplianceId(
						record.getDefault<RegistryKeyType>(CommercialLineTableSync::COL_HANDICAPPED_USE_RULE, 0)
					);
					if(handicappedComplianceId > 0)
					{
						rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(handicappedComplianceId, env).get();
					}
					else
					{
						rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = NULL;
					}
				}

				// Pedestrian compliance
				if(record.isDefined(CommercialLineTableSync::COL_PEDESTRIAN_USE_RULE))
				{
					RegistryKeyType pedestrianComplianceId(
						record.getDefault<RegistryKeyType>(CommercialLineTableSync::COL_PEDESTRIAN_USE_RULE, 0)
					);
					if(pedestrianComplianceId > 0)
					{
						rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(pedestrianComplianceId, env).get();
					}
					else
					{
						rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = NULL;
					}
				}

				if(rules != getRules())
				{
					setRules(rules);
					result = true;
				}

				// Reservation contact
				if(record.isDefined(CommercialLineTableSync::COL_RESERVATION_CONTACT_ID))
				{
					const ReservationContact* value(NULL);
					RegistryKeyType reservationContactId(
						record.get<RegistryKeyType>(CommercialLineTableSync::COL_RESERVATION_CONTACT_ID)
					);
					if(reservationContactId > 0)
					{
						value = ReservationContactTableSync::Get(reservationContactId, env).get();
					}
					if(value != _reservationContact)
					{
						_reservationContact = value;
						result = true;
					}
				}
//			}

			// Data source links (at the end of the load to avoid registration of objects which are removed later by an exception)
			if(record.isDefined(CommercialLineTableSync::COL_CREATOR_ID))
			{
				Importable::DataSourceLinks value(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						record.get<string>(CommercialLineTableSync::COL_CREATOR_ID),
						env
				)	);
				if(value != getDataSourceLinks())
				{
					if(&env == &Env::GetOfficialEnv())
					{
						setDataSourceLinksWithRegistration(value);
					}
					else
					{
						setDataSourceLinksWithoutRegistration(value);
					}
					result = true;
				}
			}

			return result;
		}

		synthese::SubObjects CommercialLine::getSubObjects() const
		{
			SubObjects r;
			BOOST_FOREACH(Path* path, getPaths())
			{
				// Avoid sublines
				if(dynamic_cast<JourneyPatternCopy*>(path))
				{
					continue;
				}

				r.push_back(path);
			}
			return r;
		}



		synthese::LinkedObjectsIds CommercialLine::getLinkedObjectsIds( const Record& record ) const
		{
			return LinkedObjectsIds();
		}



		void CommercialLine::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(&env == &Env::GetOfficialEnv())
			{
				setDataSourceLinksWithRegistration(getDataSourceLinks());
			}

			setParentLink();
		}
}	}
