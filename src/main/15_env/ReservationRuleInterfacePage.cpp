
/** ReservationRuleInterfacePage class implementation.
	@file ReservationRuleInterfacePage.cpp

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

#include "ReservationRuleInterfacePage.h"

#include "15_env/Journey.h"

#include "11_interfaces/DateTimeInterfacePage.h"
#include "11_interfaces/Interface.h"

#include "04_time/DateTime.h"

#include <sstream>

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace time;

	namespace util
	{
		// template<> const string FactoryTemplate<InterfacePage, ReservationRuleInterfacePage>::FACTORY_KEY("");
	}

	namespace env
	{

		void ReservationRuleInterfacePage::display( std::ostream& stream , interfaces::VariablesMap& variables , const Journey& journey, const server::Request* request /*= NULL  */ ) const
		{
			DateTime now(TIME_CURRENT);
			ParametersVector pv;

			pv.push_back(Conversion::ToString(boost::logic::indeterminate(journey.getReservationCompliance()) && journey.getReservationDeadLine() > now));
			pv.push_back(Conversion::ToString(journey.getReservationCompliance() == true));
			pv.push_back(Conversion::ToString(journey.getReservationDeadLine() - now));
			
			stringstream s;
			boost::shared_ptr<const DateTimeInterfacePage> datePage(getInterface()->getPage<DateTimeInterfacePage>());
			datePage->display(s, variables, journey.getReservationDeadLine(), request);
			pv.push_back(s.str());

			InterfacePage::display(stream, pv, variables, static_cast<const void*>(&journey), request);
		}
	}
}
