
//////////////////////////////////////////////////////////////////////////////////////////
///	FreeDRTAvailableSlotsService class implementation.
///	@file FreeDRTAvailableSlotsService.cpp
///	@author Hugues Romain
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

#include "FreeDRTAvailableSlotsService.hpp"

#include "FreeDRTArea.hpp"
#include "FreeDRTTimeSlot.hpp"
#include "PlacesListService.hpp"
#include "Profile.h"
#include "ResaRight.h"
#include "RequestException.h"
#include "Request.h"
#include "Session.h"
#include "User.h"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace geography;
	using namespace graph;
	using namespace pt;
	using namespace pt_website;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,resa::FreeDRTAvailableSlotsService>::FACTORY_KEY = "free_drt_available_slots";
	
	namespace resa
	{
		const string FreeDRTAvailableSlotsService::PARAMETER_AREA_ID = "area_id";
		const string FreeDRTAvailableSlotsService::PARAMETER_ARRIVAL_PLACE = "arrival_place";
		const string FreeDRTAvailableSlotsService::PARAMETER_DEPARTURE_PLACE = "departure_place";
		const string FreeDRTAvailableSlotsService::PARAMETER_MAX_DATE_TIME = "max_date_time";
		const string FreeDRTAvailableSlotsService::PARAMETER_MIN_DATE_TIME = "min_date_time";

		const string FreeDRTAvailableSlotsService::ITEM_SERVICE = "service";
		
		const string FreeDRTAvailableSlotsService::ITEM_SLOT = "slot";
		const string FreeDRTAvailableSlotsService::ATTR_ARRIVAL_TIME = "arrival_time";
		const string FreeDRTAvailableSlotsService::ATTR_DEPARTURE_TIME = "departure_time";
	


		ParametersMap FreeDRTAvailableSlotsService::_getParametersMap() const
		{
			ParametersMap map;

			// Area id
			if(_area.get())
			{
				map.insert(PARAMETER_AREA_ID, _area->getKey());
			}
			return map;
		}



		void FreeDRTAvailableSlotsService::_setFromParametersMap(const ParametersMap& map)
		{
			// Area
			try
			{
				_area = Env::GetOfficialEnv().get<FreeDRTArea>(
					map.get<RegistryKeyType>(PARAMETER_AREA_ID)
				);
			}
			catch(ObjectNotFoundException<FreeDRTArea>&)
			{
				throw RequestException("No such area");
			}

			// Departure place
			PlacesListService placesListService;
			placesListService.setNumber(1);
			placesListService.setText(map.getDefault<string>(PARAMETER_DEPARTURE_PLACE));
			placesListService.setCoordinatesSystem(&CoordinatesSystem::GetInstanceCoordinatesSystem());
			_departurePlace = dynamic_pointer_cast<NamedPlace, Place>(
				placesListService.getPlaceFromBestResult(
					placesListService.runWithoutOutput()
				).value
			);

			// Arrival place
			placesListService.setText(map.getDefault<string>(PARAMETER_ARRIVAL_PLACE));
			_arrivalPlace = dynamic_pointer_cast<NamedPlace, Place>(
				placesListService.getPlaceFromBestResult(
					placesListService.runWithoutOutput()
				).value
			);

			/// TODO check if the places belong to the area

			// Date and time
			_minDateTime = time_from_string(
				map.get<string>(PARAMETER_MIN_DATE_TIME)
			);
			_maxDateTime = time_from_string(
				map.get<string>(PARAMETER_MAX_DATE_TIME)
			);
		}



		ParametersMap FreeDRTAvailableSlotsService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap result;

			// Declarations
			typedef map<
				ptime,
				ptime
			> Results;
			Results results;

			// Best journey time at min speed
			double dst(
				_arrivalPlace->getPoint()->distance(
					_departurePlace->getPoint().get()
			)	);

			// Time slots
			BOOST_FOREACH(const Service* itServ, _area->getServices())
			{
				// Declarations
				const FreeDRTTimeSlot& timeSlot(static_cast<const FreeDRTTimeSlot&>(*itServ));
				boost::shared_ptr<ParametersMap> servicePM;

				// Calendar check
				// TODO handle time slots with date change
				if(!timeSlot.isActive(_minDateTime.date()))
				{
					continue;
				}

				// Journey time
				// TODO replace by legacy journey planner
				time_duration bestCommercialJourneyTime(
					minutes(long(0.06 * dst / timeSlot.getCommercialSpeed()))
				);

				// Time bounds
				ptime lowerBound(_minDateTime.date(), timeSlot.getFirstDeparture());
				if(lowerBound < _minDateTime)
				{
					lowerBound = _minDateTime;
				}
				ptime upperBound(_minDateTime.date(), timeSlot.getLastArrival());
				upperBound -= bestCommercialJourneyTime;
				if(upperBound > _maxDateTime)
				{
					upperBound = _maxDateTime;
				}

				// Reservations
				/// TODO

				// Time loop
				results.clear();
				for(ptime curTime(lowerBound);
					curTime <= upperBound;
					curTime = curTime + minutes(5)
				){
					// Reservations check
					/// TODO

					// Storage of the service map
					if(!servicePM.get())
					{
						servicePM.reset(new ParametersMap);
					}

					// Storage of the slot
					boost::shared_ptr<ParametersMap> slotPM(new ParametersMap);
					slotPM->insert(ATTR_DEPARTURE_TIME, curTime);
					slotPM->insert(ATTR_ARRIVAL_TIME, curTime + bestCommercialJourneyTime);
					servicePM->insert(ITEM_SLOT, slotPM);
				}

				// Service PM storage
				if(servicePM.get())
				{
					result.insert(ITEM_SERVICE, servicePM);
				}
			}

			return result;
		}
		
		
		
		bool FreeDRTAvailableSlotsService::isAuthorized(
			const Session* session
		) const {
			return
				session &&
				session->hasProfile() &&
				session->getUser()->getProfile()->isAuthorized<ResaRight>(
					READ,
					UNKNOWN_RIGHT_LEVEL,
					lexical_cast<string>(_area->getLine()->getKey())
				)
			;
		}



		std::string FreeDRTAvailableSlotsService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
