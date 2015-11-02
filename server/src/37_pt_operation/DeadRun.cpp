
/** DeadRun class implementation.
	@file DeadRun.cpp

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

#include "DeadRun.hpp"

#include "DataSourceLinksField.hpp"
#include "DeadRunEdge.hpp"
#include "DeadRunTableSync.hpp"
#include "Depot.hpp"
#include "ForbiddenUseRule.h"
#include "ImportableTableSync.hpp"
#include "OperationUnit.hpp"
#include "Profile.h"
#include "StopPoint.hpp"
#include "TransportNetwork.h"
#include "User.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace graph;
	using namespace impex;
	using namespace pt;
	using namespace pt_operation;
	using namespace util;


	CLASS_DEFINITION(DeadRun, "t080_dead_runs", 80)
	FIELD_DEFINITION_OF_OBJECT(DeadRun, "dead_run_id", "dead_run_ids")

	FIELD_DEFINITION_OF_TYPE(DeadRunNetwork, "network_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DepotId, "depot_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(StopId, "stop_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(pt_operation::Direction, "direction", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(Length, "length", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(DeadRunDataSource, "datasource_links", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DeadRunOperationUnit, "operation_unit_id", SQL_INTEGER)

	namespace pt_operation
	{
		const string DeadRun::ATTR_DEP_PLACE_NAME = "departure_place_name";
		const string DeadRun::ATTR_ARR_PLACE_NAME = "arrival_place_name";
		const string DeadRun::ATTR_DEP_SCHEDULE   = "departure_schedule";
		const string DeadRun::ATTR_ARR_SCHEDULE   = "arrival_schedule";

		DeadRun::DeadRun(
			RegistryKeyType id,
			string number
		):	Registrable(id),
			Object<DeadRun, DeadRunSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(DeadRunNetwork),
					FIELD_DEFAULT_CONSTRUCTOR(DepotId),
					FIELD_DEFAULT_CONSTRUCTOR(StopId),
					FIELD_DEFAULT_CONSTRUCTOR(Direction),
					FIELD_DEFAULT_CONSTRUCTOR(ServiceSchedules),
					FIELD_DEFAULT_CONSTRUCTOR(ServiceDates),
					FIELD_VALUE_CONSTRUCTOR(ServiceNumber, number),
					FIELD_DEFAULT_CONSTRUCTOR(Length),
					FIELD_DEFAULT_CONSTRUCTOR(DeadRunDataSource),
					FIELD_DEFAULT_CONSTRUCTOR(DeadRunOperationUnit)
			)	),
			SchedulesBasedService(number, NULL)
		{
			setPath(this);
			_vertices.push_back(NULL);
			_vertices.push_back(NULL);

			// Default use rules
			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			Path::setRules(rules);

			// Service auto registration
			boost::shared_ptr<ChronologicalServicesCollection> collection(new ChronologicalServicesCollection);
			_serviceCollections.push_back(collection);
			collection->getServices().insert(this);
		}



		DeadRun::~DeadRun()
		{
			setUndefined();
		}



		void DeadRun::setRoute(
			Depot& depot,
			pt::StopPoint& stop,
			double length,
			bool fromDepotToStop
		){
			// Cleaning existing data
			if(!_edges.empty())
			{
				setUndefined();
			}

			// Generation of edges
			Edge* startEdge(
				fromDepotToStop ?
				new DeadRunEdge(*this, depot) :
				new DeadRunEdge(*this, stop)
			);
			addEdge(*startEdge);

			Edge* endEdge(
				fromDepotToStop ?
				new DeadRunEdge(length, *this, stop) :
				new DeadRunEdge(length, *this, depot)
			);
			addEdge(*endEdge);

			set<DepotId>(depot.getKey());
			set<StopId>(stop.getKey());
			set<Length>(length);
			set<Direction>(fromDepotToStop);
		}



		void DeadRun::setUndefined()
		{
			// Edges
			for (Path::Edges::iterator it(_edges.begin()); it != _edges.end(); ++it)
			{
				delete *it;
			}
			_edges.clear();
		}



		bool DeadRun::isUndefined() const
		{
			return _edges.size() != 2;
		}



		bool DeadRun::getFromDepotToStop() const
		{
			if(isUndefined())
			{
				return true;
			}

			return dynamic_cast<Depot*>(_edges[0]->getFromVertex()) != NULL;
		}



		bool DeadRun::isContinuous() const
		{
			return false;
		}



		boost::posix_time::time_duration DeadRun::getDepartureBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			recursive_mutex::scoped_lock lock(getSchedulesMutex());
			return getDepartureSchedules(true, RTData)[rankInPath];
		}



		boost::posix_time::time_duration DeadRun::getDepartureEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			recursive_mutex::scoped_lock lock(getSchedulesMutex());
			return getDepartureSchedules(true, RTData)[rankInPath];
		}



		boost::posix_time::time_duration DeadRun::getArrivalBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			recursive_mutex::scoped_lock lock(getSchedulesMutex());
			return getArrivalSchedules(true, RTData)[rankInPath];
		}



		boost::posix_time::time_duration DeadRun::getArrivalEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			recursive_mutex::scoped_lock lock(getSchedulesMutex());
			return getArrivalSchedules(true, RTData)[rankInPath];
		}



		graph::ServicePointer DeadRun::getFromPresenceTime(const AccessParameters&, bool, bool,bool, const synthese::graph::Edge &,const boost::posix_time::ptime &,bool,bool,bool,bool,graph::UseRule::ReservationDelayType) const
		{
			return ServicePointer();
		}



		void DeadRun::completeServicePointer(synthese::graph::ServicePointer &,const synthese::graph::Edge &,const synthese::graph::AccessParameters &) const
		{
		}


		bool DeadRun::isPedestrianMode(void) const
		{
			return false;
		}



		bool DeadRun::isActive(const boost::gregorian::date& date) const
		{
			return Calendar::isActive(date);
		}



		std::string DeadRun::getRuleUserName() const
		{
			return "Haut-le-pied" + getServiceNumber();
		}



		Depot* DeadRun::getDepot() const
		{
			if(isUndefined())
			{
				return NULL;
			}

			return static_cast<Depot*>(getEdge(getFromDepotToStop() ? 0 : 1)->getFromVertex());
		}



		pt::StopPoint* DeadRun::getStop() const
		{
			if(isUndefined())
			{
				return NULL;
			}

			return static_cast<StopPoint*>(getEdge(getFromDepotToStop() ? 1 : 0)->getFromVertex());
		}



		void DeadRun::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles /*= boost::logic::indeterminate*/,
			std::string prefix /*= std::string() */
		) const	{
			pm.insert(prefix + TABLE_COL_ID, getKey());
			
			// Network
			pm.insert(
				prefix + DeadRunNetwork::FIELD.name,
				getTransportNetwork() ? getTransportNetwork()->getKey() : RegistryKeyType(0)
			);

			// Depot
			Depot* depot(getDepot());
			pm.insert(
				prefix + DepotId::FIELD.name,
				depot ? depot->getKey() : RegistryKeyType(0)
			);

			// Stop
			StopPoint* stop(getStop());
			pm.insert(
				prefix + StopId::FIELD.name,
				stop ? stop->getKey() : RegistryKeyType(0)
			);

			// Direction
			pm.insert(
				prefix + Direction::FIELD.name,
				getFromDepotToStop()
			);

			// Schedules
			pm.insert(
				prefix + ServiceSchedules::FIELD.name,
				encodeSchedules()
			);

			// Dates
			stringstream datesStr;
			serialize(datesStr);
			pm.insert(
				prefix + ServiceDates::FIELD.name,
				datesStr.str()
			);

			// Service number
			pm.insert(
				prefix + ServiceNumber::FIELD.name,
				getServiceNumber()
			);

			// Length
			pm.insert(
				prefix + Length::FIELD.name,
				isUndefined() ? 0 : getEdge(1)->getMetricOffset()
			);

			// Data source links
			pm.insert(
				prefix + DeadRunDataSource::FIELD.name,
				synthese::impex::DataSourceLinks::Serialize(
					getDataSourceLinks()
			)	);

			// Operation unit
			pm.insert(
				prefix + DeadRunOperationUnit::FIELD.name,
				getOperationUnit() ? getOperationUnit()->getKey() : RegistryKeyType(0)
			);

			// Departure and Arrival place names
			if (getFromDepotToStop()) {
				pm.insert(
					ATTR_DEP_PLACE_NAME,
					depot ? depot->getName() : ""
				);
				pm.insert(
					ATTR_ARR_PLACE_NAME,
					stop ? stop->getName() : ""
				);
			}
			else {
				pm.insert(
					ATTR_DEP_PLACE_NAME,
					stop ? stop->getName() : ""
				);
				pm.insert(
					ATTR_ARR_PLACE_NAME,
					depot ? depot->getName() : ""
				);
			}

			// Departure and Arrival schedules
			stringstream departure_schedule;
			stringstream arrival_schedule;
			if (getDataDepartureSchedules().size() == 2)
			{
				departure_schedule << EncodeSchedule(getDataDepartureSchedules()[0]);
				arrival_schedule << EncodeSchedule(getDataArrivalSchedules()[1]);
			}
			
			pm.insert(
				ATTR_DEP_SCHEDULE,
				departure_schedule.str()
			);
			pm.insert(
				ATTR_ARR_SCHEDULE,
				arrival_schedule.str()
			);
		}



		void DeadRun::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			// Service number
			Service::setServiceNumber(get<ServiceNumber>());

			// Depot, stop point, direction, length
			Depot* depot(NULL);
			StopPoint* stop(NULL);
			RegistryKeyType pid(get<DepotId>());
			RegistryKeyType stopId(get<StopId>());
			MetricOffset length(get<Length>());
			bool dir(get<Direction>());
			try
			{
				if(pid > 0)
				{
					depot = env.getEditable<Depot>(pid).get();
				}
				if(stopId > 0)
				{
					stop = env.getEditable<StopPoint>(stopId).get();
				}
			}
			catch(ObjectNotFoundException<Depot>&)
			{
				Log::GetInstance().warn("No such depot "+ lexical_cast<string>(pid) +" in Dead run "+ lexical_cast<string>(getKey()));
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				Log::GetInstance().warn("No such stop "+ lexical_cast<string>(stopId) +" in Dead run "+ lexical_cast<string>(getKey()));
			}
			if(depot &&	stop)
			{
				// Generation of edges
				Edge* startEdge(
					dir ?
					new DeadRunEdge(*this, *depot) :
					new DeadRunEdge(*this, *stop)
				);
				addEdge(*startEdge);

				Edge* endEdge(
					dir ?
					new DeadRunEdge(length, *this, *stop) :
					new DeadRunEdge(length, *this, *depot)
				);
				addEdge(*endEdge);
			}

			// Schedules
			if (!get<ServiceSchedules>().empty())
			{
				SchedulesBasedService::SchedulesPair value(
					SchedulesBasedService::DecodeSchedules(
						get<ServiceSchedules>()
				)	);
				setDataSchedules(value.first, value.second);
			}

			// Dates
			if (!get<ServiceDates>().empty())
			{
				setFromSerializedString(get<ServiceDates>());
			}
		}



		void DeadRun::unlink()
		{
			// TODO Data source links should be removed from the data source
		}



		synthese::LinkedObjectsIds DeadRun::getLinkedObjectsIds( const Record& record ) const
		{
			LinkedObjectsIds result;

			LinkedObjectsIds subResult;
			subResult = Object::getLinkedObjectsIds(record);
			BOOST_FOREACH(RegistryKeyType linkedId, subResult)
			{
				result.push_back(linkedId);
			}

			// Depot
			{
				RegistryKeyType id(
					get<DepotId>()
				);
				if(id > 0)
				{
					result.push_back(id);
				}
			}

			// Stop
			{
				RegistryKeyType id(
					get<StopId>()
				);
				if(id > 0)
				{
					result.push_back(id);
				}
			}

			return result;
		}

		void DeadRun::setServiceNumber(std::string serviceNumber)
		{
			Service::setServiceNumber(serviceNumber);
			set<ServiceNumber>(serviceNumber);
		}

		void DeadRun::setTransportNetwork(pt::TransportNetwork* value)
		{
			set<DeadRunNetwork>(value
				? boost::optional<pt::TransportNetwork&>(*value)
				: boost::none);
		}

		pt::TransportNetwork* DeadRun::getTransportNetwork() const
		{
			return get<DeadRunNetwork>() ? get<DeadRunNetwork>().get_ptr() : NULL;
		}

		void DeadRun::setOperationUnit(const boost::optional<OperationUnit&>& value)
		{
			set<DeadRunOperationUnit>(value
				? boost::optional<OperationUnit&>(*value)
				: boost::none);
		}

		const boost::optional<OperationUnit&> DeadRun::getOperationUnit() const
		{
			boost::optional<OperationUnit&> value = boost::optional<OperationUnit&>();
			if (get<DeadRunOperationUnit>())
			{
				value = get<DeadRunOperationUnit>().get();
			}
			return value;
		}

		bool DeadRun::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool DeadRun::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool DeadRun::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
