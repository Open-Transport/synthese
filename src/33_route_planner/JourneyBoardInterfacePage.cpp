
/** JourneyBoardInterfacePage class implementation.
	@file JourneyBoardInterfacePage.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "JourneyBoardInterfacePage.h"

#include "OnlineReservationRule.h"
#include "NamedPlace.h"
#include "Journey.h"
#include "Edge.h"
#include "AddressablePlace.h"
#include "ReservationContact.h"
#include "Service.h"
#include "Crossing.h"
#include "Line.h"
#include "DurationInterfacePage.h"
#include "DateTimeInterfacePage.h"
#include "Interface.h"
#include "CommercialLine.h"
#include "Conversion.h"
#include "Env.h"
#include "GeoPoint.h"
#include "Projection.h"

#include <sstream>
#include <set>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace routeplanner;
	using namespace util;
	using namespace env;
	using namespace time;
	using namespace resa;
	using namespace graph;
	using namespace road;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, JourneyBoardInterfacePage>::FACTORY_KEY("journey_board");
	}

	namespace routeplanner
	{
		const string JourneyBoardInterfacePage::DATA_RANK("rank");
		const string JourneyBoardInterfacePage::DATA_HANDICAPPED_FILTER("handicapped_filter");
		const string JourneyBoardInterfacePage::DATA_BIKE_FILTER("bike_filter");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_TIME("departure_time");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_PLACE_NAME("departure_place_name");
		const string JourneyBoardInterfacePage::DATA_ARRIVAL_TIME("arrival_time");
		const string JourneyBoardInterfacePage::DATA_ARRIVAL_PLACE_NAME("arrival_place_name");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_DATE("departure_date");
		const string JourneyBoardInterfacePage::DATA_RESERVATION_AVAILABLE("reservation_available");
		const string JourneyBoardInterfacePage::DATA_RESERVATION_COMPULSORY("reservation_compulsory");
		const string JourneyBoardInterfacePage::DATA_RESERVATION_DELAY("reservation_delay");
		const string JourneyBoardInterfacePage::DATA_RESERVATION_DEADLINE("reservation_deadline");
		const string JourneyBoardInterfacePage::DATA_RESERVATION_PHONE_NUMBER("reservation_phone_number");
		const string JourneyBoardInterfacePage::DATA_ONLINE_RESERVATION("online_reservation");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_TIME_INTERNAL_FORMAT("internal_departure_time");
		const string JourneyBoardInterfacePage::DATA_IS_THE_LAST_JOURNEY_BOARD("is_the_last_journey_board");
		const string JourneyBoardInterfacePage::DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_TIME("continuous_service_last_departure_time");
		const string JourneyBoardInterfacePage::DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_TIME("continuous_service_last_arrival_time");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_PLACE_LONGITUDE("departure_longitude");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_PLACE_LATITUDE("departure_latitude");
		const string JourneyBoardInterfacePage::DATA_ARRIVAL_PLACE_LONGITUDE("arrival_longitude");
		const string JourneyBoardInterfacePage::DATA_ARRIVAL_PLACE_LATITUDE("arrival_latitude");

		void JourneyBoardInterfacePage::display(
			std::ostream& stream
			, interfaces::VariablesMap& variables
			, int n
			, const Journey* journey
			, const Place* departurePlace
			, const Place* arrivalPlace
			, logic::tribool handicappedFilter
			, logic::tribool bikeFilter
			, bool isTheLast
			, const server::Request* request
		) const {
			const DurationInterfacePage* durationInterfacePage = getInterface()->getPage<DurationInterfacePage>();
			stringstream sDuration;
			durationInterfacePage->display(sDuration, journey->getDuration(), variables, NULL, request);
			const DateTimeInterfacePage* dateInterfacePage = getInterface()->getPage<DateTimeInterfacePage>();
			stringstream sDate;
			dateInterfacePage->display(sDate, variables, journey->getDepartureTime().getDate(), request);
			DateTime now(TIME_CURRENT);
			DateTime resaDeadLine(journey->getReservationDeadLine());
			logic::tribool resaCompliance(journey->getReservationCompliance());
			stringstream sResa;
			dateInterfacePage->display(sResa, variables, journey->getReservationDeadLine(), request);

			set<const ReservationContact*> resaRules;
			BOOST_FOREACH(const ServiceUse& su, journey->getServiceUses())
			{
				const Line* line(dynamic_cast<const Line*>(su.getService()->getPath()));
				if(line == NULL) continue;
				
				if(	line->getCommercialLine()->getReservationContact() &&
					UseRule::IsReservationPossible(su.getUseRule()->getReservationAvailability(su))
				){
					resaRules.insert(line->getCommercialLine()->getReservationContact());
				}
			}
			stringstream sPhones;
			bool onlineBooking(!resaRules.empty());
			BOOST_FOREACH(const ReservationContact* rc, resaRules)
			{
				sPhones <<
					rc->getPhoneExchangeNumber() <<
					" (" << rc->getPhoneExchangeOpeningHours() << ") "
				;
				if (!OnlineReservationRule::GetOnlineReservationRule(rc))
				{
					onlineBooking = false;
				}
			}

			// Determination of the displayed place names
			string displayedDeparturePlace(
				(dynamic_cast<const Crossing*>(journey->getOrigin()->getHub())
					? dynamic_cast<const NamedPlace*>(departurePlace)
					: dynamic_cast<const NamedPlace*>(journey->getOrigin()->getHub())
				)->getFullName()
			);
			string displayedArrivalPlace(
				(dynamic_cast<const Crossing*>(journey->getDestination()->getHub())
					? dynamic_cast<const NamedPlace*>(arrivalPlace)
					: dynamic_cast<const NamedPlace*>(journey->getDestination()->getHub())
				)->getFullName()
			);

			DateTime lastDeparture(journey->getDepartureTime());
			lastDeparture += journey->getContinuousServiceRange();
			DateTime lastArrival(journey->getArrivalTime());
			lastArrival += journey->getContinuousServiceRange();

			GeoPoint departurePoint(WGS84FromLambert(departurePlace->getPoint()));
			GeoPoint arrivalPoint(WGS84FromLambert(arrivalPlace->getPoint()));

			ParametersVector pv;
			pv.push_back(Conversion::ToString(n));
			pv.push_back(Conversion::ToString(handicappedFilter));
			pv.push_back(Conversion::ToString(bikeFilter));
			pv.push_back(journey->getDepartureTime().getHour().toString());
			pv.push_back(displayedDeparturePlace);
			pv.push_back(journey->getArrivalTime().getHour().toString());
			pv.push_back(displayedArrivalPlace);
			pv.push_back(sDuration.str());
			pv.push_back(sDate.str());
			pv.push_back(Conversion::ToString(resaCompliance && resaDeadLine > now));
			pv.push_back(Conversion::ToString(resaCompliance == true));
			pv.push_back(Conversion::ToString(resaDeadLine.isUnknown() ? 0 : resaDeadLine - now));
			pv.push_back(sResa.str());
			pv.push_back(sPhones.str());
			pv.push_back(Conversion::ToString(onlineBooking));
			pv.push_back(journey->getDepartureTime().toSQLString(false));
			pv.push_back(Conversion::ToString(isTheLast));
			pv.push_back(journey->getContinuousServiceRange() ? lastDeparture.getHour().toString() : string()); //17
			pv.push_back(journey->getContinuousServiceRange() ? lastArrival.getHour().toString() : string()); //18
			pv.push_back(lexical_cast<string>(departurePoint.getLongitude()));
			pv.push_back(lexical_cast<string>(departurePoint.getLatitude()));
			pv.push_back(lexical_cast<string>(arrivalPoint.getLongitude()));
			pv.push_back(lexical_cast<string>(arrivalPoint.getLatitude()));
			
			InterfacePage::_display(stream, pv, variables, static_cast<const void*>(journey), request);
		}



		JourneyBoardInterfacePage::JourneyBoardInterfacePage()
			: Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
