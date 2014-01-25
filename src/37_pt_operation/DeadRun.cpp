
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
#include "DeadRunTableSync.hpp"
#include "DeadRunEdge.hpp"
#include "Depot.hpp"
#include "ImportableTableSync.hpp"
#include "OperationUnit.hpp"
#include "StopPoint.hpp"
#include "TransportNetwork.h"
#include "ForbiddenUseRule.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace graph;
	using namespace impex;
	

	namespace util
	{
		template<>
		const std::string Registry<pt_operation::DeadRun>::KEY("DeadRun");
	}

	namespace pt_operation
	{
		DeadRun::DeadRun(
			RegistryKeyType id,
			string number
		):	Registrable(id),
			SchedulesBasedService(number, NULL),
			_network(NULL)
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
			return getDepartureSchedules(true, RTData)[rankInPath];
		}



		boost::posix_time::time_duration DeadRun::getDepartureEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getDepartureSchedules(true, RTData)[rankInPath];
		}



		boost::posix_time::time_duration DeadRun::getArrivalBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getArrivalSchedules(true, RTData)[rankInPath];
		}



		boost::posix_time::time_duration DeadRun::getArrivalEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getArrivalSchedules(true, RTData)[rankInPath];
		}



		graph::ServicePointer DeadRun::getFromPresenceTime(const AccessParameters&, bool, bool,bool, const synthese::graph::Edge &,const boost::posix_time::ptime &,bool,bool,bool,bool) const
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
				prefix + DeadRunTableSync::COL_NETWORK_ID,
				getTransportNetwork() ? getTransportNetwork()->getKey() : RegistryKeyType(0)
			);

			// Depot
			Depot* depot(getDepot());
			pm.insert(
				prefix + DeadRunTableSync::COL_DEPOT_ID,
				depot ? depot->getKey() : RegistryKeyType(0)
			);

			// Stop
			StopPoint* stop(getStop());
			pm.insert(
				prefix + DeadRunTableSync::COL_STOP_ID,
				stop ? stop->getKey() : RegistryKeyType(0)
			);

			// Direction
			pm.insert(
				prefix + DeadRunTableSync::COL_DIRECTION,
				getFromDepotToStop()
			);

			// Schedules
			pm.insert(
				prefix + DeadRunTableSync::COL_SCHEDULES,
				encodeSchedules()
			);

			// Dates
			stringstream datesStr;
			serialize(datesStr);
			pm.insert(
				prefix + DeadRunTableSync::COL_DATES,
				datesStr.str()
			);

			// Service number
			pm.insert(
				prefix + DeadRunTableSync::COL_SERVICE_NUMBER,
				getServiceNumber()
			);

			// Length
			pm.insert(
				prefix + DeadRunTableSync::COL_LENGTH,
				isUndefined() ? 0 : getEdge(1)->getMetricOffset()
			);

			// Data source links
			pm.insert(
				prefix + DeadRunTableSync::COL_DATASOURCE_LINKS,
				synthese::impex::DataSourceLinks::Serialize(
					getDataSourceLinks()
			)	);

			// Operation unit
			pm.insert(
				prefix + DeadRunTableSync::COL_OPERATION_UNIT_ID,
				getOperationUnit() ? getOperationUnit()->getKey() : RegistryKeyType(0)
			);
		}



		bool DeadRun::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			// Service number
			{
				string value(
					record.getDefault<string>(DeadRunTableSync::COL_SERVICE_NUMBER)
				);
				if(value != getServiceNumber())
				{
					result = true;
					setServiceNumber(value);
				}
			}

			// Network
			{
				TransportNetwork* value(NULL);
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(DeadRunTableSync::COL_NETWORK_ID, 0)
				);
				if(id > 0) try
				{
					value = env.getEditable<TransportNetwork>(id).get();
				}
				catch(ObjectNotFoundException<TransportNetwork>&)
				{
					Log::GetInstance().warn("No such network "+ lexical_cast<string>(id) +" in Dead run "+ lexical_cast<string>(getKey()));
				}
				if(value != getTransportNetwork())
				{
					result = true;
					setTransportNetwork(value);
				}
			}
				
			// Operation unit
			{
				optional<OperationUnit&> value;
				RegistryKeyType unitId(record.getDefault<RegistryKeyType>(DeadRunTableSync::COL_OPERATION_UNIT_ID, 0));
				if(unitId) try
				{
					value = *env.getEditable<OperationUnit>(unitId);
				}
				catch(ObjectNotFoundException<OperationUnit>&)
				{
					Log::GetInstance().warn("No such operation unit "+ lexical_cast<string>(unitId) +" in dead run "+ lexical_cast<string>(getKey()));
				}

				if(	(value && !getOperationUnit()) ||
					(!value && getOperationUnit()) ||
					(value && getOperationUnit() && &*value != &*getOperationUnit())
				){
					result = true;
					setOperationUnit(value);
				}
			}



			// Depot, stop point, direction, length
			{
				Depot* depot(NULL);
				StopPoint* stop(NULL);
				RegistryKeyType pid(record.getDefault<RegistryKeyType>(DeadRunTableSync::COL_DEPOT_ID, 0));
				RegistryKeyType stopId(record.getDefault<RegistryKeyType>(DeadRunTableSync::COL_STOP_ID, 0));
				MetricOffset length(record.getDefault(DeadRunTableSync::COL_LENGTH, 0));
				bool dir(record.getDefault<bool>(DeadRunTableSync::COL_DIRECTION, false));
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
				if(	depot != getDepot() ||
					stop != getStop() ||
					length != (isUndefined() ? 0 : getEdge(1)->getMetricOffset()) ||
					dir != getFromDepotToStop()
				){
					result = true;
					if(depot &&	stop)
					{
						setRoute(*depot, *stop, length, dir);
					}
					else
					{
						setUndefined();
					}
				}
			}

			// Data source links
			{
				Importable::DataSourceLinks value(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						record.getDefault<string>(DeadRunTableSync::COL_DATASOURCE_LINKS),
						env
				)	);
				if(value != getDataSourceLinks())
				{
					result = true;
					setDataSourceLinksWithRegistration(value);
				}
			}

			// Schedules
			{
				SchedulesBasedService::SchedulesPair value(
					SchedulesBasedService::DecodeSchedules(
						record.getDefault<string>(DeadRunTableSync::COL_SCHEDULES)
				)	);
				if(	value.first != getDataDepartureSchedules() ||
					value.second != getDataArrivalSchedules()
				){
					result = true;
					setDataSchedules(value.first, value.second);
				}
			}

			// Dates
			{
				Calendar value(
					record.getDefault<string>(DeadRunTableSync::COL_DATES)
				);
				if(value != *this)
				{
					result = true;
					copyDates(value);
				}
			}

			return result;
		}



		void DeadRun::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{

		}



		void DeadRun::unlink()
		{
			// TODO Data source links should be removed from the data source
		}



		synthese::LinkedObjectsIds DeadRun::getLinkedObjectsIds( const Record& record ) const
		{
			LinkedObjectsIds result;

			// Network
			{
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(DeadRunTableSync::COL_NETWORK_ID, 0)
				);
				if(id > 0)
				{
					result.push_back(id);
				}
			}

			// Operation unit
			{
				RegistryKeyType unitId(
					record.getDefault<RegistryKeyType>(DeadRunTableSync::COL_OPERATION_UNIT_ID, 0)
				);
				if(unitId > 0)
				{
					result.push_back(unitId);
				}
			}

			// Depot
			{
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(DeadRunTableSync::COL_DEPOT_ID, 0)
				);
				if(id > 0)
				{
					result.push_back(id);
				}
			}

			// Stop
			{
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(DeadRunTableSync::COL_STOP_ID, 0)
				);
				if(id > 0)
				{
					result.push_back(id);
				}
			}
			
			// Data source links
			impex::DataSourceLinks::GetLinkedObjectsIdsFromText(
				result,
				record.getDefault<string>(DeadRunTableSync::COL_DATASOURCE_LINKS)
			);

			return result;
		}
}	}
