
/** IsochronService class implementation.
	@file IsochronService.cpp

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
*/

#include "IsochronService.hpp"

#include "AdminParametersException.h"
#include "RequestException.h"
#include "Request.h"
#include "Webpage.h"

#include "AdminFunctionRequest.hpp"
#include "AccessParameters.h"
#include "AlgorithmLogger.hpp"
#include "AnalysisModule.hpp"
#include "IntegralSearcher.h"
#include "GlobalRight.h"
#include "ParametersMap.h"
#include "PlacesListFunction.h"
#include "PropertiesHTMLTable.h"
#include "PTModule.h"
#include "PTTimeSlotRoutePlanner.h"
#include "PTRoutePlannerResult.h"
#include "RequestException.h"
#include "RoadModule.h"
#include "SearchFormHTMLTable.h"
#include "Vertex.h"
#include "Edge.h"


using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace analysis;
	using namespace cms;
	using namespace server;
	using namespace util;
	using namespace pt;
	using namespace admin;
	using namespace algorithm;
	using namespace graph;
	using namespace gregorian;
	using namespace security;

	template<> const string util::FactorableTemplate<Function, analysis::IsochronService>::FACTORY_KEY("IsochronService");

	namespace analysis
	{
		const std::string IsochronService::PARAMETER_START_PLACE("start_place");
		const std::string IsochronService::PARAMETER_MAX_DISTANCE("max_distance");

		const std::string IsochronService::PARAMETER_DATE("date");
		const std::string IsochronService::PARAMETER_BEGIN_TIME_SLOT("begin_time_slot");
		const std::string IsochronService::PARAMETER_END_TIME_SLOT("end_time_slot");
		const std::string IsochronService::PARAMETER_MAX_CONNECTIONS("max_connections");

		const std::string IsochronService::PARAMETER_CURVES_STEP("curves_step");
		const std::string IsochronService::PARAMETER_MAX_DURATION("max_duration");
		const std::string IsochronService::PARAMETER_DURATION_TYPE("duration_type");
		const std::string IsochronService::PARAMETER_FREQUENCY_TYPE("frequency_type");
		const std::string IsochronService::PARAMETER_SPEED("speed");

		const std::string IsochronService::PARAMETER_PAGE("page");
		const std::string IsochronService::PARAMETER_BOARD_PAGE("board_page");
		const std::string IsochronService::PARAMETER_STOP_PAGE("stop_page");
		const std::string IsochronService::PARAMETER_TIME_PAGE("time_page");
		const std::string IsochronService::PARAMETER_WKT_PAGE("wkt_page");



		IsochronService::IsochronService()
		: _maxDistance(50),
			_date(boost::gregorian::day_clock::local_day()),
			_beginTimeSlot(7),
			_endTimeSlot(9),
			_maxConnections(1),
			_curvesStep(5),
			_maxDuration(60),
			_durationType(DURATION_TYPE_MEDIAN),
			_frequencyType(FREQUENCY_TYPE_NO),
			_speed(4)
		{
		}



		ParametersMap IsochronService::_getParametersMap() const
		{
			ParametersMap map;
			
			map.insert(PARAMETER_START_PLACE, (_startPlace.get() ? lexical_cast<string>(_startPlace->getKey()) : string()));
			map.insert(PARAMETER_MAX_DISTANCE, lexical_cast<string>(_maxDistance));

			map.insert(PARAMETER_DATE, lexical_cast<string>(_date));
			map.insert(PARAMETER_BEGIN_TIME_SLOT, lexical_cast<string>(_beginTimeSlot));
			map.insert(PARAMETER_END_TIME_SLOT, lexical_cast<string>(_endTimeSlot));
			map.insert(PARAMETER_MAX_CONNECTIONS, lexical_cast<string>(_maxConnections));

			map.insert(PARAMETER_CURVES_STEP, lexical_cast<string>(_curvesStep));
			map.insert(PARAMETER_MAX_DURATION, lexical_cast<string>(_maxDuration));
			map.insert(PARAMETER_DURATION_TYPE, lexical_cast<string>(_durationType));
			map.insert(PARAMETER_FREQUENCY_TYPE, lexical_cast<string>(_frequencyType));

			if(_page.get())
			{
				map.insert(PARAMETER_PAGE, _page->getKey());
			}
			if(_boardPage.get())
			{
				map.insert(PARAMETER_BOARD_PAGE, _boardPage->getKey());
			}
			if(_stopPage.get())
			{
				map.insert(PARAMETER_STOP_PAGE, _stopPage->getKey());
			}
			if(_timePage.get())
			{
				map.insert(PARAMETER_TIME_PAGE, _timePage->getKey());
			}
			if(_wktPage.get())
			{
				map.insert(PARAMETER_WKT_PAGE, _wktPage->getKey());
			}

			return map;
		}


		void IsochronService::_setFromParametersMap(const ParametersMap& map)
		{
			if(!map.getDefault<string>(PARAMETER_START_PLACE).empty())
			{
				try
				{
					_startPlace = Env::GetOfficialEnv().get<StopArea>(map.get<RegistryKeyType>(PARAMETER_START_PLACE));
				}
				catch(ObjectNotFoundException<StopArea>&)
				{
					throw AdminParametersException("No such stop area");
				}
			}

			// CMS output
			if(map.getOptional<RegistryKeyType>(PARAMETER_PAGE)) try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_PAGE));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such main page");
			}
			if(map.getOptional<RegistryKeyType>(PARAMETER_BOARD_PAGE)) try
			{
				_boardPage = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_BOARD_PAGE));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such boardPage");
			}
			if(map.getOptional<RegistryKeyType>(PARAMETER_STOP_PAGE)) try
			{
				_stopPage = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_STOP_PAGE));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such stopPage");
			}
			if(map.getOptional<RegistryKeyType>(PARAMETER_TIME_PAGE)) try
			{
				_timePage = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_TIME_PAGE));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such timePage");
			}
			if(map.getOptional<RegistryKeyType>(PARAMETER_WKT_PAGE)) try
			{
				_wktPage = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_WKT_PAGE));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such wktPage");
			}

			_maxDistance = map.getDefault<int>(PARAMETER_MAX_DISTANCE, 50);

			if(!map.getDefault<string>(PARAMETER_DATE).empty())
			{
				_date = gregorian::from_string(map.get<string>(PARAMETER_DATE));
			}
			_beginTimeSlot = map.getDefault<int>(PARAMETER_BEGIN_TIME_SLOT, 7);
			_endTimeSlot = map.getDefault<int>(PARAMETER_END_TIME_SLOT, 9);
			_maxConnections = map.getDefault<int>(PARAMETER_MAX_CONNECTIONS, 1);

			_curvesStep = map.getDefault<int>(PARAMETER_CURVES_STEP, 5);
			_maxDuration = map.getDefault<int>(PARAMETER_MAX_DURATION, 60);
			_durationType = map.getDefault<int>(PARAMETER_DURATION_TYPE, DURATION_TYPE_MEDIAN);
			_frequencyType = map.getDefault<int>(PARAMETER_FREQUENCY_TYPE, FREQUENCY_TYPE_NO);
			_speed = map.getDefault<int>(PARAMETER_SPEED, 4);
		}


		util::ParametersMap IsochronService::run(
			ostream& stream,
			const Request& request
		) const {
			ParametersMap result;
			
			if(_startPlace.get())
			{
				// Access Parameter
				AccessParameters accessParameter(
					USER_PEDESTRIAN,
					false, false, 1000, posix_time::minutes(23), 1.111,
					_maxConnections
				);

				// Logger
				AlgorithmLogger logger;

				// VertexAccessMap definition
				graph::VertexAccessMap ovam;
				graph::VertexAccessMap dvam;
				std::set<graph::GraphIdType> graphType;
				graphType.insert(PTModule::GRAPH_ID);
				_startPlace->getVertexAccessMap(ovam, accessParameter, graphType);

				GraphIdType graphId = PTModule::GRAPH_ID;

				ResultsMap resultsMap;
				ResultsMapAccess resultsMapAccess;
				int nbMinutes = (_endTimeSlot - _beginTimeSlot) * 60;

				// Launch IntergralSearcher for each minute of the slot [_beginTimeSlot;_endTimeSlot]
				for(int minute=0; minute <= nbMinutes; ++minute)
				{
					BestVertexReachesMap bestVertexReachesMap(DEPARTURE_TO_ARRIVAL, ovam, dvam, Vertex::GetMaxIndex());
					const ptime minMaxDateTimeAtOrigin = ptime(_date,time_duration(hours(_beginTimeSlot) + minutes(minute)));
					ptime minMaxDateTimeAtDestination = ptime(_date, time_duration(hours(_endTimeSlot)));
					JourneysResult result(minMaxDateTimeAtOrigin, DEPARTURE_TO_ARRIVAL);

					// Initialization of the IntegralSearcher
					IntegralSearcher is(
						DEPARTURE_TO_ARRIVAL,
						accessParameter,
						graphId,
						false,
						graphId,
						result,
						bestVertexReachesMap,
						ovam,
						minMaxDateTimeAtOrigin,
						minMaxDateTimeAtDestination,
						minMaxDateTimeAtDestination,
						false,
						false,
						boost::optional<boost::posix_time::time_duration>(),
						70.0,
						false,
						logger,
						0,
						boost::optional<const JourneyTemplates&>()
					);

					// Launch IntegralSearcher
					is.integralSearch(
						ovam,
						optional<size_t>(_maxConnections),
						optional<posix_time::time_duration>()
					);

					// Fill bestResultsMap (results of this iteration)
					BestResultsMap bestResultsMap;
					for(JourneysResult::ResultSet::const_iterator it(result.getJourneys().begin());
						it != result.getJourneys().end(); it++)
					{

						const RoutePlanningIntermediateJourney& journey(*it->first);
						const Vertex* reachedVertex(journey.getEndEdge().getFromVertex());
						if(!reachedVertex || !dynamic_cast<const StopArea*>(reachedVertex->getHub()))
						{
							continue;
						}
						const StopArea* reachedPlace = dynamic_cast<const StopArea*>(reachedVertex->getHub());

						if(bestResultsMap.count(reachedPlace->getKey()) == 1)
						{
							if(journey.getEffectiveDuration() < (*bestResultsMap[reachedPlace->getKey()]).first->getEffectiveDuration())
							{
								bestResultsMap.erase(reachedPlace->getKey());
								bestResultsMap.insert(pair<RegistryKeyType,JourneysResult::ResultSet::const_iterator>(reachedPlace->getKey(), it));
							}
						}
						else
						{
							bestResultsMap.insert(pair<RegistryKeyType,JourneysResult::ResultSet::const_iterator>(reachedPlace->getKey(), it));
						}
					}

					// Fill resultsMap (results of all iterations)
					BOOST_FOREACH(BestResultsMap::value_type it, bestResultsMap)
					{
						const RoutePlanningIntermediateJourney& journey(*(*it.second).first);
						const Vertex* reachedVertex(journey.getEndEdge().getFromVertex());
						if(!reachedVertex || !dynamic_cast<const StopArea*>(reachedVertex->getHub()))
						{
							continue;
						}
						const StopArea* reachedPlace = dynamic_cast<const StopArea*>(reachedVertex->getHub());

						// Ignore StopArea without geometry
						if(!reachedPlace->getPoint())
						{
							continue;
						}

						int distance = (int) (_startPlace->getPoint()->distance(reachedPlace->getPoint().get()) / 1000);
						int duration = journey.getEffectiveDuration().hours() * 60 + journey.getEffectiveDuration().minutes();

						// Tests time and length constraints
						if((distance > _maxDistance) || (duration > _maxDuration))
						{
							continue;
						}

						if(resultsMapAccess.count(reachedPlace->getKey()) == 1) // Update a result in ResultsMap
						{
							ResultsMap::iterator itMap = resultsMapAccess[reachedPlace->getKey()];

							if((minMaxDateTimeAtOrigin.time_of_day() > journey.getFirstDepartureTime(false).time_of_day()) ||
								(journey.getFirstDepartureTime(false).time_of_day() == (*itMap).second.lastDepartureTime.time_of_day())
							)
							{
								continue;
							}

							(*itMap).second.nbSolutions++;
							(*itMap).second.lastDepartureTime = journey.getFirstDepartureTime(false);
							(*itMap).second.timeDepartureList.push_back(journey.getFirstDepartureTime(false));
							if((*itMap).second.duration > duration)
							{
								(*itMap).second.duration = duration;
							}
						}
						else // Add new a result in ResultsMap
						{
							StopStruct stop;
							stop.stop = reachedPlace;
							stop.nbSolutions = 1;
							stop.duration = duration;
							stop.distance = distance;
							stop.lastDepartureTime = journey.getFirstDepartureTime();
							stop.timeDepartureList.push_back(journey.getFirstDepartureTime(false));
							ResultsMap::iterator itMap = resultsMap.insert(pair<int, StopStruct>(duration, stop));
							resultsMapAccess.insert(pair<RegistryKeyType,ResultsMap::iterator>(reachedPlace->getKey(), itMap));
						}
					}
				}

				// // CMS output
				if(_page.get()) 
				{
					int step = -_curvesStep;
					shared_ptr<ParametersMap> pm(new ParametersMap);
					pm->merge(getTemplateParameters());
					shared_ptr<ParametersMap> pmBoard(new ParametersMap);
					stringstream stopsStream;
					stringstream boardsStream;
					bool isfirst = true;
					for(ResultsMap::const_iterator it(resultsMap.begin());
						it != resultsMap.end(); it++
					){
						pmBoard->merge(getTemplateParameters());

						if((*it).second.duration >= (step + _curvesStep))
						{

							shared_ptr<ParametersMap> pmDuree(new ParametersMap);
							pmDuree->merge(getTemplateParameters());

							pmBoard->insert("duree_min", step);
							pmBoard->insert("duree_max", (step + _curvesStep - 1));

							if (isfirst)
							{
								isfirst = false;
							}
							else
							{
								pmBoard->insert("stops", stopsStream.str());
								
								//Output boards
								if (_boardPage.get())
								{
									_boardPage->display(boardsStream, request, *pmBoard);
								}

								pmBoard->clear();
								stopsStream.str("");
							}

							step = (*it).second.duration - ((*it).second.duration % _curvesStep);
						}
						
						shared_ptr<ParametersMap> pmStop(new ParametersMap);
						pmStop->merge(getTemplateParameters());

						pmStop->insert("city_name", (*it).second.stop->getCity()->getName());
						pmStop->insert("stop_name", (*it).second.stop->getName());
						pmStop->insert("nb_solutions", (*it).second.nbSolutions);
						pmStop->insert("duration", (*it).second.duration);
						pmStop->insert("distance", (*it).second.distance);
						pmStop->insert("speed", ((*it).second.distance / ((float)(*it).second.duration / 60)));

						shared_ptr<ParametersMap> pmTimes(new ParametersMap);
						pmTimes->merge(getTemplateParameters());
						stringstream timesStream;
						BOOST_FOREACH(const ptime time, (*it).second.timeDepartureList)
						{
							shared_ptr<ParametersMap> pmTime(new ParametersMap);
							pmTime->merge(getTemplateParameters());
							pmTime->insert("time",time.time_of_day());

							//Output times
							if (_timePage.get())
							{
								_timePage->display(timesStream, request, *pmTime);
							}
						}

						//Output times
						pmStop->insert("times", timesStream.str());
						if (_stopPage.get())
						{
							_stopPage->display(stopsStream, request, *pmStop);
						}
					}

					stringstream wktPointsStream;
					bool first = true;
					for(ResultsMap::const_iterator it(resultsMap.begin()); it != resultsMap.end(); it++)
					{
						shared_ptr<ParametersMap> pmWktPoint(new ParametersMap);
						pmWktPoint->merge(getTemplateParameters());

						shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*(*it).second.stop->getPoint()
						) );

						std::ostringstream streamPoint;

						streamPoint << "[ ";
						streamPoint << wgs84Point->getY();
						streamPoint << ", ";
						streamPoint << wgs84Point->getX();
						streamPoint << ", ";
						streamPoint << ((*it).second.duration); // duration
						streamPoint << " ]";

						pmWktPoint->insert("wktPoint",streamPoint.str());

						//Output wkt
						if (_wktPage.get())
						{
							_wktPage->display(wktPointsStream, request, *pmWktPoint);
						}
					}

					pm->insert("boards", boardsStream.str());
					pm->insert("wktPoints", wktPointsStream.str());
					_page->display(stream, request, *pm);
				}

			}

			return result;
		}
		
		bool IsochronService::isAuthorized(
			const Session* session
		) const {
			return true;
		}


		std::string IsochronService::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat();
		}
	}
}
