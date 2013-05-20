
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
		const string ScheduleRealTimeUpdateService::PARAMETER_STOP_AREA_ID = "sa";
		const string ScheduleRealTimeUpdateService::PARAMETER_STOP_AREA_DATASOURCE_ID = "sads";
		/// Warning, PARAMETER_SERVICE_ID is a list starting at se1 up to any number in sequence
		const string ScheduleRealTimeUpdateService::PARAMETER_SERVICE_ID = "se";
		const string ScheduleRealTimeUpdateService::PARAMETER_SERVICE_DATASOURCE_ID = "ds";
		/// Warning, PARAMETER_DEPARTURE_TIME is a list starting at dt1 up to any number in sequence
		const string ScheduleRealTimeUpdateService::PARAMETER_DEPARTURE_TIME = "dt";
		/// Warning, PARAMETER_ARRIVAL_TIME is a list starting at at1 up to any number in sequence
		const string ScheduleRealTimeUpdateService::PARAMETER_ARRIVAL_TIME = "at";



		ParametersMap ScheduleRealTimeUpdateService::_getParametersMap() const
		{
			ParametersMap map;
			// This mode is deprecated
			return map;
		}



		void ScheduleRealTimeUpdateService::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DATASOURCE_ID, 0))
			{
				try
				{
					boost::shared_ptr<const DataSource> dataSource(
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

			size_t i(0);
			bool gotOne;
			do
			{
				Record record;
				gotOne = false;
				i++;
				std::string indexStr = static_cast<std::ostringstream*>( &(ostringstream() << i) )->str();

				if(map.getDefault<RegistryKeyType>(PARAMETER_SERVICE_DATASOURCE_ID, 0) &&
				   map.getDefault<RegistryKeyType>(PARAMETER_SERVICE_ID + indexStr, 0)
				)
				{
					try
					{
						boost::shared_ptr<const DataSource> dataSource(
							Env::GetOfficialEnv().getRegistry<DataSource>().get(map.get<RegistryKeyType>(PARAMETER_SERVICE_DATASOURCE_ID))
							);
						std::string serviceCodeBySource = map.get<string>(PARAMETER_SERVICE_ID + indexStr);
						ScheduledService* obj(dataSource->getObjectByCode<ScheduledService>(serviceCodeBySource));
						if(!obj)
						{
							throw ActionException("No such service");
						}
						record.service = Env::GetOfficialEnv().getEditableSPtr(obj);
						record.lineStopRank = _getStopRankByService(record.service.get(), _stopArea);
						gotOne = true;
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
						record.lineStopRank = _getStopRankByService(record.service.get(), _stopArea);
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
					if(record.service->getDepartureSchedule(false, record.lineStopRank) - departureTime > hours(12))
					{
						departureTime += hours(24);
					}
					record.departureTime = departureTime;
				}
				if(map.isDefined(PARAMETER_ARRIVAL_TIME + indexStr))
				{
					boost::posix_time::time_duration arrivalTime =
						duration_from_string(map.get<string>(PARAMETER_ARRIVAL_TIME + indexStr));
					// Detection of bad encoding of schedules after midnight
					if(record.service->getArrivalSchedule(false, record.lineStopRank) - arrivalTime > hours(12))
					{
						arrivalTime += hours(24);
					}
					record.arrivalTime = arrivalTime;
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
			ptime now(second_clock::local_time());

			if(_records.empty())
			{
				// Nothing to do
				return pm;
			}

			BOOST_FOREACH(Record record, _records ) 
			{
				//
				// Resync the all services that are between the old scheduled date and the
				// new one for the given service and line stop.
				//
				const StopPoint *sp(static_cast<const StopPoint*>(record.service->getRealTimeVertex(record.lineStopRank)));
				ArrivalDepartureTableGenerator::PhysicalStops ps;
				ps.insert(make_pair(sp->getKey(), sp));
				DeparturesTableDirection di(DISPLAY_ARRIVALS);
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
					ptime(now.date(), record.service->getArrivalBeginScheduleToIndex(true, record.lineStopRank)),
					ptime(now.date(), record.departureTime),
					false
					);

				BOOST_FOREACH(const ArrivalDepartureList::value_type& itService, tdg.generate())
				{
					const SchedulesBasedService *nextService (dynamic_cast<const SchedulesBasedService*>(itService.first.getService()));
					size_t lineStopRank(_getStopRankByService(nextService, _stopArea));
					posix_time::time_duration theirArrivalTimeRef(
						nextService->getArrivalBeginScheduleToIndex(true, lineStopRank)
					);
					posix_time::time_duration theirDepartureTimeRef(
						nextService->getDepartureBeginScheduleToIndex(true, lineStopRank)
					);
					// Reschedule this service to the new given time
					const_cast<SchedulesBasedService*>(nextService)->applyRealTimeShiftDuration(
						lineStopRank,
						record.arrivalTime - theirArrivalTimeRef,
						record.departureTime - theirDepartureTimeRef,
						true,
						(nextService == record.service.get()) // Time stamp only the requested service
					);
					stream << "re-scheduling service " << nextService->getKey() << " " << nextService->getServiceNumber() << endl;
				}
			}
			return pm;
		}

		size_t ScheduleRealTimeUpdateService::_getStopRankByService(
			const SchedulesBasedService *service, 
			const boost::shared_ptr<StopArea> &stopArea) const
		{
			int i(0);
			BOOST_FOREACH(const graph::Vertex *vertex, service->getVertices(true))
			{
				if(vertex->getHub() == stopArea.get())
				{
					return i;
				}
				i++;
			}
			throw ActionException("No such stop in this service");
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

}	}
