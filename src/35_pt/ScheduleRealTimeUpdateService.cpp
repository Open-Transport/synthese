
/** ScheduleRealTimeUpdateService class implementation.
	@file ScheduleRealTimeUpdateService.cpp
	@author Hugues
	@date 2009

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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ScheduleRealTimeUpdateService.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "Env.h"
#include "ScheduledService.h"
#include "SchedulesBasedService.h"
#include "LineStop.h"
#include "StopPoint.hpp"
#include "DataSourceTableSync.h"
#include "JourneyPattern.hpp"
#include "StandardArrivalDepartureTableGenerator.h"
#include "ArrivalDepartureTableGenerator.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace graph;
	using namespace pt;
	using namespace impex;
	using namespace departure_boards;

	namespace util
	{
		template<> const string FactorableTemplate<Function, pt::ScheduleRealTimeUpdateService>::FACTORY_KEY("ScheduleRealTimeUpdateService");
	}

	namespace pt
	{
		const string ScheduleRealTimeUpdateService::PARAMETER_LINE_STOP_RANK = "ls";
		const string ScheduleRealTimeUpdateService::PARAMETER_STOP_AREA_ID = "sa";
		const string ScheduleRealTimeUpdateService::PARAMETER_STOP_AREA_DATASOURCE_ID = "sads";
		/// Warning, PARAMETER_SERVICE_ID is a list starting at se1 up to any number in sequence
		const string ScheduleRealTimeUpdateService::PARAMETER_SERVICE_ID = "se";
		const string ScheduleRealTimeUpdateService::PARAMETER_SERVICE_DATASOURCE_ID = "ds";
		/// Warning, PARAMETER_DEPARTURE_TIME is a list starting at dt1 up to any number in sequence
		const string ScheduleRealTimeUpdateService::PARAMETER_DEPARTURE_TIME = "dt";
		/// Warning, PARAMETER_ARRIVAL_TIME is a list starting at at1 up to any number in sequence
		const string ScheduleRealTimeUpdateService::PARAMETER_ARRIVAL_TIME = "at";


		ScheduleRealTimeUpdateService::ScheduleRealTimeUpdateService()
		{}



		ParametersMap ScheduleRealTimeUpdateService::_getParametersMap() const
		{
			ParametersMap map;
			// This mode is deprecated
			return map;
		}



		void ScheduleRealTimeUpdateService::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.isDefined(PARAMETER_LINE_STOP_RANK))
			{
				_lineStopRank = map.get<size_t>(PARAMETER_LINE_STOP_RANK);
			}
			else
			{
				if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DATASOURCE_ID, 0))
				{
					try
					{
						shared_ptr<const DataSource> dataSource(
							Env::GetOfficialEnv().getRegistry<DataSource>().get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DATASOURCE_ID))
							);
						std::string stopAreaCodeBySource = map.get<string>(PARAMETER_STOP_AREA_ID);
						StopArea* obj(dataSource->getObjectByCode<StopArea>(stopAreaCodeBySource));
						if(!obj)
						{
							throw ActionException("No such stop area");
						}
						_stopArea = Env::GetOfficialEnv().getEditableSPtr(obj);
					}
					catch(ObjectNotFoundException<DataSource>&)
					{
						throw ActionException("No such datasource");
					}
				}
				else try
				{
					_stopArea = Env::GetOfficialEnv().getEditableRegistry<StopArea>().getEditable(
						map.get<RegistryKeyType>(PARAMETER_STOP_AREA_ID)
						);
				}
				catch(ObjectNotFoundException<StopArea>)
				{
					throw ActionException("No such stop area");
				}
			}

			size_t i(0);
			bool gotOne;
			do
			{
				Record record;
				gotOne = false;
				i++;
				std::string indexStr = static_cast<std::ostringstream*>( &(ostringstream() << i) )->str();

				if(map.getDefault<RegistryKeyType>(PARAMETER_SERVICE_DATASOURCE_ID + indexStr, 0))
				{
					try
					{
						shared_ptr<const DataSource> dataSource(
							Env::GetOfficialEnv().getRegistry<DataSource>().get(map.get<RegistryKeyType>(PARAMETER_SERVICE_DATASOURCE_ID + indexStr))
							);
						std::string serviceCodeBySource = map.get<string>(PARAMETER_SERVICE_ID + indexStr);
						ScheduledService* obj(dataSource->getObjectByCode<ScheduledService>(serviceCodeBySource));
						if(!obj)
						{
							throw ActionException("No such service");
						}
						record.service.reset(obj);
					}
					catch(ObjectNotFoundException<DataSource>&)
					{
						throw ActionException("No such datasource");
					}
				}
				else if(map.getDefault<RegistryKeyType>(PARAMETER_SERVICE_ID + indexStr, 0))
				{
					try
					{
						record.service = Env::GetOfficialEnv().getEditableRegistry<ScheduledService>().getEditable(
							map.get<RegistryKeyType>(PARAMETER_SERVICE_ID + indexStr)
							);
						gotOne = true;
					}
					catch(ObjectNotFoundException<ScheduledService>)
					{
						throw ActionException("No such service");
					}
				}
				// If we got no service, no need to try to find a Time
				if(!gotOne)
				{
					break;
				}

				if(map.isDefined(PARAMETER_DEPARTURE_TIME + indexStr))
				{
					boost::posix_time::time_duration departureTime = 
						duration_from_string(map.get<string>(PARAMETER_DEPARTURE_TIME + indexStr));
					// Detection of bad encoding of schedules after midnight
					if(record.service->getDepartureSchedule(false, _lineStopRank) - departureTime > hours(12))
					{
						departureTime += hours(24);
					}
					record.newTime = departureTime;
					record.isArrival = false;
				}
				if(map.isDefined(PARAMETER_ARRIVAL_TIME + indexStr))
				{
					boost::posix_time::time_duration arrivalTime =
						duration_from_string(map.get<string>(PARAMETER_ARRIVAL_TIME + indexStr));
					// Detection of bad encoding of schedules after midnight
					if(record.service->getArrivalSchedule(false, _lineStopRank) - arrivalTime > hours(12))
					{
						arrivalTime += hours(24);
					}
					record.newTime = arrivalTime;
					record.isArrival = true;
				}

				if(gotOne)
				{
					_records.push_back(record);
				}
			} while (gotOne);
		}


		util::ParametersMap ScheduleRealTimeUpdateService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap pm;

			if(_records.empty())
			{
				// Nothing to do
				return pm;
			}

			BOOST_FOREACH(Record record,	
				_records ) 
			{
				posix_time::time_duration ourTimeRef(
					record.isArrival 
					? record.service->getArrivalBeginScheduleToIndex(true, _lineStopRank) 
					: record.service->getDepartureBeginScheduleToIndex(true, _lineStopRank)
					);

				posix_time::time_duration lateDuration(record.newTime - ourTimeRef);

				ptime now(second_clock::local_time());

				//
				// Resync the all services that are between the old sceduled date and the
				// new one for the given service and line stop.
				//
				const StopPoint *sp(static_cast<const StopPoint*>(record.service->getRealTimeVertex(_lineStopRank)));
				ArrivalDepartureTableGenerator::PhysicalStops ps;
				ps.insert(make_pair(sp->getKey(), sp));
				DeparturesTableDirection di(record.isArrival ? DISPLAY_ARRIVALS : DISPLAY_DEPARTURES);
				EndFilter ef(WITH_PASSING);
				LineFilter lf;
				DisplayedPlacesList dp;
				ForbiddenPlacesList fp;
				StandardArrivalDepartureTableGenerator tdg(
					ps,
					di,
					ef,
					lf,
					dp,
					fp,
					ptime(now.date(), ourTimeRef),
					ptime(now.date(), record.newTime),
					false
					);

				BOOST_FOREACH(const ArrivalDepartureList::value_type& itService, tdg.generate())
				{
					const SchedulesBasedService *nextService (dynamic_cast<const SchedulesBasedService*>(itService.first.getService()));

					posix_time::time_duration theirTimeRef(
						record.isArrival
						? nextService->getArrivalBeginScheduleToIndex(true, _lineStopRank) 
						: nextService->getDepartureBeginScheduleToIndex(true, _lineStopRank)
						);

					// Reschedule this service to the new given time 
					const_cast<SchedulesBasedService*>(nextService)->applyRealTimeLateDuration(
						_lineStopRank,
						ourTimeRef + lateDuration - theirTimeRef,
						record.isArrival,
						!record.isArrival,
						true
						);
				}
			}
			return pm;
		}


		bool ScheduleRealTimeUpdateService::isAuthorized(const Session* session
		) const {
#if 1
			return true;
#else // See https://extranet.rcsmobility.com/issues/16043
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
#endif
		}

		std::string ScheduleRealTimeUpdateService::getOutputMimeType() const
		{
			return "text/javascript";
		}


		void ScheduleRealTimeUpdateService::setLineStopRank( std::size_t value )
		{
			_lineStopRank = value;
		}
}	}
