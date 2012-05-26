
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
#include "DriverAllocationTableSync.hpp"
#include "DriverAllocationTemplateTableSync.hpp"
#include "DriverServiceTableSync.hpp"
#include "DRTAreaTableSync.hpp"
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
	using namespace pt_operation;
	using namespace server;
	using namespace util;

	namespace pt
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
			const DataSource& dataSource
		):	Importer(env, dataSource),
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

			// PT data
			ImportableTableSync::ObjectBySource<JourneyPatternTableSync> journeyPatterns(_dataSource, _env);
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::value_type& itPathSet, journeyPatterns.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::mapped_type::value_type& itPath, itPathSet.second)
				{
					ScheduledServiceTableSync::Search(_env, itPath->getKey());
					ContinuousServiceTableSync::Search(_env, itPath->getKey());

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

			// Driver allocations
			ImportableTableSync::ObjectBySource<DriverAllocationTableSync> driverAllocations(_dataSource, _env);
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverAllocationTableSync>::Map::value_type& itDASet, driverAllocations.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverAllocationTableSync>::Map::mapped_type::value_type& itDA, itDASet.second)
				{
					if(	_autoPurge ||
						(_fromToday && now <= itDA->get<Date>()) ||
						_calendar.isActive(itDA->get<Date>())
					){
						_driverAllocationsToRemove.insert(
							_env.getRegistry<DriverAllocation>().get(itDA->getKey())
						);
					}
			}	}

			// Driver allocation templates
			ImportableTableSync::ObjectBySource<DriverAllocationTemplateTableSync> driverAllocationTemplates(_dataSource, _env);
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverAllocationTemplateTableSync>::Map::value_type& itDASet, driverAllocationTemplates.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverAllocationTemplateTableSync>::Map::mapped_type::value_type& itDA, itDASet.second)
				{
					if(	_autoPurge ||
						(_fromToday && now <= itDA->get<Date>()) ||
						_calendar.isActive(itDA->get<Date>())
					){
						_driverAllocationTemplatesToRemove.insert(
							_env.getRegistry<DriverAllocationTemplate>().get(itDA->getKey())
						);
					}
			}	}

			// Driver services
			ImportableTableSync::ObjectBySource<DriverServiceTableSync> driverServices(_dataSource, _env);
			DriverService::Chunks emptyChunks;
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverServiceTableSync>::Map::value_type& itDSSet, driverServices.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverServiceTableSync>::Map::mapped_type::value_type& itDS, itDSSet.second)
				{
					itDS->setChunks(emptyChunks);
			}	}

			// Vehicle services
			ImportableTableSync::ObjectBySource<VehicleServiceTableSync> vehicleServices(_dataSource, _env);
			VehicleService::DriverServiceChunks emptyVSChunks;
			VehicleService::Services emptyVSServices;
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<VehicleServiceTableSync>::Map::value_type& itVSSet, vehicleServices.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<VehicleServiceTableSync>::Map::mapped_type::value_type& itVS, itVSSet.second)
				{
					itVS->setServices(emptyVSServices);
					itVS->setDriverServices(emptyVSChunks);
			}	}
		}



		void PTDataCleanerFileFormat::_selectObjectsToRemove() const
		{
			// Scheduled services without any active date
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& itService, _env.getRegistry<ScheduledService>())
			{
				if(itService.second->getRoute()->hasLinkWithSource(_dataSource) && itService.second->empty())
				{
					_scheduledServicesToRemove.insert(itService.second);
					itService.second->getPath()->removeService(*itService.second);
				}
			}

			// Continuous services without any active date
			BOOST_FOREACH(const Registry<ContinuousService>::value_type& itCService, _env.getRegistry<ContinuousService>())
			{
				if(itCService.second->getRoute()->hasLinkWithSource(_dataSource) && itCService.second->empty())
				{
					_continuousServicesToRemove.insert(itCService.second);
					itCService.second->getPath()->removeService(*itCService.second);
				}
			}

			// Journey patterns without any service
			BOOST_FOREACH(const Registry<JourneyPattern>::value_type& itJourneyPattern, _env.getRegistry<JourneyPattern>())
			{
				if(itJourneyPattern.second->hasLinkWithSource(_dataSource) && itJourneyPattern.second->getServices().empty())
				{
					_journeyPatternsToRemove.insert(itJourneyPattern.second);
				}
			}

			// Scheduled services to delete are removed from the environment to avoid useless saving
			BOOST_FOREACH(const shared_ptr<ScheduledService>& sservice, _scheduledServicesToRemove)
			{
				_env.getEditableRegistry<ScheduledService>().remove(sservice->getKey());
			}

			// Continuous services to delete are removed from the environment to avoid useless saving
			BOOST_FOREACH(const shared_ptr<ContinuousService>& cservice, _continuousServicesToRemove)
			{
				_env.getEditableRegistry<ContinuousService>().remove(cservice->getKey());
			}

			// Journey patterns to delete are removed from the environment to avoid useless saving
			BOOST_FOREACH(const shared_ptr<JourneyPattern>& journeyPattern, _journeyPatternsToRemove)
			{
				BOOST_FOREACH(const Edge* edge, journeyPattern->getEdges())
				{
					if(dynamic_cast<const DesignatedLinePhysicalStop*>(edge))
					{
						_edgesToRemove.insert(static_pointer_cast<const LineStop, const Edge>(_env.getSPtr(dynamic_cast<const DesignatedLinePhysicalStop*>(edge))));
						_env.getEditableRegistry<DesignatedLinePhysicalStop>().remove(edge->getKey());
					}
					else if(dynamic_cast<const LineArea*>(edge))
					{
						_edgesToRemove.insert(static_pointer_cast<const LineStop, const LineArea>(_env.getSPtr(dynamic_cast<const LineArea*>(edge))));
						_env.getEditableRegistry<LineArea>().remove(edge->getKey());
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

			// Vehicle services without services
			BOOST_FOREACH(const Registry<VehicleService>::value_type& itVehicleService, _env.getRegistry<VehicleService>())
			{
				if(	itVehicleService.second->hasLinkWithSource(_dataSource) &&
					itVehicleService.second->getServices().empty()
				){
					_vehicleServicesToRemove.insert(itVehicleService.second);
				}
			}

			// Driver services without services
			BOOST_FOREACH(const Registry<DriverService>::value_type& itDriverService, _env.getRegistry<DriverService>())
			{
				if(!itDriverService.second->hasLinkWithSource(_dataSource))
				{
					continue;
				}

				DriverService::Chunks chunks;
				BOOST_FOREACH(const DriverService::Chunks::value_type& chunk, itDriverService.second->getChunks())
				{
					if( !chunk.vehicleService ||
						_vehicleServicesToRemove.find(
							_env.getRegistry<VehicleService>().get(chunk.vehicleService->getKey())
						) == _vehicleServicesToRemove.end()
					){
						chunks.push_back(chunk);
					}
				}

				if(chunks.empty())
				{
					_driverServicesToRemove.insert(itDriverService.second);
				}
				else
				{
					itDriverService.second->setChunks(chunks);
				}
			}

			// Driver allocations
			BOOST_FOREACH(
				const shared_ptr<const DriverAllocation>& driverAllocation,
				_driverAllocationsToRemove
			){
				_env.getEditableRegistry<DriverAllocation>().remove(
					driverAllocation->getKey()
				);
			}

			// Driver allocation templates
			BOOST_FOREACH(
				const shared_ptr<const DriverAllocationTemplate>& driverAllocationTemplate,
				_driverAllocationTemplatesToRemove
			){
				_env.getEditableRegistry<DriverAllocationTemplate>().remove(
					driverAllocationTemplate->getKey()
				);
			}

			// Driver services
			BOOST_FOREACH(const shared_ptr<const DriverService>& driverService, _driverServicesToRemove)
			{
				_env.getEditableRegistry<DriverService>().remove(driverService->getKey());
			}

			// Vehicle services
			BOOST_FOREACH(const shared_ptr<const VehicleService>& vehicleService, _vehicleServicesToRemove)
			{
				_env.getEditableRegistry<VehicleService>().remove(vehicleService->getKey());
			}

			if(_cleanUnusedStops)
			{
				// Stops without Journey patterns without any service
				Env checkEnv;
				shared_ptr<const DataSource> dataSourceInCheckEnv(DataSourceTableSync::Get(_dataSource.getKey(), _env));
				DRTAreaTableSync::Search(_env);
				BOOST_FOREACH(const Registry<StopPoint>::value_type& itStopPoint, _env.getRegistry<StopPoint>())
				{
					const StopPoint& stop(*itStopPoint.second);

					// Check if the stop should be removed according to the imported files
					if(	!stop.hasLinkWithSource(_dataSource) ||
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
					BOOST_FOREACH(const shared_ptr<LineStop>& lineStop, lineStops)
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
				BOOST_FOREACH(const shared_ptr<StopPoint>& stop, _stopsToRemove)
				{
					_env.getEditableRegistry<StopPoint>().remove(stop->getKey());
				}

				// Stop areas to delete are removed from the environment to avoid useless saving
				BOOST_FOREACH(const shared_ptr<StopArea>& stopArea, _stopAreasToRemove)
				{
					_env.getEditableRegistry<StopArea>().remove(stopArea->getKey());
				}
			}
		}



		void PTDataCleanerFileFormat::_addRemoveQueries( db::DBTransaction& transaction ) const
		{
			BOOST_FOREACH(const shared_ptr<ScheduledService>& sservice, _scheduledServicesToRemove)
			{
				ScheduledServiceTableSync::RemoveRow(sservice->getKey(), transaction);
			}
			BOOST_FOREACH(const shared_ptr<ContinuousService>& cservice, _continuousServicesToRemove)
			{
				ContinuousServiceTableSync::RemoveRow(cservice->getKey(), transaction);
			}
			BOOST_FOREACH(const shared_ptr<JourneyPattern>& journeyPattern, _journeyPatternsToRemove)
			{
				BOOST_FOREACH(const Edge* edge, journeyPattern->getEdges())
				{
					LineStopTableSync::RemoveRow(edge->getKey(), transaction);
				}
				JourneyPatternTableSync::RemoveRow(journeyPattern->getKey(), transaction);
			}
			BOOST_FOREACH(const shared_ptr<StopPoint>& stop, _stopsToRemove)
			{
				StopPointTableSync::RemoveRow(stop->getKey(), transaction);
			}
			BOOST_FOREACH(const shared_ptr<StopArea>& stopArea, _stopAreasToRemove)
			{
				StopAreaTableSync::RemoveRow(stopArea->getKey(), transaction);
			}

			// Driver allocation templates
			BOOST_FOREACH(
				const shared_ptr<const DriverAllocationTemplate>& driverAllocation,
				_driverAllocationTemplatesToRemove
			){
				DriverAllocationTemplateTableSync::RemoveRow(driverAllocation->getKey(), transaction);
			}

			// Driver allocations
			BOOST_FOREACH(
				const shared_ptr<const DriverAllocation>& driverAllocation,
				_driverAllocationsToRemove
			){
				DriverAllocationTableSync::RemoveRow(driverAllocation->getKey(), transaction);
			}

			// Driver services
			BOOST_FOREACH(const shared_ptr<const DriverService>& driverService, _driverServicesToRemove)
			{
				DriverServiceTableSync::RemoveRow(driverService->getKey(), transaction);
			}

			// Vehicle services
			BOOST_FOREACH(const shared_ptr<const VehicleService>& vehicleService, _vehicleServicesToRemove)
			{
				VehicleServiceTableSync::RemoveRow(vehicleService->getKey(), transaction);
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

			if(!map.getDefault<string>(PARAMETER_START_DATE).empty())
			{
				_startDate = from_string(map.get<string>(PARAMETER_START_DATE));
			}
			if(!map.getDefault<string>(PARAMETER_END_DATE).empty())
			{
				_endDate = from_string(map.get<string>(PARAMETER_END_DATE));
			}

			_fromToday = map.getDefault<bool>(PARAMETER_FROM_TODAY, false);

			_calendar.clear();
			if(!_fromToday)
			{
				if(_calendarTemplate.get())
				{
					if(_startDate && _endDate)
					{
						_calendar = _calendarTemplate->getResult(Calendar(*_startDate, *_endDate));
					}
					else if(_calendarTemplate->isLimited())
					{
						_calendar = _calendarTemplate->getResult();
						if(_startDate)
						{
							_calendar &= Calendar(*_startDate, _calendar.getLastActiveDate());
						}
						if(_endDate)
						{
							_calendar &= Calendar(_calendar.getFirstActiveDate(), *_endDate);
						}
					}
				}
				else
				{
					if(_startDate && _endDate)
					{
						_calendar = Calendar(*_startDate, *_endDate);
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
			if(_startDate)
			{
				result.insert(PARAMETER_START_DATE, *_startDate);
			}
			if(_endDate)
			{
				result.insert(PARAMETER_END_DATE, *_endDate);
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
			ImportableTableSync::ObjectBySource<JourneyPatternTableSync> journeyPatterns(_dataSource, _env);

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
			BOOST_FOREACH(ScheduledService* scheduledService, scheduledServicesToRemove)
			{
				ScheduledServiceTableSync::RemoveRow(scheduledService->getKey(), t);
			}
			BOOST_FOREACH(ContinuousService* continuousService, continuousServicesToRemove)
			{
				ContinuousServiceTableSync::RemoveRow(continuousService->getKey(), t);
			}
			BOOST_FOREACH(JourneyPattern* journeyPatterns, journeyPatternsToRemove)
			{
				BOOST_FOREACH(const Edge* edge, journeyPatterns->getEdges())
				{
					LineStopTableSync::RemoveRow(edge->getKey(), t);
				}
				JourneyPatternTableSync::RemoveRow(journeyPatterns->getKey(), t);
			}
			t.run();
		}
}	}
