
/** IneoRealTimeFileFormat class implementation.
	@file IneoRealTimeFileFormat.cpp

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

#include "IneoRealTimeFileFormat.hpp"

#include "Import.hpp"
#include "DataSource.h"
#include "DataSourceTableSync.h"
#include "DBTransaction.hpp"
#include "ImportableTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "StopPointTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineStopTableSync.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace gregorian;

namespace synthese
{
	using namespace data_exchange;
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace impex;
	using namespace db;
	using namespace graph;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, IneoRealTimeFileFormat>::FACTORY_KEY("ineo_temps_reel");
	}

	namespace data_exchange
	{
		const string IneoRealTimeFileFormat::Importer_::PARAMETER_PLANNED_DATASOURCE_ID("ps");
		const string IneoRealTimeFileFormat::Importer_::PARAMETER_COURSE_ID("ci");
		const string IneoRealTimeFileFormat::Importer_::PARAMETER_DB_CONN_STRING("conn_string");
		const string IneoRealTimeFileFormat::Importer_::PARAMETER_STOP_CODE_PREFIX("stop_code_prefix");



		bool IneoRealTimeFileFormat::Importer_::_read(
		) const	{
			if(_database.empty() || !_plannedDataSource.get())
			{
				return false;
			}

			DataSource& dataSource(*_import.get<DataSource>());

			boost::shared_ptr<DB> db;

			if(_dbConnString)
			{
				db = DBModule::GetDBForStandaloneUse(*_dbConnString);
			}
			else
			{
				db = DBModule::GetDBSPtr();
			}

			date today(day_clock::local_day());
			string todayStr("'"+ to_iso_extended_string(today) +"'");

			// Services linked to the planned source
			ImportableTableSync::ObjectBySource<StopPointTableSync> stops(*_plannedDataSource, _env);
			ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(*_plannedDataSource, _env);

			if(!_courseId)
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<CommercialLineTableSync>::Map::value_type& itLine, lines.getMap())
				{
					BOOST_FOREACH(const ImportableTableSync::ObjectBySource<CommercialLineTableSync>::Map::mapped_type::value_type& line, itLine.second)
					{
						JourneyPatternTableSync::Search(_env, line->getKey());
						ScheduledServiceTableSync::Search(_env, optional<RegistryKeyType>(), line->getKey());
						BOOST_FOREACH(const Path* route, line->getPaths())
						{
							LineStopTableSync::Search(_env, route->getKey());
						}
				}	}

				// 1 : clean the old references to the current source
				ImportableTableSync::ObjectBySource<ScheduledServiceTableSync> sourcedServices(dataSource, _env);
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<ScheduledServiceTableSync>::Map::value_type& itService, sourcedServices.getMap())
				{
					BOOST_FOREACH(const ImportableTableSync::ObjectBySource<ScheduledServiceTableSync>::Map::mapped_type::value_type& obj, itService.second)
					{
						obj->removeSourceLinks(dataSource);
					}
				}
			}
			else
			{
				// 1 : clean the old references to the current source
				ImportableTableSync::ObjectBySource<ScheduledServiceTableSync> sourcedServices(dataSource, _env);
				set<ScheduledService*> services(sourcedServices.get(*_courseId));
				BOOST_FOREACH(ScheduledService* service, services)
				{
					service->removeSourceLinks(dataSource);
					_services.insert(service);
				}
			}

			// 2 : loop on the services present in the database and link to existing or new services
			stringstream query;
			query << "SELECT c.ref, c.chainage, c.ligne, l.mnemo as ligne_ref FROM " << _database << ".COURSE c " <<
				"INNER JOIN " << _database << ".LIGNE l on c.ligne=l.ref AND l.jour=c.jour " <<
				"WHERE c.jour=" << todayStr << " AND c.type='C'";
			if(_courseId)
			{
				query << " AND c.ref=" << *_courseId;
			}
			DBResultSPtr result(db->execQuery(query.str()));
			while(result->next())
			{
				string serviceRef(result->getText("ref"));
				string chainage(result->getText("chainage"));
				string ligneRef(result->getText("ligne_ref"));

				_logDebug(
					"Processing serviceRef="+ serviceRef +" chainage="+	chainage +" ligneRef="+ ligneRef
				);

				CommercialLine* line(
					_getLine(
						lines,
						ligneRef,
						*_plannedDataSource
				)	);

				if(!line)
				{
					_logWarning(
						"Line "+ ligneRef +" was not found for service "+ serviceRef
					);
					continue;
				}

				stringstream chainageQuery;
				chainageQuery << "SELECT a.mnemol AS mnemol, h.htd AS htd, h.hta AS hta, h.type AS type, c.pos AS pos FROM "
					<< _database << ".ARRETCHN c " <<
					"INNER JOIN " << _database << ".ARRET a ON a.ref=c.arret AND a.jour=c.jour " <<
					"INNER JOIN " << _database << ".HORAIRE h ON h.arretchn=c.ref AND h.jour=a.jour " <<
					"INNER JOIN " << _database << ".COURSE o ON o.chainage=c.chainage AND o.ref=h.course AND c.jour=o.jour " <<
					"WHERE h.course='" << serviceRef << "' AND h.jour=" << todayStr << " ORDER BY c.pos";
				DBResultSPtr chainageResult(db->execQuery(chainageQuery.str()));

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
						_getStopPoints(
							stops,
							_stopCodePrefix + stopCode,
							boost::optional<const std::string&>()
					)	);
					if(stopsSet.empty())
					{
						_logWarning(
							"Can't find stops for code "+ _stopCodePrefix + stopCode
						);
						continue;
					}

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
					_getRoutes(
						*line,
						servedStops,
						*_plannedDataSource
				)	);

				if(routes.empty())
				{
					stringstream routeQuery;
					routeQuery << "SELECT * FROM " << _database << ".CHAINAGE c " <<
						"WHERE c.ref='" << chainage << "' AND c.jour=" << todayStr;
					DBResultSPtr routeResult(db->execQuery(routeQuery.str()));
					if(routeResult->next())
					{
						string routeName(routeResult->getText("nom"));
						bool wayBack(routeResult->getText("sens") != "A");

						_logCreation(
							"Creation of route "+ routeName
						);

						JourneyPattern* result = new JourneyPattern(
							JourneyPatternTableSync::getId()
						);
						result->setCommercialLine(line);
						line->addPath(result);
						result->setName(routeName);
						result->setWayBack(wayBack);
						result->addCodeBySource(*_plannedDataSource, string());
						_env.getEditableRegistry<JourneyPattern>().add(boost::shared_ptr<JourneyPattern>(result));
						routes.insert(result);

						size_t rank(0);
						BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, servedStops)
						{
							boost::shared_ptr<LineStop> ls(
								new LineStop(
									LineStopTableSync::getId(),
									result,
									rank,
									rank+1 < servedStops.size() && stop._departure,
									rank > 0 && stop._arrival,
									*stop._metricOffset,
									**stop._stop.begin()
							)	);
							ls->set<ScheduleInput>(stop._withTimes ? *stop._withTimes : true);
							ls->link(_env, true);
							_env.getEditableRegistry<LineStop>().add(ls);
							++rank;
					}	}
				}
				assert(!routes.empty());

				ScheduledService* service(NULL);
				BOOST_FOREACH(JourneyPattern* route, routes)
				{
					boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
						*route->sharedServicesMutex
					);
					BOOST_FOREACH(Service* sservice, route->getAllServices())
					{
						service = dynamic_cast<ScheduledService*>(sservice);
						if(!service)
						{
							continue;
						}
						if(	service->isActive(today) &&
							service->comparePlannedSchedules(departureSchedules, arrivalSchedules)
						){
							_logLoad(
								"Use of service "+ lexical_cast<string>(service->getKey()) +" ("+ lexical_cast<string>(departureSchedules[0]) +") on route "+ lexical_cast<string>(route->getKey()) +" ("+ route->getName() +")"
							);
							service->addCodeBySource(dataSource, serviceRef);
							_services.insert(service);
							break;
						}
						service = NULL;
					}
					if(service)
					{
						break;
					}
				}

				if(!service)
				{
					if (!departureSchedules.empty() && !arrivalSchedules.empty())
					{
						JourneyPattern* route(*routes.begin());
						service = new ScheduledService(
							ScheduledServiceTableSync::getId(),
							string(),
							route
						);
						service->setDataSchedules(departureSchedules, arrivalSchedules);
						service->setPath(route);
						service->addCodeBySource(dataSource, serviceRef);
						service->setActive(today);
						route->addService(*service, false);
						_env.getEditableRegistry<ScheduledService>().add(boost::shared_ptr<ScheduledService>(service));
						_services.insert(service);

						_logCreation(
							"Creation of service ("+ lexical_cast<string>(departureSchedules[0]) +") on route "+ lexical_cast<string>(route->getKey()) +" ("+ route->getName() +")"
						);
					}
					else
					{
						_logWarning(
							"Service (ref="+ serviceRef +") has empty departure or arrival schedules, not creating"
						);
					}
				}
			}


			// 3 : loop on the planned services and remove current day of run if not linked to current source
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<CommercialLineTableSync>::Map::value_type& itLine, lines.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<CommercialLineTableSync>::Map::mapped_type::value_type& obj, itLine.second)
				{
					BOOST_FOREACH(Path* route, obj->getPaths())
					{
						// Avoid junctions
						if(!dynamic_cast<JourneyPattern*>(route))
						{
							continue;
						}

						JourneyPattern* jp(static_cast<JourneyPattern*>(route));

						if(!jp->hasLinkWithSource(*_plannedDataSource))
						{
							continue;
						}

						boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
									*jp->sharedServicesMutex
						);
						BOOST_FOREACH(const Service* service, jp->getAllServices())
						{
							const ScheduledService* sservice(dynamic_cast<const ScheduledService*>(service));
							if(	sservice &&
								sservice->isActive(today) &&
								!sservice->hasLinkWithSource(dataSource)
							){
								const_cast<ScheduledService*>(sservice)->setInactive(today);
								_logInfo(
									"Deactivating unlinked service "+ lexical_cast<string>(sservice->getKey()) +
										" on route "+ lexical_cast<string>(sservice->getRoute()->getKey()) +" (" +
										sservice->getRoute()->getName() +")"
								);
							}
						}
					}
			}	}
			return true;
		}



		IneoRealTimeFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			DatabaseReadImporter<IneoRealTimeFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		util::ParametersMap IneoRealTimeFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;
			if(_plannedDataSource.get())
			{
				map.insert(PARAMETER_PLANNED_DATASOURCE_ID, _plannedDataSource->getKey());
			}
			if(_courseId)
			{
				map.insert(PARAMETER_COURSE_ID, *_courseId);
			}
			if(_dbConnString)
			{
				map.insert(PARAMETER_DB_CONN_STRING, *_dbConnString);
			}
			if(!_stopCodePrefix.empty())
			{
				map.insert(PARAMETER_STOP_CODE_PREFIX, _stopCodePrefix);
			}
			return map;
		}



		void IneoRealTimeFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			if(map.isDefined(PARAMETER_PLANNED_DATASOURCE_ID)) try
			{
				_plannedDataSource = DataSourceTableSync::Get(map.get<RegistryKeyType>(PARAMETER_PLANNED_DATASOURCE_ID), _env);
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw Exception("No such planned data source");
			}
			_courseId = map.getOptional<string>(PARAMETER_COURSE_ID);
			_dbConnString = map.getOptional<string>(PARAMETER_DB_CONN_STRING);
			_stopCodePrefix = map.getDefault<string>(PARAMETER_STOP_CODE_PREFIX, "");
		}



		db::DBTransaction IneoRealTimeFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			if(_courseId)
			{
				BOOST_FOREACH(ScheduledService* service, _services)
				{
					JourneyPatternTableSync::Save(static_cast<JourneyPattern*>(service->getPath()), transaction);
					BOOST_FOREACH(LineStop* edge, static_cast<JourneyPattern*>(service->getPath())->getLineStops())
					{
						LineStopTableSync::Save(edge, transaction);
					}
					ScheduledServiceTableSync::Save(service, transaction);
			}	}
			else
			{
				BOOST_FOREACH(const Registry<JourneyPattern>::value_type& journeyPattern, _env.getRegistry<JourneyPattern>())
				{
					JourneyPatternTableSync::Save(journeyPattern.second.get(), transaction);
				}
				BOOST_FOREACH(Registry<LineStop>::value_type lineStop, _env.getRegistry<LineStop>())
				{
					LineStopTableSync::Save(lineStop.second.get(), transaction);
				}
				BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
				{
					ScheduledServiceTableSync::Save(service.second.get(), transaction);
				}
			}
			return transaction;
		}
	}
}
