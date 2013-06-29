
/** PTDataCleanerFileFormat class implementation.
	@file PTDataCleanerFileFormat.cpp

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

#include "PTDataCleanerFileFormat.hpp"

#include "CalendarTemplateTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "DBTransaction.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "DRTAreaTableSync.hpp"
#include "Import.hpp"
#include "ImportableTableSync.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineArea.hpp"
#include "LineStopTableSync.h"
#include "RequestException.h"
#include "ScheduledServiceTableSync.h"
#include "StopPointTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "VehicleServiceTableSync.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace calendar;
	using namespace db;
	using namespace impex;
	using namespace graph;
	using namespace pt;
	using namespace pt_operation;
	using namespace server;
	using namespace util;

	namespace data_exchange
	{
		const string PTDataCleanerFileFormat::PARAMETER_CALENDAR_ID("calendar_id");
		const string PTDataCleanerFileFormat::PARAMETER_CLEAN_OLD_DATA("clean_old_data");
		const string PTDataCleanerFileFormat::PARAMETER_CLEAN_UNUSED_STOPS("clean_unused_stops");
		const string PTDataCleanerFileFormat::PARAMETER_END_DATE("end_date");
		const string PTDataCleanerFileFormat::PARAMETER_FROM_TODAY("from_today");
		const string PTDataCleanerFileFormat::PARAMETER_START_DATE("start_date");
		const string PTDataCleanerFileFormat::PARAMETER_AUTO_PURGE = "auto_purge";



		PTDataCleanerFileFormat::PTDataCleanerFileFormat(
			util::Env& env,
			const Import& import,
			ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			_fromToday(false),
			_cleanOldData(true),
			_cleanUnusedStops(false),
			_autoPurge(false)
		{}



		void PTDataCleanerFileFormat::_cleanCalendars() const
		{
			if(!_cleanOldData)
			{
				return;
			}

			date now(gregorian::day_clock::local_day());
			DataSource& dataSource(*_import.get<DataSource>());

			// PT data
			ImportableTableSync::ObjectBySource<JourneyPatternTableSync> journeyPatterns(dataSource, _env);
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::value_type& itPathSet, journeyPatterns.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::mapped_type::value_type& itPath, itPathSet.second)
				{
					ScheduledServiceTableSync::Search(_env, itPath->getKey());
					ContinuousServiceTableSync::Search(_env, itPath->getKey());
					boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
						*itPath->sharedServicesMutex
					);

					BOOST_FOREACH(const ServiceSet::value_type& itService, itPath->getServices())
					{
						if(!dynamic_cast<NonPermanentService*>(itService))
						{
							continue;
						}

						NonPermanentService* service(static_cast<NonPermanentService*>(itService));

						// Avoid call of Calendar constructor with undefined end date for services without dates
						if(service->empty())
						{
							continue;
						}

						if(_fromToday)
						{
							if(now <= service->getLastActiveDate())
							{
								Calendar dates(now, service->getLastActiveDate());
								*service -= dates;
							}
						}
						else
						{
							*service -= _calendar;
						}

						// If auto purge : delete all days before today
						if(_autoPurge && service->getFirstActiveDate() < now)
						{
							date yesterday(now);
							yesterday -= days(1);
							Calendar dates(service->getFirstActiveDate(), yesterday);
							*service -= dates;
						}
					}
				}
			}


			//////////////////////////////////////////////////////////////////////////
			// PT Operation

			// Vehicle services
			ImportableTableSync::ObjectBySource<VehicleServiceTableSync> vehicleServices(dataSource, _env);
			VehicleService::DriverServiceChunks emptyVSChunks;
			VehicleService::Services emptyVSServices;
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<VehicleServiceTableSync>::Map::value_type& itVSSet, vehicleServices.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<VehicleServiceTableSync>::Map::mapped_type::value_type& itVS, itVSSet.second)
				{
					if(!itVS->hasAtLeastOneCommonDateWith(_calendar))
					{
						continue;
					}

					*itVS -= _calendar;
					itVS->setServices(emptyVSServices);
					itVS->setDriverServices(emptyVSChunks);
			}	}
		}



		void PTDataCleanerFileFormat::_selectObjectsToRemove() const
		{
			DataSource& dataSource(*_import.get<DataSource>());

			// Scheduled services without any active date
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& itService, _env.getRegistry<ScheduledService>())
			{
				if(itService.second->getRoute()->hasLinkWithSource(dataSource) && itService.second->empty())
				{
					_scheduledServicesToRemove.insert(itService.second);
					itService.second->getPath()->removeService(*itService.second);
				}
			}

			// Continuous services without any active date
			BOOST_FOREACH(const Registry<ContinuousService>::value_type& itCService, _env.getRegistry<ContinuousService>())
			{
				if(itCService.second->getRoute()->hasLinkWithSource(dataSource) && itCService.second->empty())
				{
					_continuousServicesToRemove.insert(itCService.second);
					itCService.second->getPath()->removeService(*itCService.second);
				}
			}

			// Journey patterns without any service
			BOOST_FOREACH(const Registry<JourneyPattern>::value_type& itJourneyPattern, _env.getRegistry<JourneyPattern>())
			{
				if(itJourneyPattern.second->hasLinkWithSource(dataSource) && itJourneyPattern.second->getServices().empty())
				{
					_journeyPatternsToRemove.insert(itJourneyPattern.second);
				}
			}

			// Scheduled services to delete are removed from the environment to avoid useless saving
			BOOST_FOREACH(const boost::shared_ptr<ScheduledService>& sservice, _scheduledServicesToRemove)
			{
				_env.getEditableRegistry<ScheduledService>().remove(sservice->getKey());
			}

			// Continuous services to delete are removed from the environment to avoid useless saving
			BOOST_FOREACH(const boost::shared_ptr<ContinuousService>& cservice, _continuousServicesToRemove)
			{
				_env.getEditableRegistry<ContinuousService>().remove(cservice->getKey());
			}

			// Journey patterns to delete are removed from the environment to avoid useless saving
			BOOST_FOREACH(const boost::shared_ptr<JourneyPattern>& journeyPattern, _journeyPatternsToRemove)
			{
				BOOST_FOREACH(const Edge* edge, journeyPattern->getEdges())
				{
					if(dynamic_cast<const DesignatedLinePhysicalStop*>(edge))
					{
						_edgesToRemove.insert(
							_env.getSPtr(dynamic_cast<const LineStop*>(edge))
						);
						_env.getEditableRegistry<LineStop>().remove(edge->getKey());
					}
					else if(dynamic_cast<const LineArea*>(edge))
					{
						_edgesToRemove.insert(
							_env.getSPtr(dynamic_cast<const LineStop*>(edge))
						);
						_env.getEditableRegistry<LineStop>().remove(edge->getKey());
					}
					if(static_cast<const LineStop*>(edge)->getIsArrival())
					{
						edge->getFromVertex()->removeArrivalEdge(edge);
					}
					if(static_cast<const LineStop*>(edge)->getIsDeparture())
					{
						edge->getFromVertex()->removeDepartureEdge(edge);
					}
				}
				_env.getEditableRegistry<JourneyPattern>().remove(journeyPattern->getKey());
			}

			// Vehicle services without services or without day
			BOOST_FOREACH(const Registry<VehicleService>::value_type& itVehicleService, _env.getRegistry<VehicleService>())
			{
				if(	itVehicleService.second->hasLinkWithSource(dataSource) &&
					(	itVehicleService.second->getServices().empty() ||
						itVehicleService.second->empty()
				)	){
					_vehicleServicesToRemove.insert(itVehicleService.second);
				}
			}

			// Vehicle services
			BOOST_FOREACH(const boost::shared_ptr<const VehicleService>& vehicleService, _vehicleServicesToRemove)
			{
				_env.getEditableRegistry<VehicleService>().remove(vehicleService->getKey());
			}

			if(_cleanUnusedStops)
			{
				// Stops without Journey patterns without any service
				Env checkEnv;
				boost::shared_ptr<const DataSource> dataSourceInCheckEnv(DataSourceTableSync::Get(dataSource.getKey(), _env));
				DRTAreaTableSync::Search(_env);
				BOOST_FOREACH(const Registry<StopPoint>::value_type& itStopPoint, _env.getRegistry<StopPoint>())
				{
					const StopPoint& stop(*itStopPoint.second);

					// Check if the stop should be removed according to the imported files
					if(	!stop.hasLinkWithSource(dataSource) ||
						stop.getDataSourceLinks().size() != 1 ||
						!stop.getDepartureEdges().empty() ||
						!stop.getArrivalEdges().empty()
					){
						continue;
					}

					// Check if the stop is not used by journey patterns not linked with any datasource
					bool theStopCanBeRemoved(true);
					LineStopTableSync::SearchResult lineStops(
						LineStopTableSync::Search(
							checkEnv,
							optional<RegistryKeyType>(),
							stop.getKey()
					)	);
					BOOST_FOREACH(const boost::shared_ptr<LineStop>& lineStop, lineStops)
					{
						if(	dynamic_cast<const JourneyPattern*>(lineStop->getParentPath()) &&
							!static_cast<const JourneyPattern*>(lineStop->getParentPath())->hasLinkWithSource(*dataSourceInCheckEnv)
						){
							theStopCanBeRemoved = false;
							break;
						}
					}
					if(!theStopCanBeRemoved)
					{
						continue;
					}

					// Check if the stop is not used in a DRT Area
					BOOST_FOREACH(const DRTArea::Registry::value_type& drtArea, _env.getRegistry<DRTArea>())
					{
						BOOST_FOREACH(StopArea* stopArea, drtArea.second->getStops())
						{
							if(stopArea->getKey() == stop.getConnectionPlace()->getKey())
							{
								theStopCanBeRemoved = false;
								break;
							}
						}
					}
					if(!theStopCanBeRemoved)
					{
						continue;
					}

					// The removal
					_stopsToRemove.insert(itStopPoint.second);
					const_cast<StopArea*>(stop.getConnectionPlace())->removePhysicalStop(stop);
					if(stop.getConnectionPlace()->getPhysicalStops().empty())
					{
						_stopAreasToRemove.insert(_env.getEditableSPtr(const_cast<StopArea*>(stop.getConnectionPlace())));
					}
				}

				// Stops to delete are removed from the environment to avoid useless saving
				BOOST_FOREACH(const boost::shared_ptr<StopPoint>& stop, _stopsToRemove)
				{
					_env.getEditableRegistry<StopPoint>().remove(stop->getKey());
				}

				// Stop areas to delete are removed from the environment to avoid useless saving
				BOOST_FOREACH(const boost::shared_ptr<StopArea>& stopArea, _stopAreasToRemove)
				{
					_env.getEditableRegistry<StopArea>().remove(stopArea->getKey());
				}
			}
		}



		void PTDataCleanerFileFormat::_addRemoveQueries( db::DBTransaction& transaction ) const
		{

			// Vehicle services
			BOOST_FOREACH(const boost::shared_ptr<const VehicleService>& vehicleService, _vehicleServicesToRemove)
			{
				VehicleServiceTableSync::Remove(
					NULL, 
					vehicleService->getKey(),
					transaction,
					false
				);
			}

			BOOST_FOREACH(const boost::shared_ptr<ScheduledService>& sservice, _scheduledServicesToRemove)
			{
				ScheduledServiceTableSync::Remove(
					NULL,
					sservice->getKey(),
					transaction,
					false
				);
			}
			BOOST_FOREACH(const boost::shared_ptr<ContinuousService>& cservice, _continuousServicesToRemove)
			{
				ContinuousServiceTableSync::Remove(
					NULL,
					cservice->getKey(),
					transaction,
					false
				);
			}
			BOOST_FOREACH(const boost::shared_ptr<JourneyPattern>& journeyPattern, _journeyPatternsToRemove)
			{
				JourneyPatternTableSync::Remove(
					NULL,
					journeyPattern->getKey(),
					transaction,
					false
				);
			}
			BOOST_FOREACH(const boost::shared_ptr<StopPoint>& stop, _stopsToRemove)
			{
				StopPointTableSync::Remove(
					NULL,
					stop->getKey(),
					transaction,
					false
				);
			}
			BOOST_FOREACH(const boost::shared_ptr<StopArea>& stopArea, _stopAreasToRemove)
			{
				StopAreaTableSync::Remove(
					NULL,
					stopArea->getKey(),
					transaction,
					false
				);
			}
		}



		bool PTDataCleanerFileFormat::beforeParsing()
		{
			_cleanCalendars();
			return !_cleanOldData || !_calendar.empty() || _fromToday;
		}



		bool PTDataCleanerFileFormat::afterParsing()
		{
			_selectObjectsToRemove();
			return true;
		}



		void PTDataCleanerFileFormat::_setFromParametersMap( const util::ParametersMap& map )
		{
			_cleanOldData = map.getDefault<bool>(PARAMETER_CLEAN_OLD_DATA, true);
			_cleanUnusedStops = map.getDefault<bool>(PARAMETER_CLEAN_UNUSED_STOPS, false);
			_autoPurge = map.getDefault<bool>(PARAMETER_AUTO_PURGE, false);

			RegistryKeyType calendarId(map.getDefault<RegistryKeyType>(PARAMETER_CALENDAR_ID, 0));
			if(calendarId) try
			{
				_calendarTemplate = CalendarTemplateTableSync::Get(calendarId, _env);
			}
			catch(ObjectNotFoundException<CalendarTemplate>&)
			{
				throw RequestException("No such calendar template");
			}

			date startDate(not_a_date_time);
			date endDate(not_a_date_time);
			if(!map.getDefault<string>(PARAMETER_START_DATE).empty())
			{
				startDate = from_string(map.get<string>(PARAMETER_START_DATE));
			}
			if(!map.getDefault<string>(PARAMETER_END_DATE).empty())
			{
				endDate = from_string(map.get<string>(PARAMETER_END_DATE));
			}

			_fromToday = map.getDefault<bool>(PARAMETER_FROM_TODAY, false);

			_calendar.clear();
			if(!_fromToday)
			{
				if(_calendarTemplate.get())
				{
					if(!startDate.is_not_a_date() && endDate.is_not_a_date())
					{
						_calendar = _calendarTemplate->getResult(Calendar(startDate, endDate));
					}
					else if(_calendarTemplate->isLimited())
					{
						_calendar = _calendarTemplate->getResult();
						if(!startDate.is_not_a_date())
						{
							_calendar &= Calendar(startDate, _calendar.getLastActiveDate());
						}
						if(!endDate.is_not_a_date())
						{
							_calendar &= Calendar(_calendar.getFirstActiveDate(), endDate);
						}
					}
				}
				else
				{
					if(!startDate.is_not_a_date() && !endDate.is_not_a_date())
					{
						_calendar = Calendar(startDate, endDate);
					}
				}
			}
		}



		util::ParametersMap PTDataCleanerFileFormat::_getParametersMap() const
		{
			ParametersMap result;
			result.insert(PARAMETER_CLEAN_OLD_DATA, _cleanOldData);
			if(_calendarTemplate.get())
			{
				result.insert(PARAMETER_CALENDAR_ID, _calendarTemplate->getKey());
			}
			result.insert(PARAMETER_FROM_TODAY, _fromToday);
			result.insert(PARAMETER_CLEAN_UNUSED_STOPS, _cleanUnusedStops);
			if(_autoPurge)
			{
				result.insert(PARAMETER_AUTO_PURGE, _autoPurge);
			}
			return result;
		}



		void PTDataCleanerFileFormat::cleanObsoleteData(
			const date& firstDayToKeep
		) const {

			DataSource& dataSource(*_import.get<DataSource>());
			ImportableTableSync::ObjectBySource<JourneyPatternTableSync> journeyPatterns(dataSource, _env);

			set<ScheduledService*> scheduledServicesToRemove;
			set<ContinuousService*> continuousServicesToRemove;
			set<JourneyPattern*> journeyPatternsToRemove;

			// Select obsolete services
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::value_type& itPathSet, journeyPatterns.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::mapped_type::value_type& itPath, itPathSet.second)
				{
					ScheduledServiceTableSync::Search(_env, itPath->getKey());
					ContinuousServiceTableSync::Search(_env, itPath->getKey());
					boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
								*itPath->sharedServicesMutex
					);

					BOOST_FOREACH(const ServiceSet::value_type& itService, itPath->getServices())
					{
						if(!dynamic_cast<NonPermanentService*>(itService))
						{
							continue;
						}

						NonPermanentService* service(static_cast<NonPermanentService*>(itService));

						// Avoid call of Calendar constructor with undefined end date for services without dates
						if(service->empty() || service->getLastActiveDate() < firstDayToKeep)
						{
							if(dynamic_cast<ScheduledService*>(service))
							{
								scheduledServicesToRemove.insert(static_cast<ScheduledService*>(service));
							}
							else if(dynamic_cast<ContinuousService*>(service))
							{
								continuousServicesToRemove.insert(static_cast<ContinuousService*>(service));
							}
						}
					}
			}	}

			// Removes all deleted services from their corresponding route
			BOOST_FOREACH(ScheduledService* scheduledService, scheduledServicesToRemove)
			{
				const_cast<JourneyPattern*>(scheduledService->getRoute())->removeService(*scheduledService);
			}
			BOOST_FOREACH(ContinuousService* continuousService, continuousServicesToRemove)
			{
				const_cast<JourneyPattern*>(continuousService->getRoute())->removeService(*continuousService);
			}

			// Select empty routes (due to services removal or not)
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::value_type& itPathSet, journeyPatterns.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::mapped_type::value_type& itPath, itPathSet.second)
				{
					if(itPath->getServices().empty())
					{
						journeyPatternsToRemove.insert(itPath);
					}
			}	}

			// Remove services
			DBTransaction t;
			DB& db(*DBModule::GetDB());
			BOOST_FOREACH(ScheduledService* scheduledService, scheduledServicesToRemove)
			{
				db.deleteStmt(scheduledService->getKey(), t);
			}
			BOOST_FOREACH(ContinuousService* continuousService, continuousServicesToRemove)
			{
				db.deleteStmt(continuousService->getKey(), t);
			}
			BOOST_FOREACH(JourneyPattern* journeyPatterns, journeyPatternsToRemove)
			{
				BOOST_FOREACH(const Edge* edge, journeyPatterns->getEdges())
				{
					db.deleteStmt(edge->getKey(), t);
				}
				db.deleteStmt(journeyPatterns->getKey(), t);
			}
			t.run();
		}
}	}
