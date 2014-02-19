
/** ReservationRuleInterfacePage class implementation.
	@file ReservationRuleInterfacePage.cpp

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

#include "ReservationRuleInterfacePage.h"
#include "Journey.h"
#include "DateTimeInterfacePage.h"
#include "Webpage.h"
#include "Request.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;
	using namespace cms;
	using namespace server;

	namespace pt
	{
		const string ReservationRuleInterfacePage::DATA_DEADLINE("deadline");
		const string ReservationRuleInterfacePage::DATA_DELAY("delay");
		const string ReservationRuleInterfacePage::DATA_IS_COMPULSORY("is_compulsory");
		const string ReservationRuleInterfacePage::DATA_IS_OPTIONAL("is_optional");

		void ReservationRuleInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const Webpage> page,
			boost::shared_ptr<const Webpage> dateTimePage,
			const Request& request,
			const util::ParametersMap& templateParametersMap,
			const Journey& journey
		){
			ParametersMap pm(templateParametersMap);

			ptime now(second_clock::local_time());
			ptime deadLine(journey.getReservationDeadLine());
			logic::tribool compliance(journey.getReservationCompliance(false));

			pm.insert(DATA_IS_OPTIONAL, logic::indeterminate(compliance) && (deadLine.is_not_a_date_time() || deadLine > now));
			pm.insert(DATA_IS_COMPULSORY, compliance == true);
			pm.insert(DATA_DELAY, deadLine.is_not_a_date_time() ? 0 : (deadLine - now).total_seconds() / 60);

			if(!deadLine.is_not_a_date_time())
			{
				if(dateTimePage.get())
				{
					stringstream s;
					DateTimeInterfacePage::Display(
						s,
						dateTimePage,
						request,
						deadLine
					);
					pm.insert(DATA_DEADLINE, s.str());
				}
				else
				{
					pm.insert(DATA_DEADLINE, deadLine);
				}
			}

			page->display(stream, request, pm);
		}
}	}
