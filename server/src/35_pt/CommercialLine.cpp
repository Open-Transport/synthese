
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
#include "AllowedUseRule.h"
#include "CalendarTemplateTableSync.h"
#include "CommercialLineTableSync.h"
#include "DataSourceLinksField.hpp"
#include "DBConstants.h"
#include "Edge.h"
#include "Fare.hpp"
#include "ForbiddenUseRule.h"
#include "GraphConstants.h"
#include "ImportableTableSync.hpp"
#include "JourneyPattern.hpp"
#include "NonPermanentService.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTModule.h"
#include "PTUseRuleTableSync.h"
#include "Registry.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"
#include "RollingStock.hpp"
#include "StopAreaTableSync.hpp"
#include "TransportNetworkRight.h"
#include "TransportNetworkTableSync.h"
#include "TreeFolderTableSync.hpp"
#include "User.h"

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
		template<> const std::string Registry<synthese::tree::TreeFolderUpNode>::KEY("CommercialLine");
	}

	CLASS_DEFINITION(CommercialLine, "t042_commercial_lines", 42)
	FIELD_DEFINITION_OF_OBJECT(CommercialLine, "commercial_line_id", "commercial_line_ids")

	FIELD_DEFINITION_OF_TYPE(Network, "network_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ShortName, "short_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(LongName, "long_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Color, "color", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ForegroundColor, "foreground_color", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Style, "style", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(LineImage, "image", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(OptionalReservationPlaces, "optional_reservation_places", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(CreatorId, "creator_id", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(BikeComplianceId, "bike_compliance_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(HandicappedComplianceId, "handicapped_compliance_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(PedestrianComplianceId, "pedestrian_compliance_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LineReservationContact, "reservation_contact_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LineCalendarTemplate, "calendar_template_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MapUrl, "map_url", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DocUrl, "doc_url", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(TimetableId, "timetable_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DisplayDurationBeforeFirstDeparture, "display_duration_before_first_departure", SQL_TIME)
	FIELD_DEFINITION_OF_TYPE(WeightForSorting, "weight_for_sorting", SQL_INTEGER)

	namespace pt
	{
		const string CommercialLine::DATA_LINE_SHORT_NAME("line_short_name");
		const string CommercialLine::DATA_LINE_LONG_NAME("line_long_name");
		const string CommercialLine::DATA_LINE_NAME("lineName");
		const string CommercialLine::DATA_LINE_COLOR("line_color");
		const string CommercialLine::DATA_LINE_FOREGROUND_COLOR("line_foreground_color");
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
			Object<CommercialLine, CommercialLineSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(Network),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(ShortName),
					FIELD_DEFAULT_CONSTRUCTOR(LongName),
					FIELD_DEFAULT_CONSTRUCTOR(Color),
					FIELD_DEFAULT_CONSTRUCTOR(ForegroundColor),
					FIELD_DEFAULT_CONSTRUCTOR(Style),
					FIELD_DEFAULT_CONSTRUCTOR(LineImage),
					FIELD_DEFAULT_CONSTRUCTOR(OptionalReservationPlaces),
					FIELD_DEFAULT_CONSTRUCTOR(CreatorId),
					FIELD_DEFAULT_CONSTRUCTOR(BikeComplianceId),
					FIELD_DEFAULT_CONSTRUCTOR(HandicappedComplianceId),
					FIELD_DEFAULT_CONSTRUCTOR(PedestrianComplianceId),
					FIELD_DEFAULT_CONSTRUCTOR(LineReservationContact),
					FIELD_DEFAULT_CONSTRUCTOR(LineCalendarTemplate),
					FIELD_DEFAULT_CONSTRUCTOR(MapUrl),
					FIELD_DEFAULT_CONSTRUCTOR(DocUrl),
					FIELD_VALUE_CONSTRUCTOR(TimetableId, 0),
					FIELD_VALUE_CONSTRUCTOR(DisplayDurationBeforeFirstDeparture, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(WeightForSorting, 0)
			)	),
			graph::PathGroup(key)
		{
			// Default use rules
			RuleUser::Rules rules(getRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			setRules(rules);
		}

		CommercialLine::~CommercialLine()
		{
			unlink();
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
				BOOST_FOREACH(const Service* service, path->getAllServices())
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
				BOOST_FOREACH(const Service* service, path->getAllServices())
				{
					service->clearNonConcurrencyCache();
				}
			}
		}



		bool CommercialLine::operator<(const CommercialLine& cl) const {
			if(getNetwork() == cl.getNetwork())
			{
				if(getWeightForSorting() == cl.getWeightForSorting())
				{
					if ((getShortName().empty() && cl.getShortName().empty()) ||
						(getShortName() == cl.getShortName()))
					{
						return getKey() < cl.getKey();
					}
					else
					{
						if(PTModule::getSortLettersBeforeNumbers())
						{
							return (alphanum_text_first_comp<string>(getShortName(), cl.getShortName()) < 0);
						}
						else
						{
							return (alphanum_comp<string>(getShortName(), cl.getShortName()) < 0);
						}
					}
				}
				else
				{
					return (getWeightForSorting() > cl.getWeightForSorting());
				}
			}
			else if(!getNetwork())
			{
				return false;
			}
			else if(!cl.getNetwork())
			{
				return true;
			}
			else
			{
				return (getNetwork()->getKey() < cl.getNetwork()->getKey());
			}
		}



		bool CommercialLine::respectsCalendarTemplate(
			date_duration duration
		) const	{
			if(!get<LineCalendarTemplate>())
			{
				return true;
			}

			date now(day_clock::local_day());
			date endDate(now + duration);
			Calendar period(now, endDate);

			Calendar targetCalendar(get<LineCalendarTemplate>()->getResult(period));

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
				BOOST_FOREACH(const Service* service, path->getAllServices())
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
				prefix + Network::FIELD.name,
				_getParent() ? _getParent()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + SimpleObjectFieldDefinition<Name>::FIELD.name,
				getName()
			);
			pm.insert(
				prefix + ShortName::FIELD.name,
				getShortName()
			);
			pm.insert(
				prefix + LongName::FIELD.name,
				getLongName()
			);
			pm.insert(
				prefix + "xmlcolor",
				getColor() ? getColor()->toXMLColor() : string()
			);
			pm.insert(
				prefix + Style::FIELD.name,
				getStyle()
			);
			pm.insert(
				prefix + Image::FIELD.name,
				getImage()
			);
			pm.insert(
				prefix + OptionalReservationPlaces::FIELD.name,
				optionalReservationPlaces.str()
			);
			pm.insert(
				prefix + CreatorId::FIELD.name,
				synthese::DataSourceLinks::Serialize(getDataSourceLinks())
			);
			pm.insert(
				prefix + BikeComplianceId::FIELD.name,
				(	getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(getRule(USER_BIKE)) ?
					static_cast<const PTUseRule*>(getRule(USER_BIKE))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + HandicappedComplianceId::FIELD.name,
				(	getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
					static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + PedestrianComplianceId::FIELD.name,
				(	getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN)) ?
					static_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + LineReservationContact::FIELD.name,
				getReservationContact() ? getReservationContact()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + LineCalendarTemplate::FIELD.name,
				getCalendarTemplate() ? getCalendarTemplate()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + MapUrl::FIELD.name,
				getMapURL()
			);
			pm.insert(
				prefix + DocUrl::FIELD.name,
				getDocURL()
			);
			pm.insert(
				prefix + TimetableId::FIELD.name,
				getTimetableId()
			);
			pm.insert(
				prefix + DisplayDurationBeforeFirstDeparture::FIELD.name,
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
				pm.insert(prefix + Color::FIELD.name, getColor()->toXMLColor()); // Maybe break CMS views ! but needed for load in inter_synthese_package
			}

			if(getFgColor())
			{
				pm.insert(prefix + DATA_LINE_FOREGROUND_COLOR, getFgColor()->toString());
				pm.insert(prefix + ForegroundColor::FIELD.name, getFgColor()->toXMLColor()); // Maybe break CMS views ! but needed for load in inter_synthese_package
			}

			if(getNetwork())
			{
				getNetwork()->toParametersMap(pm, withAdditionalParameters, withFiles, "line_network_");
			}
			pm.insert(prefix + DATA_LINE_IMAGE, getImage());
			pm.insert(prefix + "lineImage", getImage()); // For StopAreasList compatibility
			pm.insert(prefix + "image", getImage()); // For LinesListFunction compatibility
			BOOST_FOREACH(const Path* path, _paths)
			{
				// Jump over paths with non defined transport mode
				if(!static_cast<const JourneyPattern*>(path)->getRollingStock())
				{
					continue;
				}

				pm.insert(prefix + "transportMode", static_cast<const JourneyPattern*>(path)->getRollingStock()->getName());
			}
			pm.insert(prefix + DATA_LINE_TIMETABLE_ID, getTimetableId());
			if(!get<DisplayDurationBeforeFirstDeparture>().is_not_a_date_time())
			{
				pm.insert(prefix + DATA_MAX_DISPLAY_DELAY, get<DisplayDurationBeforeFirstDeparture>().total_seconds()/60);
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

				BOOST_FOREACH(const Path::ServiceCollections::value_type& itCollection, path->getServiceCollections())
				{
					ServicePointer nextService(
						edge.getNextService(
							*itCollection,
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


		synthese::SubObjects CommercialLine::getSubObjects() const
		{
			SubObjects r;
			BOOST_FOREACH(Path* path, getPaths())
			{
				r.push_back(path);
			}
			return r;
		}


		void CommercialLine::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			// Color
			optional<RGBColor> value;
			string color(get<Color>());
			if(!color.empty())
			{
				try
				{
					value = RGBColor::FromXMLColor(color);
					_color = value;
				}
				catch(RGBColor::Exception&)
				{
					Log::GetInstance().warn("No such color "+ color +" in commercial line "+ lexical_cast<string>(getKey()));
				}
			}

			// Foreground color
			color = get<ForegroundColor>();
			if(!color.empty())
			{
				try
				{
					value = RGBColor::FromXMLColor(color);
					_fgColor = value;
				}
				catch(RGBColor::Exception&)
				{
					Log::GetInstance().warn("No such foreground color "+ color +" in commercial line "+ lexical_cast<string>(getKey()));
				}
			}

			// Parent network
			if(get<Network>())
			{
				TreeFolderUpNode* parentValue(dynamic_cast<TreeFolderUpNode*>(&(get<Network>().get())));
				_setParent(*parentValue);
			}
			else
			{
				setNullParent();
			}

			// Places with optional reservation
			std::vector<std::string> stops;
			CommercialLine::PlacesSet placesWithOptionalReservation;
			string colORP(get<OptionalReservationPlaces>());
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
			_optionalReservationPlaces = placesWithOptionalReservation;

			// Use rules
			RuleUser::Rules rules(getRules());

			// Bike compliance
			if(get<BikeComplianceId>())
			{
				if(get<BikeComplianceId>() > 0)
				{
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(get<BikeComplianceId>(), env).get();
				}
				else
				{
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = NULL;
				}
			}

			// Handicapped compliance
			if(get<HandicappedComplianceId>())
			{
				if(get<HandicappedComplianceId>() > 0)
				{
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(get<HandicappedComplianceId>(), env).get();
				}
				else
				{
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = NULL;
				}
			}

			// Pedestrian compliance
			if(get<PedestrianComplianceId>())
			{
				if(get<PedestrianComplianceId>() > 0)
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(get<PedestrianComplianceId>(), env).get();
				}
				else
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = NULL;
				}
			}
			setRules(rules);

			if(&env == &Env::GetOfficialEnv())
			{
				setDataSourceLinksWithRegistration(getDataSourceLinks());
			}
		}

		void CommercialLine::unlink()
		{
			removeParentLink();

			if(Env::GetOfficialEnv().contains(*this))
			{
				cleanDataSourceLinks(true);
			}
		}

		void CommercialLine::setOptionalReservationPlaces(const PlacesSet& value)
		{
			_optionalReservationPlaces = value;
			stringstream optionalReservationPlaces;
			bool first(true);
			BOOST_FOREACH(const StopArea* place, _optionalReservationPlaces)
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
			set<OptionalReservationPlaces>(optionalReservationPlaces.str());
		}

		void CommercialLine::setColor (const boost::optional<util::RGBColor>& color)
		{
			_color = color;
			_color ? set<Color>(_color->toXMLColor()) : set<Color>(string());
		}

		void CommercialLine::setFgColor (const boost::optional<util::RGBColor>& color)
		{
			_fgColor = color;
			_fgColor ? set<ForegroundColor>(_fgColor->toXMLColor()) : set<ForegroundColor>(string());
		}

		void CommercialLine::setRules(const Rules& value)
		{
			RuleUser::setRules(value);
			getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(getRule(USER_BIKE)) ?
				set<BikeComplianceId>(static_cast<const PTUseRule*>(getRule(USER_BIKE))->getKey()) :
				set<BikeComplianceId>(RegistryKeyType(0));
			getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
				set<HandicappedComplianceId>(static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey()) :
				set<HandicappedComplianceId>(RegistryKeyType(0));
			getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN)) ?
				set<PedestrianComplianceId>(static_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN))->getKey()) :
				set<PedestrianComplianceId>(RegistryKeyType(0));
		}

		void CommercialLine::setReservationContact(ReservationContact* value)
		{
			set<LineReservationContact>(value
				? boost::optional<ReservationContact&>(*value)
				: boost::none);
		}

		void CommercialLine::setCalendarTemplate(calendar::CalendarTemplate* value)
		{
			set<LineCalendarTemplate>(value
				? boost::optional<calendar::CalendarTemplate&>(*value)
				: boost::none);
		}

		bool CommercialLine::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool CommercialLine::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool CommercialLine::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::DELETE_RIGHT);
		}

		const pt::ReservationContact* CommercialLine::getReservationContact() const
		{
			return get<LineReservationContact>() ? get<LineReservationContact>().get_ptr() : NULL;
		}

		calendar::CalendarTemplate* CommercialLine::getCalendarTemplate() const
		{
			return get<LineCalendarTemplate>() ? get<LineCalendarTemplate>().get_ptr() : NULL;
		}
}	}
