
/** DeviateServiceAction class implementation.
	@file DeviateServiceAction.cpp
	@author Hugues
	@date 2009

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

#include "ActionException.h"
#include "ParametersMap.h"
#include "DeviateServiceAction.hpp"
#include "Request.h"
#include "ScheduledService.h"
#include "StopPoint.hpp"
#include "Env.h"
#include "JourneyPattern.hpp"
#include "LineStop.h"
#include "DataSource.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::DeviateServiceAction>::FACTORY_KEY("DeviateService");
	}

	namespace pt
	{
		const string DeviateServiceAction::PARAMETER_DATASOURCE_ID = Action_PARAMETER_PREFIX + "datasource_id";
		const string DeviateServiceAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "service_id";
		const string DeviateServiceAction::PARAMETER_CHAINAGE = Action_PARAMETER_PREFIX + "chainage";
		const string DeviateServiceAction::PARAMETER_DATABASE = Action_PARAMETER_PREFIX + "database";



		DeviateServiceAction::DeviateServiceAction():
			_chainage(0)
		{}



		ParametersMap DeviateServiceAction::getParametersMap() const
		{
			ParametersMap map;
			
			// Service
			if(_service.get())
			{
				if(_dataSource.get())
				{
					map.insert(PARAMETER_DATASOURCE_ID, _dataSource->getKey());
					map.insert(PARAMETER_SERVICE_ID, _service->getCodeBySource(*_dataSource));
				}
				else
				{
					map.insert(PARAMETER_SERVICE_ID, _service->getKey());
				}
			}

			// Chainage
			map.insert(PARAMETER_CHAINAGE, _chainage);

			// Database
			map.insert(PARAMETER_DATABASE, _database);

			return map;
		}



		void DeviateServiceAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Load of the service
			if(map.getDefault<RegistryKeyType>(PARAMETER_DATASOURCE_ID, 0))
			{
				try
				{
					shared_ptr<const DataSource> dataSource(
						Env::GetOfficialEnv().getRegistry<DataSource>().get(map.get<RegistryKeyType>(PARAMETER_DATASOURCE_ID))
					);
					Importable* obj(dataSource->getObjectByCode(map.get<string>(PARAMETER_SERVICE_ID)));
					if(!obj || !dynamic_cast<ScheduledService*>(obj))
					{
						throw ActionException("No such service");
					}
					_service = Env::GetOfficialEnv().getEditableSPtr(static_cast<ScheduledService*>(obj));
				}
				catch(ObjectNotFoundException<DataSource>&)
				{
					throw ActionException("No such datasource");
				}
			}
			else try
			{
				_service = Env::GetOfficialEnv().getEditableRegistry<ScheduledService>().getEditable(
					map.get<RegistryKeyType>(PARAMETER_SERVICE_ID)
				);
			}
			catch(ObjectNotFoundException<ScheduledService>)
			{
				throw ActionException("No such service");
			}

			// chainage
			_chainage = map.getDefault<size_t>(PARAMETER_CHAINAGE, 0);

			// Database
			_database = map.get<string>(PARAMETER_DATABASE);
			if(_database.empty())
			{
				throw ActionException("Database must be non empty");
			}
		}



		void DeviateServiceAction::run(Request& request)
		{
			//1. Load of the new chainage
/*			stringstream chainageQuery;
			chainageQuery << "SELECT a.mnemol AS mnemol, h.htd AS htd, h.hta AS hta, h.type AS type, c.pos AS pos FROM "
				<< _database << ".ARRETCHN c " <<
				"INNER JOIN " << _database << ".ARRET a ON a.ref=c.arret AND a.jour=c.jour " <<
				"INNER JOIN " << _database << ".HORAIRE h ON h.arretchn=c.ref AND h.jour=a.jour " <<
				"INNER JOIN " << _database << ".COURSE o ON o.chainage=c.chainage AND o.ref=h.course AND c.jour=o.jour " <<
				"WHERE h.course='" << serviceRef << "' AND h.jour=" << todayStr << " ORDER BY c.pos";
			DBResultSPtr chainageResult(DBModule::GetDB()->execQuery(chainageQuery.str()));

			JourneyPattern::StopsWithDepartureArrivalAuthorization servedStops;
			SchedulesBasedService::Schedules departureSchedules;
			SchedulesBasedService::Schedules arrivalSchedules;

			while(chainageResult->next())
			{
				string type(chainageResult->getText("type"));
				string stopCode(chainageResult->getText("mnemol"));
				time_duration departureTime(duration_from_string(chainageResult->getText("htd")));
				time_duration arrivalTime(duration_from_string(chainageResult->getText("hta")));
				MetricOffset stopPos(chainageResult->getInt("pos"));
				bool referenceStop(type != "N");

				std::set<StopPoint*> stopsSet(
					PTFileFormat::GetStopPoints(
						stops,
						stopCode,
						boost::optional<const std::string&>(),
						os
				)	);

				servedStops.push_back(
					JourneyPattern::StopWithDepartureArrivalAuthorization(
						stopsSet,
						stopPos,
						(type != "A"),
						(type != "D"),
						referenceStop
				)	);

				// Ignoring interpolated times
				if(referenceStop)
				{
					// If the bus leaves after midnight, the hours are stored as 0 instead of 24
					if( !departureSchedules.empty() && departureTime < *departureSchedules.rbegin())
					{
						departureTime += hours(24);
					}
					if( !arrivalSchedules.empty() && arrivalTime < *arrivalSchedules.rbegin())
					{
						arrivalTime += hours(24);
					}

					// round of the seconds
					departureTime -= seconds(departureTime.seconds());
					if(arrivalTime.seconds())
					{
						arrivalTime += seconds(60 - arrivalTime.seconds());
					}

					// storage of the times
					departureSchedules.push_back(departureTime);
					arrivalSchedules.push_back(arrivalTime);
				}
			}

			set<JourneyPattern*> routes(
				PTFileFormat::GetRoutes(
					*line,
					servedStops,
					*_plannedDataSource
			)	);

			if(routes.empty())
			{
				stringstream routeQuery;
				routeQuery << "SELECT * FROM " << _database << ".CHAINAGE c " <<
					"WHERE c.ref='" << chainage << "' AND c.jour=" << todayStr;
				DBResultSPtr routeResult(DBModule::GetDB()->execQuery(routeQuery.str()));
				if(routeResult->next())
				{
					string routeName(routeResult->getText("nom"));
					bool wayBack(routeResult->getText("sens") != "A");

					os << "CREA : Creation of route<br />";

					JourneyPattern* result = new JourneyPattern(
						JourneyPatternTableSync::getId()
					);
					result->setCommercialLine(line);
					line->addPath(result);
					result->setName(routeName);
					result->setWayBack(wayBack);
					result->setCodeBySource(*_plannedDataSource, string());
					_env.getEditableRegistry<JourneyPattern>().add(shared_ptr<JourneyPattern>(result));
					routes.insert(result);

					size_t rank(0);
					BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, servedStops)
					{
						shared_ptr<DesignatedLinePhysicalStop> ls(
							new DesignatedLinePhysicalStop(
								LineStopTableSync::getId(),
								result,
								rank,
								rank+1 < servedStops.size() && stop._departure,
								rank > 0 && stop._arrival,
								0,
								*stop._stop.begin(),
								stop._withTimes ? *stop._withTimes : true
						)	);
						result->addEdge(*ls);
						_env.getEditableRegistry<DesignatedLinePhysicalStop>().add(ls);
						++rank;
				}	}
			}
			assert(!routes.empty());

*/

			//2. Search one or more existing journey pattern(s) corresponding to the chainage or creation



			//3. Search an existing service in the found journey pattern(s)
		}



		bool DeviateServiceAction::isAuthorized(const Session* session
		) const {
			return true;
		}
}	}
