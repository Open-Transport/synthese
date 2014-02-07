/** CheckLineCalendarFunction class implementation.
	@file CheckLineCalendarFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "CheckLineCalendarFunction.hpp"
#include "CommercialLine.h"
#include "CalendarTemplate.h"
#include "Calendar.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt;
	using namespace calendar;

	template<>
	const string util::FactorableTemplate<Function, analysis::CheckLineCalendarFunction>::FACTORY_KEY("CheckLineCalendar");

	namespace analysis
	{
		const string CheckLineCalendarFunction::PARAMETER_DURATION("du");

		ParametersMap CheckLineCalendarFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_line.get())
			{
				map.insert(Request::PARAMETER_OBJECT_ID, _line->getKey());
				map.insert(PARAMETER_DURATION, static_cast<int>(_duration.days()));
			}
			return map;
		}

		void CheckLineCalendarFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType id(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			if(Env::GetOfficialEnv().getRegistry<CommercialLine>().contains(id))
			{
				_line = Env::GetOfficialEnv().get<CommercialLine>(id);
			}

			_duration = days(map.getDefault<int>(PARAMETER_DURATION, 45));
		}



		util::ParametersMap CheckLineCalendarFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;

			if(	!_line.get()
			){
				stream << "3\nNo such line.|days=-1\n";
			} else {

				date now(day_clock::local_day());
				date endDate(now + _duration);
				Calendar period(now, endDate);

				Calendar runDays(_line->getRunDays(period));

				if(	!_line->getCalendarTemplate()
				){
					stream << "3\nLine "+ _line->getShortName() +" is unmonitored.|days="+ lexical_cast<string>(runDays.size()) + "\n";
				} else {

					if(_line->respectsCalendarTemplate(_duration))
					{
						stream << "0\nOK JourneyPattern "+ _line->getShortName() +" runs the "+ lexical_cast<string>(_duration.days()) +" next days of "+ _line->getCalendarTemplate()->getName() +"|days="+ lexical_cast<string>(runDays.size()) + "\n";
					}
					else
					{
						stream << "2\nKO JourneyPattern "+ _line->getShortName() +" does not run then "+ lexical_cast<string>(_duration.days()) +" days of "+ _line->getCalendarTemplate()->getName() +"|days="+ lexical_cast<string>(runDays.size()) + "\n";
					}
				}
			}

			return pm;
		}



		bool CheckLineCalendarFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string CheckLineCalendarFunction::getOutputMimeType() const
		{
			return "text/plain";
		}



		CheckLineCalendarFunction::CheckLineCalendarFunction():
			FactorableTemplate<server::Function,CheckLineCalendarFunction>(),
			_duration(days(45))
		{

		}
	}
}
