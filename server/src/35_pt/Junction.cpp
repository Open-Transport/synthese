
/** Junction class implementation.
	@file Junction.cpp

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

#include "Junction.hpp"

#include "AllowedUseRule.h"
#include "JunctionStop.hpp"
#include "JunctionTableSync.hpp"
#include "PermanentService.h"
#include "Profile.h"
#include "StopPointTableSync.hpp"
#include "StopArea.hpp"
#include "TransportNetworkRight.h"
#include "User.h"

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace graph;
	using namespace pt;

	CLASS_DEFINITION(Junction, "t066_junctions", 66)
	FIELD_DEFINITION_OF_OBJECT(Junction, "junction_id", "junctions_ids")

	FIELD_DEFINITION_OF_TYPE(StartPhysicalStop, "start_physical_stop", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(EndPhysicalStop, "end_physical_stop", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Length, "length", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DurationMinutes, "duration", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Bidirectional, "bidirectional", SQL_BOOLEAN)

	namespace pt
	{
		Junction::Junction(
			util::RegistryKeyType id
		):	Registrable(id),
			Object<Junction, JunctionSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(StartPhysicalStop),
					FIELD_DEFAULT_CONSTRUCTOR(EndPhysicalStop),
					FIELD_DEFAULT_CONSTRUCTOR(Length),
					FIELD_DEFAULT_CONSTRUCTOR(DurationMinutes),
					FIELD_DEFAULT_CONSTRUCTOR(Bidirectional)
			)	)
		{
			// Default use rules
			RuleUser::Rules rules(getRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			setRules(rules);
		}



		Junction::~Junction()
		{
			_clean();
		}



		void Junction::setStops(
			StopPoint* start,
			StopPoint* end,
			double length,
			boost::posix_time::time_duration duration,
			bool doBack,
			bool setDBValues
		){
			// Cleaning existing data
			if(!_edges.empty())
			{
				_clean();
			}
			_back.reset();

			// Generation of edges
			Edge* startEdge(new JunctionStop(
					this,
					start
			)	);
			addEdge(*startEdge);
			start->addDepartureEdge(startEdge);

			Edge* endEdge(new JunctionStop(
					length,
					this,
					end
			)	);
			addEdge(*endEdge);
			end->addArrivalEdge(endEdge);

			// Generation of service
			Service* service(new PermanentService(
					0,
					this,
					duration
			)	);
			addService(*service, false);
			service->setPath(this);


			// Generation of back
			if(doBack)
			{
				_back.reset(new Junction);
				_back->setStops(end, start, length, duration, false);
			}

			if (setDBValues)
			{
				set<StartPhysicalStop>(start
					? boost::optional<StopPoint&>(*start)
					: boost::none);
				set<EndPhysicalStop>(end
					? boost::optional<StopPoint&>(*end)
					: boost::none);
				set<Length>(length);
				set<DurationMinutes>(duration.minutes());
				set<Bidirectional>(doBack);
			}
		}



		boost::shared_ptr<Junction> Junction::getBack() const
		{
			return _back;
		}



		StopPoint* Junction::getStart() const
		{
			assert(isValid());
			return dynamic_cast<StopPoint*>((*_edges.begin())->getFromVertex());
		}



		StopPoint* Junction::getEnd() const
		{
			assert(isValid());
			return dynamic_cast< StopPoint*>((*(_edges.begin()+1))->getFromVertex());
		}



		bool Junction::isValid() const
		{
			ServiceSet services(getAllServices());
			return _edges.size() == 2 && services.size() == 1 && static_cast<PermanentService*>(*services.begin())->getDuration();
		}



		double Junction::getLength() const
		{
			assert(isValid());
			return (*(_edges.begin()+1))->getMetricOffset();
		}



		boost::posix_time::time_duration Junction::getDuration() const
		{
			assert(isValid());
			ServiceSet services(getAllServices());
			return *static_cast<PermanentService*>(*services.begin())->getDuration();
		}



		bool Junction::isPedestrianMode() const
		{
			return true;
		}



		bool Junction::isRoad() const
		{
			return true;
		}



		std::string Junction::getRuleUserName() const
		{
			return "Jonction " + getStart()->getConnectionPlace()->getFullName() + " -> " + getEnd()->getConnectionPlace()->getFullName();
		}



		bool Junction::isActive( const boost::gregorian::date& date ) const
		{
			return true;
		}



		void Junction::_clean()
		{
			// Edges
			for (Path::Edges::iterator it(_edges.begin()); it != _edges.end(); ++it)
			{
				Vertex* stop((*it)->getFromVertex());

				// Removing edge from stop point
				if(it == _edges.begin())
				{
					stop->removeDepartureEdge(*it);
				}
				else
				{
					stop->removeArrivalEdge(*it);
				}

				delete *it;
			}

			// Services
			ServiceSet services(getAllServices());
			for (ServiceSet::iterator it(services.begin()); it != services.end(); ++it)
			{
				delete *it;
			}
		}


		
		void Junction::toParametersMap( util::ParametersMap& pm, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			pm.insert(TABLE_COL_ID, getKey());
			pm.insert(
				StartPhysicalStop::FIELD.name,
				isValid() ? getStart()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				EndPhysicalStop::FIELD.name,
				isValid() ? getEnd()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				Length::FIELD.name,
				isValid() ? getLength() : double(0)
			);
			pm.insert(
				DurationMinutes::FIELD.name,
				isValid() ? getDuration().total_seconds() / 60 : 0
			);
			pm.insert(
				Bidirectional::FIELD.name,
				getBack() != NULL
			);
		}


		 
		synthese::SubObjects Junction::getSubObjects() const
		{
			SubObjects result;		
			return result;
		}



		synthese::LinkedObjectsIds Junction::getLinkedObjectsIds( const Record& record ) const
		{
			LinkedObjectsIds result;
			RegistryKeyType sid(get<StartPhysicalStop>() ? get<StartPhysicalStop>()->getKey() : RegistryKeyType(0));
			if(sid)
			{
				result.push_back(sid);
			}
			RegistryKeyType eid(get<EndPhysicalStop>() ? get<EndPhysicalStop>()->getKey() : RegistryKeyType(0));
			if(eid)
			{
				result.push_back(eid);
			}
			return result;
		}



		void Junction::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			time_duration duration(minutes(get<DurationMinutes>()));
			try
			{
				if (get<StartPhysicalStop>() && get<EndPhysicalStop>())
				{
					setStops(
						get<StartPhysicalStop>().get_ptr(),
						get<EndPhysicalStop>().get_ptr(),
						get<Length>(),
						duration,
						get<Bidirectional>(),
						false
					);
				}
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				Log::GetInstance().warn("No such stop in Junction "+ lexical_cast<string>(getKey()));
			}
		}

		bool Junction::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool Junction::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool Junction::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::DELETE_RIGHT);
		}

}	}
