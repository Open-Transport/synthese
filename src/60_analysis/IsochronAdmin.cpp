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
#include "GlobalRight.h"
#include "IntegralSearcher.h"
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

#include <boost/date_time/posix_time/posix_time.hpp>
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
			_beginTimeSlot(3),
			_endTimeSlot(27),
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
			const admin::AdminRequest& request
		) const	{
			stream << "<h1>Paramètres d'entrée</h1>";
			AdminFunctionRequest<IsochronAdmin> searchRequest(request);
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
			stream << t.cell("Distance maximale", t.getForm().getSelectNumberInput(PARAMETER_MAX_DISTANCE, 0, 200, _maxDistance, 10));

			stream << t.title("Paramètres du calcul d'itinéraire");
			stream << t.cell("Date du calcul", t.getForm().getCalendarInput(PARAMETER_DATE, _date));
			stream << t.cell("Début tranche horaire (heure)", t.getForm().getSelectNumberInput(PARAMETER_BEGIN_TIME_SLOT, 0, 26, _beginTimeSlot, 1));
			stream << t.cell("Fin tranche horaire (heure)", t.getForm().getSelectNumberInput(PARAMETER_END_TIME_SLOT, 1, 27, _endTimeSlot, 1));
			stream << t.cell("Nombre de correspondances maximales", t.getForm().getSelectNumberInput(PARAMETER_MAX_CONNECTIONS, 0, 10, _maxConnections, 1));

			stream << t.title("Paramètres de l'extraction des données");
			stream << t.cell("Pas des courbes (min)", t.getForm().getSelectNumberInput(PARAMETER_CURVES_STEP, 1, 60, _curvesStep, 1));
			stream << t.cell("Durée maximale (min)", t.getForm().getSelectNumberInput(PARAMETER_MAX_DURATION, 10, 300, _maxDuration, 10));
/*
			std::vector<std::pair<boost::optional<DurationType>, std::string> > durationTypesList;
			durationTypesList.push_back(make_pair(DURATION_TYPE_FIXED_DATETIME, "Date/Heure fixée"));
			durationTypesList.push_back(make_pair(DURATION_TYPE_BEST, "Meilleure"));
			durationTypesList.push_back(make_pair(DURATION_TYPE_AVERAGE, "Moyenne"));
			durationTypesList.push_back(make_pair(DURATION_TYPE_MEDIAN, "Médiane"));
			durationTypesList.push_back(make_pair(DURATION_TYPE_WORST, "Pire"));
			stream << t.cell(
				"Durée de trajet prise en compte",
				t.getForm().getSelectInput(
					PARAMETER_DURATION_TYPE,
					durationTypesList,
					optional<DurationType>(_durationType)
			)	);
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
			stream << t.cell("Vitesse approche (par défaut: 4km/h)", t.getForm().getSelectNumberInput(PARAMETER_SPEED, 1, 60, _speed, 1));
*/
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

				const ptime minMaxDateTimeAtOrigin = ptime(_date,time_duration(hours(_beginTimeSlot)));
				ptime minMaxDateTimeAtDestination = ptime(_date,time_duration(hours(_endTimeSlot)));

				JourneysResult result(minMaxDateTimeAtOrigin, DEPARTURE_TO_ARRIVAL);
				BestVertexReachesMap bestVertexReachesMap(DEPARTURE_TO_ARRIVAL, ovam, dvam, Vertex::GetMaxIndex());
				GraphIdType graphId = PTModule::GRAPH_ID;

				// Initialization of the integral searcher
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

				is.integralSearch(
					ovam,
					optional<size_t>(_maxConnections),
					optional<posix_time::time_duration>()
				);

				typedef multimap<int, StopStruct> ResultsMap;
				ResultsMap resultsMap;

				for(JourneysResult::ResultSet::const_iterator it(result.getJourneys().begin());
					it != result.getJourneys().end(); it++
				){
					const RoutePlanningIntermediateJourney& journey(*it->first);
					const Vertex* reachedVertex(journey.getEndEdge().getFromVertex());
					if(!reachedVertex || !dynamic_cast<const StopArea*>(reachedVertex->getHub()))
					{
						continue;
					}
					const StopArea* reachedPlace = dynamic_cast<const StopArea*>(reachedVertex->getHub());
					if(!reachedPlace->getPoint())
					{
						continue;
					}

					int distance = (int) (_startPlace->getPoint()->distance(reachedPlace->getPoint().get()) / 1000);
					int duration = journey.getEffectiveDuration().hours() * 60 + journey.getEffectiveDuration().minutes();

					if((distance > _maxDistance) || (duration > _maxDuration))
					{
						continue;
					}

					StopStruct stop;
					stop.stop = reachedPlace;
					stop.nbSolutions = 1;
					stop.duration = duration;
					stop.distance = distance;

					resultsMap.insert(pair<int,StopStruct>(duration,stop));
				}

				stream << "<h1>Résultats</h1>";
				HTMLTable::ColsVector vs;
				vs.push_back("Commune");
				vs.push_back("Nom arrêt");
				vs.push_back("Durée (min)");
				vs.push_back("Distance à vol d'oiseau (km)");
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
						stream << tc.col(4, string(), true);
						stream << "Arrêts atteignables en " << step;
						stream << " à " << (step + _curvesStep - 1) << " min";
					}
					stream << tc.row();
					stream << tc.col() << (*it).second.stop->getCity()->getName();
					stream << tc.col() << (*it).second.stop->getName();
					stream << tc.col() << (*it).second.duration;
					stream << tc.col() << (*it).second.distance;
				}
				stream << tc.close();
			}
		}



		AdminInterfaceElement::PageLinks IsochronAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
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
