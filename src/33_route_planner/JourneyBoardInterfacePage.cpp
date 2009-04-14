
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

#include <sstream>
#include <set>
#include <boost/foreach.hpp>

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
				
				if(	line->getCommercialLine()->getReservationContact(su.getUserClass()) &&
					su.getUseRule().getReservationType() != UseRule::RESERVATION_FORBIDDEN
				){
					resaRules.insert(line->getCommercialLine()->getReservationContact(su.getUserClass()));
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
			
			InterfacePage::_display(stream, pv, variables, static_cast<const void*>(journey), request);
		}



		JourneyBoardInterfacePage::JourneyBoardInterfacePage()
			: Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
