
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

#include "31_resa/OnlineReservationRule.h"

#include "15_env/Journey.h"
#include "15_env/Edge.h"
#include "15_env/AddressablePlace.h"
#include "15_env/ReservationRule.h"
#include "15_env/Service.h"
#include "15_env/Crossing.h"

#include "11_interfaces/DurationInterfacePage.h"
#include "11_interfaces/DateTimeInterfacePage.h"
#include "11_interfaces/Interface.h"

#include "01_util/Conversion.h"
#include "Env.h"

#include <sstream>
#include <set>

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

			set<const ReservationRule*> resaRules;
			for (Journey::ServiceUses::const_iterator it(journey->getServiceUses().begin()); it != journey->getServiceUses().end(); ++it)
			{
				if (it->getService()->getReservationRule() && it->getService()->getReservationRule() != Env::GetOfficialEnv()->getRegistry<ReservationRule>().get(0))
					resaRules.insert(it->getService()->getReservationRule().get());
			}
			stringstream sPhones;
			bool onlineBooking(!resaRules.empty());
			for (set<const ReservationRule*>::const_iterator it(resaRules.begin()); it != resaRules.end(); ++it)
			{
				sPhones << (*it)->getPhoneExchangeNumber() << " (" << (*it)->getPhoneExchangeOpeningHours() << ") ";
				if (!OnlineReservationRule::GetOnlineReservationRule(*it))
					onlineBooking = false;
			}

			// Determination of the displayed place names
			string displayedDeparturePlace(
				dynamic_cast<const Crossing*>(journey->getOrigin()->getPlace())
				? departurePlace->getFullName()
				: journey->getOrigin()->getPlace()->getFullName()
			);
			string displayedArrivalPlace(
				dynamic_cast<const Crossing*>(journey->getDestination()->getPlace())
				? arrivalPlace->getFullName()
				: journey->getDestination()->getPlace()->getFullName()
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
			
			InterfacePage::display(stream, pv, variables, static_cast<const void*>(journey), request);
		}



		JourneyBoardInterfacePage::JourneyBoardInterfacePage()
			: Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
