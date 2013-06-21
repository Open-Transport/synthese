
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
#include "CoordinatesSystem.hpp"
#include "Edge.h"
#include "IntegralSearcher.h"
#include "GlobalRight.h"
#include "ParametersMap.h"
#include "Place.h"
#include "PlacesListService.hpp"
#include "PTModule.h"
#include "PTRoutePlannerResult.h"
#include "RequestException.h"
#include "RoadModule.h"
#include "Vertex.h"

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
	using namespace pt_website;
	using namespace admin;
	using namespace algorithm;
	using namespace graph;
	using namespace gregorian;
	using namespace security;
	using namespace road;
	using namespace geography;

	template<> const string util::FactorableTemplate<Function, analysis::IsochronService>::FACTORY_KEY("IsochronService");

	namespace analysis
	{
		const std::string IsochronService::PARAMETER_COORDINATES_XY("coordinates_XY");
		const std::string IsochronService::PARAMETER_START_PLACE_NAME("start_place_name");
		const std::string IsochronService::PARAMETER_MAX_DISTANCE("max_distance");

		const std::string IsochronService::PARAMETER_NETWORK_LIST("nwl");
		const std::string IsochronService::PARAMETER_ROLLING_STOCK_LIST("tml");

		const std::string IsochronService::PARAMETER_DATE("date");
		const std::string IsochronService::PARAMETER_BEGIN_TIME_SLOT("begin_time_slot");
		const std::string IsochronService::PARAMETER_END_TIME_SLOT("end_time_slot");
		const std::string IsochronService::PARAMETER_MAX_CONNECTIONS("max_connections");

		const std::string IsochronService::PARAMETER_CURVES_STEP("curves_step");
		const std::string IsochronService::PARAMETER_MAX_DURATION("max_duration");
		const std::string IsochronService::PARAMETER_DURATION_TYPE("duration_type");
		const std::string IsochronService::PARAMETER_FREQUENCY_TYPE("frequency_type");
		const std::string IsochronService::PARAMETER_SPEED("speed");
		const std::string IsochronService::PARAMETER_ONLY_WKT("only_wkt");

		const std::string IsochronService::PARAMETER_PAGE("page");
		const std::string IsochronService::PARAMETER_BOARD_PAGE("board_page");
		const std::string IsochronService::PARAMETER_STOP_PAGE("stop_page");
		const std::string IsochronService::PARAMETER_TIME_PAGE("time_page");
		const std::string IsochronService::PARAMETER_SRID = "srid";
		const std::string IsochronService::PARAMETER_DEPARTURE_CLASS_FILTER = "departure_class_filter";



		IsochronService::IsochronService()
		: _maxDistance(50),
			_date(boost::gregorian::day_clock::local_day()),
			_beginTimeSlot(7),
			_endTimeSlot(9),
			_maxConnections(1),
			_curvesStep(5),
			_maxDuration(60),
			_speed(4),
			_durationType(DURATION_TYPE_MEDIAN),
			_frequencyType(FREQUENCY_TYPE_NO),
			_onlyWKT(false)
		{
		}



		ParametersMap IsochronService::_getParametersMap() const
		{
			ParametersMap map;
			
			if(dynamic_cast<NamedPlace*>(_startPlace.get()))
			{
				map.insert(PARAMETER_START_PLACE_NAME, dynamic_cast<NamedPlace*>(_startPlace.get())->getFullName());
			}
			else if(dynamic_cast<City*>(_startPlace.get()))
			{
				map.insert(PARAMETER_START_PLACE_NAME, dynamic_cast<City*>(_startPlace.get())->getName());
			}
			
			map.insert(PARAMETER_MAX_DISTANCE, lexical_cast<string>(_maxDistance));

			map.insert(PARAMETER_DATE, lexical_cast<string>(_date));
			map.insert(PARAMETER_BEGIN_TIME_SLOT, lexical_cast<string>(_beginTimeSlot));
			map.insert(PARAMETER_END_TIME_SLOT, lexical_cast<string>(_endTimeSlot));
			map.insert(PARAMETER_MAX_CONNECTIONS, lexical_cast<string>(_maxConnections));

			map.insert(PARAMETER_CURVES_STEP, lexical_cast<string>(_curvesStep));
			map.insert(PARAMETER_MAX_DURATION, lexical_cast<string>(_maxDuration));
			map.insert(PARAMETER_DURATION_TYPE, lexical_cast<string>(_durationType));
			map.insert(PARAMETER_FREQUENCY_TYPE, lexical_cast<string>(_frequencyType));

			map.insert(PARAMETER_ONLY_WKT, (_onlyWKT ? 1 : 0));

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

			return map;
		}



		void IsochronService::_setFromParametersMap(const ParametersMap& map)
		{
			// Set coordinate system if provided else 4326 (WGS84)
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, 4326)
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);	

			string coordinatesXY = map.getDefault<string>(PARAMETER_COORDINATES_XY);
			string startPlaceName = map.getDefault<string>(PARAMETER_START_PLACE_NAME);

			if(coordinatesXY.empty() && startPlaceName.empty())
			{
				throw RequestException("Neither start place name nor coordinates have been defined.");
			}

		    PlacesListService placesListService;
		    placesListService.setNumber(1);
		    placesListService.setCoordinatesSystem(_coordinatesSystem);

			if(!coordinatesXY.empty())
			{
				placesListService.addRequiredUserClass(USER_PEDESTRIAN);
				placesListService.setCoordinatesXY(coordinatesXY, false);
			}
			else if(!startPlaceName.empty())
			{
				placesListService.setText(startPlaceName);
			}

		    PlacesListService::PlaceResult placeResult = placesListService.getPlaceFromBestResult(placesListService.runWithoutOutput());

			_startPlace = placeResult.value;

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
			_onlyWKT = map.getDefault<bool>(PARAMETER_ONLY_WKT, false);

			AccessParameters::AllowedPathClasses allowedPathClasses;
			string rsStr(map.getDefault<string>(PARAMETER_ROLLING_STOCK_LIST));
			try
			{
				if(!rsStr.empty())
				{
					vector<string> rsVect;
					split(rsVect, rsStr, is_any_of(",; "));
					allowedPathClasses.insert(0);
					BOOST_FOREACH(string& rsItem, rsVect)
					{
						allowedPathClasses.insert(lexical_cast<RegistryKeyType>(rsItem));
					}
				}
			}
			catch(bad_lexical_cast&)
			{
				throw RequestException("Rolling Stock List is unreadable");	
			}

			AccessParameters::AllowedNetworks allowedNetworks;
			string nwlStr(map.getDefault<string>(PARAMETER_NETWORK_LIST));
			try
			{
				if(!nwlStr.empty())
				{
					vector<string> nwVect;
					split(nwVect, nwlStr, is_any_of(",; "));
					allowedNetworks.insert(0);
					BOOST_FOREACH(string& nwItem, nwVect)
					{
						allowedNetworks.insert(lexical_cast<RegistryKeyType>(nwItem));
					}		
				}
			}
			catch(bad_lexical_cast&)
			{
				throw RequestException("Network List is unreadable");
			}

			_accessParameters = AccessParameters(
						USER_PEDESTRIAN,
						false, false, 1000, posix_time::minutes(23), 1.111,
						_maxConnections,
						allowedPathClasses,
						allowedNetworks
					);

		}



		util::ParametersMap IsochronService::run(
			ostream& stream,
			const Request& request
		) const {
			ParametersMap result;

			if(_startPlace.get())
			{
				// Logger
				AlgorithmLogger logger;

				// VertexAccessMap definition
				VertexAccessMap originVam, ovam, dvam;

				set<graph::GraphIdType> graphTypes;
				graphTypes.insert(PTModule::GRAPH_ID);
				graphTypes.insert(RoadModule::GRAPH_ID);
				_startPlace->getVertexAccessMap(originVam, _accessParameters, graphTypes);

				ovam = _extendToPhysicalStops(
					originVam,
					dvam,
					DEPARTURE_TO_ARRIVAL,
					ptime(_date, time_duration(hours(_beginTimeSlot))),
					ptime(_date, time_duration(hours(_endTimeSlot))),
					ptime(_date, time_duration(hours(_beginTimeSlot))),
					ptime(_date, time_duration(hours(_endTimeSlot))),
					_accessParameters,
					logger
				);

				GraphIdType graphId = PTModule::GRAPH_ID;

				ResultsMap resultsMap;
				ResultsMapAccess resultsMapAccess;
				int nbMinutes = (_endTimeSlot - _beginTimeSlot) * 60;

				// Launch IntergralSearcher for each minute of the slot [_beginTimeSlot;_endTimeSlot]
				for(int minute=0; minute <= nbMinutes; ++minute)
				{
					BestVertexReachesMap bestVertexReachesMap(DEPARTURE_TO_ARRIVAL, ovam, dvam, Vertex::GetMaxIndex());
					const ptime minMaxDateTimeAtOrigin = ptime(_date, time_duration(hours(_beginTimeSlot) + minutes(minute)));
					ptime minMaxDateTimeAtDestination = ptime(_date, time_duration(hours(_endTimeSlot)));
					JourneysResult result(minMaxDateTimeAtOrigin, DEPARTURE_TO_ARRIVAL);

					// Initialization of the IntegralSearcher
					IntegralSearcher is(
						DEPARTURE_TO_ARRIVAL,
						_accessParameters,
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
					for(JourneysResult::ResultSet::const_iterator it(result.getJourneys().begin()) ;	it != result.getJourneys().end() ; it++)
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
					int step = 0;
					ParametersMap pm;
					pm.merge(getTemplateParameters());
					stringstream boardsStream;

					if(!_onlyWKT)
					{
						ParametersMap pmBoard;

						stringstream stopsStream;

						for(ResultsMap::const_iterator it(resultsMap.begin()) ; it != resultsMap.end() ; it++)
						{
							// On change d'interval de courbe
							if(((*it).second.duration >= (step + _curvesStep)))
							{
								int newstep = (*it).second.duration - ((*it).second.duration % _curvesStep);

								pmBoard.insert("duree_min", step);
								pmBoard.insert("duree_max", newstep - 1);
								pmBoard.insert("stops", stopsStream.str());

								if(_boardPage.get())
								{
									_boardPage->display(boardsStream, request, pmBoard);
								}

								pmBoard.clear();
								pmBoard.merge(getTemplateParameters());
								stopsStream.str("");
								step = newstep;
							}

							ParametersMap pmStop;
							pmStop.merge(getTemplateParameters());

							pmStop.insert("city_name", (*it).second.stop->getCity()->getName());
							pmStop.insert("stop_name", (*it).second.stop->getName());
							pmStop.insert("nb_solutions", (*it).second.nbSolutions);
							pmStop.insert("duration", (*it).second.duration);
							pmStop.insert("distance", (*it).second.distance);
							pmStop.insert("speed", ((*it).second.distance / ((float)(*it).second.duration / 60)));

							stringstream timesStream;
							BOOST_FOREACH(const ptime time, (*it).second.timeDepartureList)
							{
								ParametersMap pmTime;
								pmTime.merge(getTemplateParameters());
								pmTime.insert("time",time.time_of_day());

								if (_timePage.get())
								{
									_timePage->display(timesStream, request, pmTime);
								}
							}

							pmStop.insert("times", timesStream.str());

							if (_stopPage.get())
							{
								_stopPage->display(stopsStream, request, pmStop);
							}
						}
					}

					stringstream wktPointsStream;
					wktPointsStream << "[";
					bool first(true);
					for(ResultsMap::const_iterator it(resultsMap.begin()); it != resultsMap.end(); it++)
					{
						boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*(*it).second.stop->getPoint()
						));

						std::ostringstream streamPoint;

						if(!first)
							streamPoint << ",";
						else
							first = false;
						streamPoint << "[ ";
						streamPoint << wgs84Point->getY();
						streamPoint << ", ";
						streamPoint << wgs84Point->getX();
						streamPoint << ", ";
						streamPoint << ((*it).second.duration); // duration
						streamPoint << " ]";

						wktPointsStream << streamPoint.str();
					}

					wktPointsStream << "]";
					pm.insert("boards", boardsStream.str());
					pm.insert("wktPoints", wktPointsStream.str());

					_page->display(stream, request, pm);
				}
			}

			return result;
		}



		VertexAccessMap IsochronService::_extendToPhysicalStops(
			const VertexAccessMap& vam,
			const VertexAccessMap& destinationVam,
			PlanningPhase direction,
			const ptime& lowestDepartureTime,
			const ptime& lowestArrivalTime,
			const ptime& highestDepartureTime,
			const ptime& highestArrivalTime,
			const AccessParameters& ap,
			const AlgorithmLogger& logger
		) const {

			VertexAccessMap result;

			// Create origin vam from integral search on roads
			JourneysResult resultJourneys(
				direction == DEPARTURE_TO_ARRIVAL ?	lowestDepartureTime : highestArrivalTime ,
				direction
			);
			VertexAccessMap emptyMap;
			BestVertexReachesMap bvrmd(
				direction,
				vam,
				emptyMap,
				Vertex::GetMaxIndex()
			);

			ptime maxTime = (direction == DEPARTURE_TO_ARRIVAL ? highestArrivalTime : lowestDepartureTime);
			IntegralSearcher iso(
				direction,
				ap,
				PTModule::GRAPH_ID,
				false,
				RoadModule::GRAPH_ID,
				resultJourneys,
				bvrmd,
				destinationVam,
				direction == DEPARTURE_TO_ARRIVAL ? lowestDepartureTime : highestArrivalTime,
				direction == DEPARTURE_TO_ARRIVAL ? highestDepartureTime : lowestArrivalTime,
				maxTime,
				direction == DEPARTURE_TO_ARRIVAL ? false : true,
				false,
				ap.getMaxApproachTime(),
				ap.getApproachSpeed(),
				false,
				logger
			);
			iso.integralSearch(vam, optional<size_t>(), optional<posix_time::time_duration>());

			// Include physical stops from originVam into result of integral search
			// (cos not taken into account in returned journey vector).
			BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, vam.getMap())
			{
				const Vertex* vertex(itps.first);

				if(	vertex->getGraphType() == PTModule::GRAPH_ID)
				{
					result.insert(vertex, itps.second);
				}

				VertexAccessMap vam2;
				vertex->getHub()->getVertexAccessMap(
					vam2,
					PTModule::GRAPH_ID,
					*vertex,
					direction == DEPARTURE_TO_ARRIVAL
				);
				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& it, vam2.getMap())
				{
					result.insert(
						it.first,
						VertexAccess(
							itps.second.approachTime +
							(	direction == DEPARTURE_TO_ARRIVAL ?
								vertex->getHub()->getTransferDelay(*vertex, *it.first) :
								vertex->getHub()->getTransferDelay(*it.first, *vertex)
							),
							itps.second.approachDistance,
							itps.second.approachJourney
					)	);
				}
			}


			Journey candidate;
			BOOST_FOREACH(const JourneysResult::ResultSet::value_type& it, resultJourneys.getJourneys())
			{
				JourneysResult::ResultSet::key_type oj(it.first);

				// Store each reached physical stop with full approach time addition :
				//	- approach time in departure place
				//	- duration of the approach journey
				//	- transfer delay between approach journey end address and physical stop
				posix_time::time_duration commonApproachTime(
					vam.getVertexAccess(
						direction == DEPARTURE_TO_ARRIVAL ?
						oj->getOrigin()->getFromVertex() :
						oj->getDestination()->getFromVertex()
					).approachTime + minutes(static_cast<long>(ceil(oj->getDuration().total_seconds() / double(60))))
				);
				double commonApproachDistance(
					vam.getVertexAccess(
						direction == DEPARTURE_TO_ARRIVAL ?
						oj->getOrigin()->getFromVertex() :
						oj->getDestination()->getFromVertex()
					).approachDistance + oj->getDistance ()
				);
				VertexAccessMap vam2;
				const Hub* cp(
					(	direction == DEPARTURE_TO_ARRIVAL ?
						oj->getDestination() :
						oj->getOrigin()
					)->getHub()
				);
				const Vertex& v(
					*(	direction == DEPARTURE_TO_ARRIVAL ?
						oj->getDestination() :
						oj->getOrigin()
					)->getFromVertex()
				);
				cp->getVertexAccessMap(
					vam2,
					PTModule::GRAPH_ID,
					v,
					direction == DEPARTURE_TO_ARRIVAL
				);
				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& it, vam2.getMap())
				{
					result.insert(
						it.first,
						VertexAccess(
							commonApproachTime + (
								(&v == it.first) ?
								posix_time::seconds(0) :
								(
									direction == DEPARTURE_TO_ARRIVAL ?
									cp->getTransferDelay(v, *it.first) :
									cp->getTransferDelay(*it.first, v)
							)	),
							commonApproachDistance,
							*oj
					)	);
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
