//////////////////////////////////////////////////////////////////////////
/// IsochronAdmin class implementation.
///	@file IsochronAdmin.cpp
///	@author Gaël Sauvanet
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "IsochronAdmin.hpp"

#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "AccessParameters.h"
#include "AlgorithmLogger.hpp"
#include "AnalysisModule.hpp"
#include "Edge.h"
#include "IntegralSearcher.h"
#include "GlobalRight.h"
#include "ParametersMap.h"
#include "PlacesListFunction.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "PTModule.h"
#include "PTRoutePlannerResult.h"
#include "RequestException.h"
#include "RoadModule.h"
#include "SearchFormHTMLTable.h"
#include "User.h"
#include "Vertex.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <queue>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace algorithm;
	using namespace admin;
	using namespace graph;
	using namespace gregorian;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace analysis;
	using namespace pt_journey_planner;
	using namespace html;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, IsochronAdmin>::FACTORY_KEY("IsochronAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<IsochronAdmin>::ICON("map.png");
		template<> const string AdminInterfaceElementTemplate<IsochronAdmin>::DEFAULT_TITLE("Isochrones");
	}

	namespace analysis
	{
		const std::string IsochronAdmin::PARAMETER_START_PLACE("start_place");
		const std::string IsochronAdmin::PARAMETER_MAX_DISTANCE("max_distance");

		const std::string IsochronAdmin::PARAMETER_DATE("date");
		const std::string IsochronAdmin::PARAMETER_BEGIN_TIME_SLOT("begin_time_slot");
		const std::string IsochronAdmin::PARAMETER_END_TIME_SLOT("end_time_slot");
		const std::string IsochronAdmin::PARAMETER_MAX_CONNECTIONS("max_connections");

		const std::string IsochronAdmin::PARAMETER_CURVES_STEP("curves_step");
		const std::string IsochronAdmin::PARAMETER_MAX_DURATION("max_duration");
		const std::string IsochronAdmin::PARAMETER_DURATION_TYPE("duration_type");
		const std::string IsochronAdmin::PARAMETER_FREQUENCY_TYPE("frequency_type");
		const std::string IsochronAdmin::PARAMETER_SPEED("speed");

		IsochronAdmin::IsochronAdmin()
		:	AdminInterfaceElementTemplate<IsochronAdmin>(),
			_maxDistance(50),
			_date(day_clock::local_day()),
			_beginTimeSlot(7),
			_endTimeSlot(9),
			_maxConnections(1),
			_curvesStep(5),
			_maxDuration(60),
			_durationType(DURATION_TYPE_MEDIAN),
			_frequencyType(FREQUENCY_TYPE_NO),
			_speed(4)
		{ }



		void IsochronAdmin::setFromParametersMap(
			const ParametersMap& map
		){
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
			_maxDistance = map.getDefault<int>(PARAMETER_MAX_DISTANCE, 50);

			if(!map.getDefault<string>(PARAMETER_DATE).empty())
			{
				_date = gregorian::from_string(map.get<string>(PARAMETER_DATE));
			}
			_beginTimeSlot = map.getDefault<int>(PARAMETER_BEGIN_TIME_SLOT, 3);
			_endTimeSlot = map.getDefault<int>(PARAMETER_END_TIME_SLOT, 27);
			_maxConnections = map.getDefault<int>(PARAMETER_MAX_CONNECTIONS, 1);

			_curvesStep = map.getDefault<int>(PARAMETER_CURVES_STEP, 5);
			_maxDuration = map.getDefault<int>(PARAMETER_MAX_DURATION, 60);
			_durationType = map.getDefault<int>(PARAMETER_DURATION_TYPE, DURATION_TYPE_MEDIAN);
			_frequencyType = map.getDefault<int>(PARAMETER_FREQUENCY_TYPE, FREQUENCY_TYPE_NO);
			_speed = map.getDefault<int>(PARAMETER_SPEED, 4);
		}



		ParametersMap IsochronAdmin::getParametersMap() const
		{
			ParametersMap m;

			m.insert(PARAMETER_START_PLACE, (_startPlace.get() ? lexical_cast<string>(_startPlace->getKey()) : string()));
			m.insert(PARAMETER_MAX_DISTANCE, lexical_cast<string>(_maxDistance));

			m.insert(PARAMETER_DATE, lexical_cast<string>(_date));
			m.insert(PARAMETER_BEGIN_TIME_SLOT, lexical_cast<string>(_beginTimeSlot));
			m.insert(PARAMETER_END_TIME_SLOT, lexical_cast<string>(_endTimeSlot));
			m.insert(PARAMETER_MAX_CONNECTIONS, lexical_cast<string>(_maxConnections));

			m.insert(PARAMETER_CURVES_STEP, lexical_cast<string>(_curvesStep));
			m.insert(PARAMETER_MAX_DURATION, lexical_cast<string>(_maxDuration));
			m.insert(PARAMETER_DURATION_TYPE, lexical_cast<string>(_durationType));
			m.insert(PARAMETER_FREQUENCY_TYPE, lexical_cast<string>(_frequencyType));
			m.insert(PARAMETER_SPEED, lexical_cast<string>(_speed));

			return m;
		}



		bool IsochronAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void IsochronAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{
			stream << "<h1>Paramètres d'entrée</h1>";
			AdminFunctionRequest<IsochronAdmin> searchRequest(request, *this);
			PropertiesHTMLTable t(searchRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Paramètres du périmètre d'étude");
			stream << t.cell("Arrêt de départ", t.getForm().getTextInputAutoCompleteFromService(
					PARAMETER_START_PLACE,
					_startPlace.get() ? lexical_cast<string>(_startPlace->getKey()) : string(),
					_startPlace.get() ? lexical_cast<string>(_startPlace->getFullName()) : string(),
					pt_website::PlacesListFunction::FACTORY_KEY,
					pt_website::PlacesListFunction::DATA_PLACES,
					pt_website::PlacesListFunction::DATA_PLACE,
					"ct",string(),
					false, false, true
			)	);
			stream << t.cell("Distance maximale (km)", t.getForm().getSelectNumberInput(PARAMETER_MAX_DISTANCE, 0, 200, _maxDistance, 10));

			stream << t.title("Paramètres du calcul d'itinéraire");
			stream << t.cell("Date du calcul", t.getForm().getCalendarInput(PARAMETER_DATE, _date));
			stream << t.cell("Début tranche horaire (heure)", t.getForm().getSelectNumberInput(PARAMETER_BEGIN_TIME_SLOT, 0, 26, _beginTimeSlot, 1));
			stream << t.cell("Fin tranche horaire (heure)", t.getForm().getSelectNumberInput(PARAMETER_END_TIME_SLOT, 1, 27, _endTimeSlot, 1));
			stream << t.cell("Nombre de correspondances maximales", t.getForm().getSelectNumberInput(PARAMETER_MAX_CONNECTIONS, 0, 10, _maxConnections, 1));

			stream << t.title("Paramètres de l'extraction des données");
			stream << t.cell("Pas des courbes (min)", t.getForm().getSelectNumberInput(PARAMETER_CURVES_STEP, 1, 60, _curvesStep, 1));
			stream << t.cell("Durée maximale (min)", t.getForm().getSelectNumberInput(PARAMETER_MAX_DURATION, 10, 300, _maxDuration, 10));

			std::vector<std::pair<boost::optional<DurationType>, std::string> > durationTypesList;
			//durationTypesList.push_back(make_pair(static_cast<DurationType>(DURATION_TYPE_FIXED_DATETIME), "Date/Heure fixée"));
			durationTypesList.push_back(make_pair(boost::optional<DurationType>(DURATION_TYPE_BEST), "Meilleure"));
			//durationTypesList.push_back(make_pair(DURATION_TYPE_AVERAGE, "Moyenne"));
			//durationTypesList.push_back(make_pair(DURATION_TYPE_MEDIAN, "Médiane"));
			//durationTypesList.push_back(make_pair(DURATION_TYPE_WORST, "Pire"));

			stream << t.cell(
				"Durée de trajet prise en compte",
				t.getForm().getSelectInput(
					PARAMETER_DURATION_TYPE,
					durationTypesList,
					optional<DurationType>(static_cast<DurationType>(_durationType))
			)	);
/*
			std::vector<std::pair<boost::optional<FrequencyType>, std::string> > frequencyTypesList;
			frequencyTypesList.push_back(make_pair(FREQUENCY_TYPE_NO, "non"));
			frequencyTypesList.push_back(make_pair(FREQUENCY_TYPE_HALF_FREQUENCY_AVERAGE, "Demi-fréquence moyenne"));
			frequencyTypesList.push_back(make_pair(FREQUENCY_TYPE_HALF_FREQUENCY_MEDIAN, "Demi-fréquence médiane"));
			frequencyTypesList.push_back(make_pair(FREQUENCY_TYPE_HALF_FREQUENCY_WORST, "Demi-pire fréquence"));
			frequencyTypesList.push_back(make_pair(FREQUENCY_TYPE_AVERAGE, "fréquence moyenne"));
			frequencyTypesList.push_back(make_pair(FREQUENCY_TYPE_MEDIAN, "fréquence médiane"));
			frequencyTypesList.push_back(make_pair(FREQUENCY_TYPE_WORST, "pire fréquence"));
			stream << t.cell(
				"Prise en compte de la fréquence",
				t.getForm().getSelectInput(
					PARAMETER_FREQUENCY_TYPE,
					frequencyTypesList,
					optional<FrequencyType>(_frequencyType)
			)	);
*/
			stream << t.cell("Vitesse approche (par défaut: 4km/h)", t.getForm().getSelectNumberInput(PARAMETER_SPEED, 1, 60, _speed, 1));

			stream << t.close();

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
					ptime minMaxDateTimeAtDestination = ptime(_date,time_duration(hours(_endTimeSlot)));
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
								bestResultsMap.insert(pair<RegistryKeyType,JourneysResult::ResultSet::const_iterator>(reachedPlace->getKey(),it));
							}
						}
						else
						{
							bestResultsMap.insert(pair<RegistryKeyType,JourneysResult::ResultSet::const_iterator>(reachedPlace->getKey(),it));
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

						if(resultsMapAccess.count(reachedPlace->getKey()) == 1)	// Update a result in ResultsMap
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
						else	// Add new a result in ResultsMap
						{
							StopStruct stop;
							stop.stop = reachedPlace;
							stop.nbSolutions = 1;
							stop.duration = duration;
							stop.distance = distance;
							stop.lastDepartureTime = journey.getFirstDepartureTime();
							stop.timeDepartureList.push_back(journey.getFirstDepartureTime(false));
							ResultsMap::iterator itMap = resultsMap.insert(pair<int,StopStruct>(duration,stop));
							resultsMapAccess.insert(pair<RegistryKeyType,ResultsMap::iterator>(reachedPlace->getKey(),itMap));
						}
					}
				}
				stream << "<h1>Résultats</h1>";

				// Draw OpenLayers map
				stream << "<div id=\"map\" style=\"height:600px; width:800px;\" class=\"olMap\"></div>";
//				stream << "<!--[if IE]><script src=\"/map/js/excanvas.js\"></script><![endif]-->";
				stream << HTMLModule::GetHTMLJavascriptOpen("/lib/openlayers/OpenLayers.js");
				stream << "<script src=\"/admin/IsochronLayer.js\" type=\"text/javascript\"></script>";
				stream << "<script type=\"text/javascript\">";
				stream << "var map;" << endl;

				stream << "map = new OpenLayers.Map('map', {projection: new OpenLayers.Projection(\"EPSG:900913\"), displayProjection: new OpenLayers.Projection(\"EPSG:4326\"), ";
				stream << "controls: [new OpenLayers.Control.Navigation(), new OpenLayers.Control.PanZoomBar(), new OpenLayers.Control.LayerSwitcher({'ascending':false}), new OpenLayers.Control.MousePosition()]}); ";

				stream << "var isochron = new Isochron.Layer(\"Isochrone\"); ";

				stream << "var stops = [";
				bool first = true;
				for(ResultsMap::const_iterator it(resultsMap.begin()); it != resultsMap.end(); it++)
				{
					if(first)
					{
						first = false;
					}
					else
					{
						stream << ",";
					}
					boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
						*(*it).second.stop->getPoint()
					)	);
					stream << "[ ";
					stream << wgs84Point->getY();
					stream << ", ";
					stream << wgs84Point->getX();
					stream << ", ";
					stream << ((*it).second.duration);	// duration
					stream << " ]";
				}
				stream << "];";

				stream << "for (var stop in stops) {";
				stream << "var coord = new OpenLayers.LonLat(stops[stop][1], stops[stop][0]);";
				stream << "var coord2 = new Isochron.Stop(coord.transform(new OpenLayers.Projection(\"EPSG:4326\"),new OpenLayers.Projection(\"EPSG:900913\")), stops[stop][2]);";
				stream << "isochron.addStop(coord2);";
				stream << "}";

				stream << "isochron.setOpacity(0.7);";
				stream << "isochron.setSpeed(" << _speed << ");";
				stream << "isochron.initSteps(" << _curvesStep << "," << _maxDuration << ",'#303030','#c0c0c0');";
				stream << "var osm = new OpenLayers.Layer.OSM();";

				stream << "map.addLayers([osm, isochron]);";
				stream << "map.zoomToExtent(isochron.getDataExtent());";
				stream << endl;
				stream << "</script>";

				// Display results table
				HTMLTable::ColsVector vs;
				vs.push_back("Arrêt");
				vs.push_back("Nombre de solution (départs)");
				vs.push_back("Durée (min)");
				vs.push_back("Distance à vol d'oiseau (km)");
				vs.push_back("Vitesse (km/h)");
				HTMLTable tc(vs, ResultHTMLTable::CSS_CLASS);
				stream << tc.open();
				int step = -_curvesStep;
				for(ResultsMap::const_iterator it(resultsMap.begin());
					it != resultsMap.end(); it++
				){
					if((*it).second.duration >= (step + _curvesStep))
					{
						step = (*it).second.duration - ((*it).second.duration % _curvesStep);
						stream << tc.row();
						stream << tc.col(5, string(), true);
						stream << "Arrêts atteignables en " << step;
						stream << " à " << (step + _curvesStep - 1) << " min";
					}
					stream << tc.row();
					stream << tc.col() << (*it).second.stop->getCity()->getName() << " " << (*it).second.stop->getName();
					stream << tc.col() << (*it).second.nbSolutions;
					stream << " (";
					BOOST_FOREACH(const ptime time, (*it).second.timeDepartureList)
					{
						stream << " " << time.time_of_day();
					}
					stream << " )";
					stream << tc.col() << (*it).second.duration;
					stream << tc.col() << (*it).second.distance;
					stream << tc.col() << ((*it).second.distance / ((float)(*it).second.duration / 60));
				}
				stream << tc.close();
			}
		}



		AdminInterfaceElement::PageLinks IsochronAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const AnalysisModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}
	}
}
