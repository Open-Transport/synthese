
/** FreeDRTArea class implementation.
	@file FreeDRTArea.cpp

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

#include "FreeDRTArea.hpp"

#include "City.h"
#include "CityTableSync.h"
#include "CommercialLine.h"
#include "FreeDRTTimeSlot.hpp"
#include "NamedPlace.h"
#include "Profile.h"
#include "PTModule.h"
#include "PTUseRule.h"
#include "PTUseRuleTableSync.h"
#include "RollingStock.hpp"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "TransportNetwork.h"
#include "TransportNetworkRight.h"
#include "User.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace boost::posix_time;

namespace synthese
{
	using namespace geography;
	using namespace graph;
	using namespace pt;
	using namespace util;
	using namespace vehicle;

	CLASS_DEFINITION(FreeDRTArea, "t082_free_drt_areas", 82)
	FIELD_DEFINITION_OF_OBJECT(FreeDRTArea, "free_drt_area_id", "free_drt_area_ids")

	FIELD_DEFINITION_OF_TYPE(FreeDRTAreaCommercialLine, "commercial_line_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(FreeDRTAreaRollingStock, "transport_mode_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(FreeDRTAreaCities, "cities", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(FreeDRTAreaStopAreas, "stop_areas", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(UseRules, "use_rules", SQL_TEXT)

	namespace pt
	{
		FreeDRTArea::FreeDRTArea(
			util::RegistryKeyType key /*= 0 */
		):	Registrable(key),
			Object<FreeDRTArea, FreeDRTAreaSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(FreeDRTAreaCommercialLine),
					FIELD_DEFAULT_CONSTRUCTOR(FreeDRTAreaRollingStock),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(FreeDRTAreaCities),
					FIELD_DEFAULT_CONSTRUCTOR(FreeDRTAreaStopAreas),
					FIELD_DEFAULT_CONSTRUCTOR(UseRules)
			)	)
		{}

		FreeDRTArea::~FreeDRTArea()
		{
			unlink();
		}



		bool FreeDRTArea::includesPlace(
			const geography::Place& place
		) const	{
			// Check if the place is in a city included in the area
			const NamedPlace* namedPlace(dynamic_cast<const NamedPlace*>(&place));
			if(namedPlace)
			{
				Cities::const_iterator it(_cities.find(const_cast<City*>(namedPlace->getCity())));
				if( it != _cities.end())
				{
					return true;
				}
			}

			// Check if the place is a stop area included in the area
			const StopArea* stopArea(dynamic_cast<const StopArea*>(&place));
			if(stopArea)
			{
				if(	_stopAreas.find(const_cast<StopArea*>(stopArea)) != _stopAreas.end())
				{
					return true;
				}
			}

			// No valid check : the place does not belong to the area
			return false;
		}



		void FreeDRTArea::addTimeSlot(
			FreeDRTTimeSlot& value
		){
			Path::addService(value, false);
		}



		void FreeDRTArea::removeTimeSlot(
			FreeDRTTimeSlot& value
		){
			Path::removeService(value);
		}



		graph::Journey FreeDRTArea::getJourney(
			Direction direction,
			const boost::posix_time::ptime time,
			const geography::Place& from,
			const geography::Place& to
		) const	{

			// Check of geometries availability
			if(!from.getPoint().get())
			{
				throw MissingGeometryInPlaceException(*this, from);
			}
			if(!to.getPoint().get())
			{
				throw MissingGeometryInPlaceException(*this, to);
			}

			// Search for the best time slot
			const FreeDRTTimeSlot* bestTimeSlot(NULL);
			BOOST_FOREACH(const Service* service, getAllServices())
			{
				// Declarations
				const FreeDRTTimeSlot& timeSlot(static_cast<const FreeDRTTimeSlot&>(*service));

				// Checks if the time slot is active at the specified date
				if(!timeSlot.isActive(time.date()))
				{
					continue;
				}

				// Journey duration
				time_duration journeyDuration(
					timeSlot.getJourneyDuration(from, to)
				);

				// Check the time
				if(direction == DEPARTURE_TO_ARRIVAL)
				{
					time_duration lastDeparture(timeSlot.getLastArrival());
					lastDeparture -= journeyDuration;
					if(lastDeparture > time.time_of_day())
					{
						continue;
					}
				}
				else
				{
					time_duration firstArrival(timeSlot.getFirstDeparture());
					firstArrival += journeyDuration;
					if(firstArrival < time.time_of_day())
					{
						continue;
					}
				}

				// If first available time slot then it is the best
				if(!bestTimeSlot)
				{
					bestTimeSlot = &timeSlot;
					continue;
				}

				// Time slot time comparison
				if(direction == DEPARTURE_TO_ARRIVAL)
				{
					if(bestTimeSlot->getFirstDeparture() < timeSlot.getFirstDeparture())
					{
						bestTimeSlot = &timeSlot;
					}
				}
				else
				{
					if(bestTimeSlot->getLastArrival() > timeSlot.getLastArrival())
					{
						bestTimeSlot = &timeSlot;
					}
				}
			}

			// Journey build
			if(direction == DEPARTURE_TO_ARRIVAL)
			{
				return bestTimeSlot->getJourneyDepartureToArrival(time, from, to);
			}
			else
			{
				return bestTimeSlot->getJourneyArrivalToDeparture(time, from, to);
			}
		}



		CommercialLine* FreeDRTArea::getLine() const
		{
			return static_cast<CommercialLine*>(_pathGroup);
		}



		void FreeDRTArea::setLine( CommercialLine* value )
		{
			_pathGroup = value;
			set<FreeDRTAreaCommercialLine>(value
				? boost::optional<CommercialLine&>(*value)
				: boost::none);
		}



		void FreeDRTArea::setRollingStock( RollingStock* value)
		{
			_pathClass = static_cast<PathClass*>(value);
			set<FreeDRTAreaRollingStock>(value
				? boost::optional<RollingStock&>(*value)
				: boost::none);
		}
		void FreeDRTArea::setNetwork( TransportNetwork* value)
		{
			_pathNetwork = static_cast<PathClass*>(value);
		}


		RollingStock* FreeDRTArea::getRollingStock() const
		{
			return static_cast<RollingStock*>(_pathClass);
		}
		TransportNetwork* FreeDRTArea::getNetwork() const
		{
			return static_cast<TransportNetwork*>(_pathNetwork);
		}



		FreeDRTArea::ReachableStopAreas FreeDRTArea::getReachableStopAreas(
			Direction direction,
			const boost::posix_time::ptime time,
			const geography::Place& place
		) const	{

			// Check of the precondition
			assert(includesPlace(place));

			// Declarations
			ReachableStopAreas result;
			std::set<StopArea*> attemptedStopAreas;

			// Loop on cities
			BOOST_FOREACH(City* city, _cities)
			{
				// Loop on stops
				BOOST_FOREACH(const City::PlacesMatcher::Map::value_type& item, city->getLexicalMatcher(StopArea::FACTORY_KEY).entries())
				{
					// Declaration
					boost::shared_ptr<StopArea> stopArea(static_pointer_cast<StopArea, NamedPlace>(item.second));

					// Storage of the attempt
					attemptedStopAreas.insert(stopArea.get());

					// Get the journey
					Journey journey(
						getJourney(
							direction,
							time,
							direction == DEPARTURE_TO_ARRIVAL ? place : *stopArea,
							direction == DEPARTURE_TO_ARRIVAL ? *stopArea : place
					)	);

					// Storage
					if(!journey.empty())
					{
						result.insert(
							make_pair(
								stopArea.get(),
								journey
						)	);
					}
				}

			}

			// Loop on stops
			BOOST_FOREACH(StopArea* stopArea, _stopAreas)
			{
				// Avoid already attempted stops
				if(attemptedStopAreas.find(stopArea) != attemptedStopAreas.end())
				{
					continue;
				}

				// Get the journey
				Journey journey(
					getJourney(
						direction,
						time,
						direction == DEPARTURE_TO_ARRIVAL ? place : *stopArea,
						direction == DEPARTURE_TO_ARRIVAL ? *stopArea : place
				)	);

				// Storage
				if(!journey.empty())
				{
					result.insert(
						make_pair(
							stopArea,
							journey
					)	);
				}
			}

			return result;
		}



		std::string FreeDRTArea::getRuleUserName() const
		{
			return "Zone " + getName();
		}



		bool FreeDRTArea::isPedestrianMode() const
		{
			return false;
		}



		bool FreeDRTArea::isActive( const boost::gregorian::date& date ) const
		{
			return true;
		}



		bool FreeDRTArea::isDepartureAllowed() const
		{
			return true;
		}



		bool FreeDRTArea::isArrivalAllowed() const
		{
			return true;
		}



		graph::GraphIdType FreeDRTArea::getGraphType() const
		{
			return PTModule::GRAPH_ID;
		}



		FreeDRTArea::MissingGeometryInPlaceException::MissingGeometryInPlaceException(
			const FreeDRTArea& area,
			const geography::Place& place
		):	Exception(
				"The place "+ (dynamic_cast<const NamedPlace*>(&place) ? dynamic_cast<const NamedPlace&>(place).getFullName() : string()) +
				" is not located. The Free DRT Area "+ area.getName() +" ("+ lexical_cast<string>(area.getKey()) +
				") cannot use it."
			)
		{}

		void FreeDRTArea::link(util::Env& env, bool withAlgorithmOptimizations)
		{
			if(get<FreeDRTAreaCommercialLine>())
			{
				_pathGroup = get<FreeDRTAreaCommercialLine>().get_ptr();
			}
			else
			{
				_pathGroup = NULL;
			}
			if(get<FreeDRTAreaCommercialLine>())
			{
				setNetwork(get<FreeDRTAreaCommercialLine>()->getNetwork());
			}
			else
			{
				setNetwork(NULL);
			}

			_cities = UnserializeCities(get<FreeDRTAreaCities>(), env);
			_stopAreas = UnserializeStopAreas(get<FreeDRTAreaStopAreas>(), env);

			RuleUser::setRules(
				PTUseRuleTableSync::UnserializeUseRules(
					get<UseRules>(),
					env
			)	);
		}

		void FreeDRTArea::unlink()
		{
			if(getLine())
			{
				const_cast<CommercialLine*>(getLine())->removePath(this);
			}
		}

		void FreeDRTArea::setCities(const Cities& value)
		{
			_cities = value;
			string strCities = SerializeCities(_cities);
			set<FreeDRTAreaCities>(strCities);
		}

		void FreeDRTArea::setStopAreas(const StopAreas& value)
		{
			_stopAreas = value;
			string strStopAreas = SerializeStopAreas(_stopAreas);
			set<FreeDRTAreaStopAreas>(strStopAreas);
		}

		void FreeDRTArea::setRules(const Rules& value)
		{
			RuleUser::setRules(value);
			string strUseRules = PTUseRuleTableSync::SerializeUseRules(value);
			set<UseRules>(strUseRules);
		}

		bool FreeDRTArea::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool FreeDRTArea::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool FreeDRTArea::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::DELETE_RIGHT);
		}

		FreeDRTArea::Cities FreeDRTArea::UnserializeCities(
			const std::string& value,
			util::Env& env
		){
			FreeDRTArea::Cities result;
			if(!value.empty())
			{
				vector<string> cities;
				split(cities, value, is_any_of(","));
				BOOST_FOREACH(const string& city, cities)
				{
					try
					{
						RegistryKeyType cityId(lexical_cast<RegistryKeyType>(city));
						result.insert(
							CityTableSync::GetEditable(cityId, env).get()
						);
					}
					catch(ObjectNotFoundException<City>&)
					{
					}
				}
			}
			return result;
		}

		std::string FreeDRTArea::SerializeCities( const FreeDRTArea::Cities& value )
		{
			stringstream s;
			bool first(true);
			BOOST_FOREACH(const FreeDRTArea::Cities::value_type& city, value)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				s << city->getKey();
			}
			return s.str();
		}

		FreeDRTArea::StopAreas FreeDRTArea::UnserializeStopAreas(
			const std::string& value,
			util::Env& env
		){
			FreeDRTArea::StopAreas result;
			if(!value.empty())
			{
				vector<string> stopAreas;
				split(stopAreas, value, is_any_of(","));
				BOOST_FOREACH(const string& stopArea, stopAreas)
				{
					try
					{
						RegistryKeyType stopAreaId(lexical_cast<RegistryKeyType>(stopArea));
						result.insert(
							StopAreaTableSync::GetEditable(stopAreaId, env).get()
						);
					}
					catch(ObjectNotFoundException<City>&)
					{
					}
				}
			}
			return result;
		}

		std::string FreeDRTArea::SerializeStopAreas( const FreeDRTArea::StopAreas& value )
		{
			stringstream s;
			bool first(true);
			BOOST_FOREACH(const FreeDRTArea::StopAreas::value_type& stopArea, value)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				s << stopArea->getKey();
			}
			return s.str();
		}
}	}
