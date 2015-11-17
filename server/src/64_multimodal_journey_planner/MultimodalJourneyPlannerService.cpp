
/** MultimodalJourneyPlannerService class implementation.
	@file MultimodalJourneyPlannerService.cpp

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

#include "MultimodalJourneyPlannerService.hpp"

#include "MultimodalJourneyPlannerResult.h"
#include "NamedPlace.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "Request.h"
#include "RequestException.h"
#include "Session.h"

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace geography;
	using namespace util;

	template<> const string util::FactorableTemplate<multimodal_journey_planner::MultimodalJourneyPlannerService::_FunctionWithSite,multimodal_journey_planner::MultimodalJourneyPlannerService>::FACTORY_KEY("multimodal_journey_planner");

	namespace multimodal_journey_planner
	{
		const string MultimodalJourneyPlannerService::PARAMETER_DEPARTURE_CITY_TEXT = "departure_city";
		const string MultimodalJourneyPlannerService::PARAMETER_ARRIVAL_CITY_TEXT = "arrival_city";
		const string MultimodalJourneyPlannerService::PARAMETER_DEPARTURE_PLACE_TEXT = "departure_place";
		const string MultimodalJourneyPlannerService::PARAMETER_ARRIVAL_PLACE_TEXT = "arrival_place";
		const string MultimodalJourneyPlannerService::PARAMETER_DEPARTURE_DAY = "departure_date";
		const string MultimodalJourneyPlannerService::PARAMETER_DEPARTURE_TIME = "departure_time";


		const string MultimodalJourneyPlannerService::DATA_MULTIMODAL_JOURNEY_PLANNER_RESULT = "MultimodalJourneyPlannerResult";


		MultimodalJourneyPlannerService::MultimodalJourneyPlannerService(
		):	_day(boost::gregorian::day_clock::local_day()),
			_departureTime(not_a_date_time)
		{}



		ParametersMap MultimodalJourneyPlannerService::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSiteBase::_getParametersMap());

			// Departure place
			if(_departure_place.placeResult.value.get())
			{
				if(dynamic_cast<NamedPlace*>(_departure_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_DEPARTURE_PLACE_TEXT,
						dynamic_cast<NamedPlace*>(_departure_place.placeResult.value.get())->getFullName()
					);
				}
				else if(dynamic_cast<City*>(_departure_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_DEPARTURE_PLACE_TEXT,
						dynamic_cast<City*>(_departure_place.placeResult.value.get())->getName()
					);
				}
			}

			// Arrival place
			if(_arrival_place.placeResult.value.get())
			{
				if(dynamic_cast<NamedPlace*>(_arrival_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_ARRIVAL_PLACE_TEXT,
						dynamic_cast<NamedPlace*>(_arrival_place.placeResult.value.get())->getFullName()
					);
				}
				else if(dynamic_cast<City*>(_arrival_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_ARRIVAL_PLACE_TEXT,
						dynamic_cast<City*>(_arrival_place.placeResult.value.get())->getName()
					);
				}
			}

			return map;
		}



		void MultimodalJourneyPlannerService::_setFromParametersMap(const ParametersMap& map)
		{
			_FunctionWithSite::_setFromParametersMap(map);

			// Departure place
			if( // Two fields input
				map.isDefined(PARAMETER_DEPARTURE_CITY_TEXT) &&
				map.isDefined(PARAMETER_DEPARTURE_PLACE_TEXT)
			){
				/*_originCityText = map.getDefault<string>(PARAMETER_DEPARTURE_CITY_TEXT);
				_originPlaceText = map.getDefault<string>(PARAMETER_DEPARTURE_PLACE_TEXT);
				if(!_originCityText.empty() || !_originPlaceText.empty())
				{
					if(_originCityText.empty())
					{
						road::RoadModule::ExtendedFetchPlacesResult results(pt::PTModule::ExtendedFetchPlaces(_originPlaceText, 1));
						if(!results.empty())
						{
							_departure_place = *results.begin();
						}
					}
					else
					{
						_departure_place = road::RoadModule::ExtendedFetchPlace(_originCityText, _originPlaceText);
					}
				}*/
			}
			// One field input
			else if(map.isDefined(PARAMETER_DEPARTURE_PLACE_TEXT))
			{
				// TO-DO after smile-1
			}

			// Destination
			if( // Two fields input
				map.isDefined(PARAMETER_ARRIVAL_CITY_TEXT) &&
				map.isDefined(PARAMETER_ARRIVAL_PLACE_TEXT)
			){
				/*_destinationCityText = map.getDefault<string>(PARAMETER_ARRIVAL_CITY_TEXT);
				_destinationPlaceText = map.getDefault<string>(PARAMETER_ARRIVAL_PLACE_TEXT);
				if(!_destinationCityText.empty() || !_destinationPlaceText.empty())
				{
					if(_destinationCityText.empty())
					{
						road::RoadModule::ExtendedFetchPlacesResult results(pt::PTModule::ExtendedFetchPlaces(_destinationPlaceText, 1));
						if(!results.empty())
						{
							_arrival_place = *results.begin();
						}
					}
					else
					{
						_arrival_place = road::RoadModule::ExtendedFetchPlace(_destinationCityText, _destinationPlaceText);
					}
				}*/
			}
			// One field input
			else if(map.isDefined(PARAMETER_ARRIVAL_PLACE_TEXT))
			{
				// TO-DO after smile-1
			}

			// Date parameters
			try
			{
				// by day and time
				if(!map.getDefault<string>(PARAMETER_DEPARTURE_DAY).empty())
				{
					// Day
					_day = from_string(map.get<string>(PARAMETER_DEPARTURE_DAY));

					// Time
					if(!map.getDefault<string>(PARAMETER_DEPARTURE_TIME).empty())
					{
						_departureTime = duration_from_string(map.get<string>(PARAMETER_DEPARTURE_TIME));
					}
				}
			}
			catch(ParametersMap::MissingParameterException& e)
			{
				throw server::RequestException(e.what());
			}

			Function::setOutputFormatFromMap(map,string());
		}



		util::ParametersMap MultimodalJourneyPlannerService::run(
			ostream& stream,
			const server::Request& request
		) const	{

			//////////////////////////////////////////////////////////////////////////
			// Display
			ParametersMap pm;

			// Hardcoded result
			string strFakeDepartureTime("2016-01-01 11:00");
			string strFakeArrivalTime("2016-01-01 13:00");
			string strFakeDeparturePlace("Lausanne, place du départ");
			string strFakeDeparturePlaceId("1");
			string strFakePlaceType("RoadPlace");
			string strFakeArrivalPlace("Lausanne, place de l'arrivée");
			string strFakeArrivalPlaceId("2");
			string strFakeLineString("LINESTRING(46.5320 6.5970,46.4974 6.6842)");
			string strFakeRoadName("Mega pont imaginaire");
			string strFakeRoadId("1");
			boost::shared_ptr<ParametersMap> submapFakeJourney(new ParametersMap);
			submapFakeJourney->insert("departure_date_time", strFakeDepartureTime);
			submapFakeJourney->insert("arrival_date_time", strFakeArrivalTime);

			boost::shared_ptr<ParametersMap> submapFakeDeparturePlace(new ParametersMap);
			submapFakeDeparturePlace->insert("name", strFakeDeparturePlace);
			submapFakeDeparturePlace->insert("type", strFakePlaceType);
			submapFakeDeparturePlace->insert("id", strFakeDeparturePlaceId);
			submapFakeDeparturePlace->insert("latitude", 6.5970);
			submapFakeDeparturePlace->insert("longitude", 46.5320);
			boost::shared_ptr<ParametersMap> submapFakeArrivalPlace(new ParametersMap);
			submapFakeArrivalPlace->insert("name", strFakeArrivalPlace);
			submapFakeArrivalPlace->insert("type", strFakePlaceType);
			submapFakeArrivalPlace->insert("id", strFakeArrivalPlaceId);
			submapFakeArrivalPlace->insert("latitude", 6.6842);
			submapFakeArrivalPlace->insert("longitude", 46.4974);

			boost::shared_ptr<ParametersMap> submapFakeLeg(new ParametersMap);
			submapFakeLeg->insert("departure_date_time", strFakeDepartureTime);
			submapFakeLeg->insert("arrival_date_time", strFakeArrivalTime);
			submapFakeLeg->insert("geometry", strFakeLineString);
			submapFakeLeg->insert("length", 4000);

			boost::shared_ptr<ParametersMap> submapFakeLegDeparturePlace(new ParametersMap);
			submapFakeLegDeparturePlace->insert("name", strFakeDeparturePlace);
			submapFakeLegDeparturePlace->insert("type", strFakePlaceType);
			submapFakeLegDeparturePlace->insert("id", strFakeDeparturePlaceId);
			submapFakeLegDeparturePlace->insert("latitude", 6.5970);
			submapFakeLegDeparturePlace->insert("longitude", 46.5320);
			boost::shared_ptr<ParametersMap> submapFakeLegArrivalPlace(new ParametersMap);
			submapFakeLegArrivalPlace->insert("name", strFakeArrivalPlace);
			submapFakeLegArrivalPlace->insert("type", strFakePlaceType);
			submapFakeLegArrivalPlace->insert("id", strFakeArrivalPlaceId);
			submapFakeLegArrivalPlace->insert("latitude", 6.6842);
			submapFakeLegArrivalPlace->insert("longitude", 46.4974);

			submapFakeLeg->insert("departure", submapFakeLegDeparturePlace);
			submapFakeLeg->insert("arrival", submapFakeLegArrivalPlace);

			boost::shared_ptr<ParametersMap> submapFakeLegRoad(new ParametersMap);
			submapFakeLegRoad->insert("name", strFakeRoadName);
			submapFakeLegRoad->insert("id", strFakeRoadId);

			submapFakeLeg->insert("road", submapFakeLegRoad);


			submapFakeJourney->insert("departure", submapFakeDeparturePlace);
			submapFakeJourney->insert("arrival", submapFakeArrivalPlace);
			submapFakeJourney->insert("leg", submapFakeLeg);

			pm.insert("journey", submapFakeJourney);

			if(_outputFormat == MimeTypes::XML)
			{
				pm.outputXML(
					stream,
					DATA_MULTIMODAL_JOURNEY_PLANNER_RESULT,
					true,
					"http://schemas.open-transport.org/smile/MultimodalJourneyPlanner.xsd"
				);
			}
			else if(_outputFormat == MimeTypes::JSON)
			{
				pm.outputJSON(stream, DATA_MULTIMODAL_JOURNEY_PLANNER_RESULT);
			}

			return pm;
		}



		bool MultimodalJourneyPlannerService::isAuthorized(
			const server::Session* session
		) const {
			return true;
		}



		std::string MultimodalJourneyPlannerService::getOutputMimeType() const
		{
			return _outputFormat;
		}
}	}
