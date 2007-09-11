
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

#include "11_interfaces/DurationInterfacePage.h"
#include "11_interfaces/DateTimeInterfacePage.h"
#include "11_interfaces/Interface.h"

#include "01_util/Conversion.h"

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
		// template<> const string FactoryTemplate<InterfacePage, JourneyBoardInterfacePage>::FACTORY_KEY("");
	}

	namespace routeplanner
	{

		void JourneyBoardInterfacePage::display(
			std::ostream& stream
			, interfaces::VariablesMap& variables
			, int n
			, const Journey* journey
			, boost::logic::tribool handicappedFilter
			, boost::logic::tribool bikeFilter
			, const server::Request* request
		) const {
			const DurationInterfacePage* durationInterfacePage = getInterface()->getPage<DurationInterfacePage>();
			stringstream sDuration;
			durationInterfacePage->display(sDuration, journey->getDuration(), variables, NULL, request);
			const DateTimeInterfacePage* dateInterfacePage = getInterface()->getPage<DateTimeInterfacePage>();
			stringstream sDate;
			dateInterfacePage->display(sDate, variables, journey->getDepartureTime().getDate(), request);
			DateTime now(TIME_CURRENT);
			stringstream sResa;
			dateInterfacePage->display(sResa, variables, journey->getReservationDeadLine(), request);

			set<const ReservationRule*> resaRules;
			for (Journey::ServiceUses::const_iterator it(journey->getServiceUses().begin()); it != journey->getServiceUses().end(); ++it)
			{
				if (it->getService()->getReservationRule() && it->getService()->getReservationRule() != ReservationRule::Get(0).get())
					resaRules.insert(it->getService()->getReservationRule());
			}
			stringstream sPhones;
			bool onlineBooking(!resaRules.empty());
			for (set<const ReservationRule*>::const_iterator it(resaRules.begin()); it != resaRules.end(); ++it)
			{
				sPhones << (*it)->getPhoneExchangeNumber() << " (" << (*it)->getPhoneExchangeOpeningHours() << ") ";
				if (!OnlineReservationRule::GetOnlineReservationRule(*it))
					onlineBooking = false;
			}

			ParametersVector pv;
			pv.push_back(Conversion::ToString(n));
			pv.push_back(Conversion::ToString(handicappedFilter));
			pv.push_back(Conversion::ToString(bikeFilter));
			pv.push_back(journey->getDepartureTime().getHour().toString());
			pv.push_back(journey->getOrigin()->getPlace()->getFullName());
			pv.push_back(journey->getArrivalTime().getHour().toString());
			pv.push_back(journey->getDestination()->getPlace()->getFullName());
			pv.push_back(sDuration.str());
			pv.push_back(sDate.str());
			pv.push_back(Conversion::ToString(boost::logic::indeterminate(journey->getReservationCompliance()) && journey->getReservationDeadLine() > now));
			pv.push_back(Conversion::ToString(journey->getReservationCompliance() == true));
			pv.push_back(Conversion::ToString(journey->getReservationDeadLine() - now));
			pv.push_back(sResa.str());
			pv.push_back(sPhones.str());
			pv.push_back(Conversion::ToString(onlineBooking));
			
			InterfacePage::display(stream, pv, variables, static_cast<const void*>(journey), request);
		}
	}
}
