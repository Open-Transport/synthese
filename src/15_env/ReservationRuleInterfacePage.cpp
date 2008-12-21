
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

#include "Journey.h"

#include "DateTimeInterfacePage.h"
#include "Interface.h"

#include "DateTime.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, env::ReservationRuleInterfacePage>::FACTORY_KEY("reservation_rule");
	}

	namespace env
	{

		void ReservationRuleInterfacePage::display( std::ostream& stream , interfaces::VariablesMap& variables , const Journey& journey, const server::Request* request /*= NULL  */ ) const
		{
			DateTime now(TIME_CURRENT);
			ParametersVector pv;
			DateTime deadLine(journey.getReservationDeadLine());
			logic::tribool compliance(journey.getReservationCompliance());

			pv.push_back(Conversion::ToString(logic::indeterminate(compliance) && deadLine > now));
			pv.push_back(Conversion::ToString(compliance == true));
			pv.push_back(Conversion::ToString(deadLine.isUnknown() ? 0 : deadLine - now));
			
			stringstream s;
			const DateTimeInterfacePage* datePage(getInterface()->getPage<DateTimeInterfacePage>());
			datePage->display(s, variables, deadLine, request);
			pv.push_back(s.str());

			InterfacePage::display(stream, pv, variables, static_cast<const void*>(&journey), request);
		}



		ReservationRuleInterfacePage::ReservationRuleInterfacePage()
			: Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
