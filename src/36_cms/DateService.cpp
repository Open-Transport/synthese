
/** DateService class implementation.
	@file DateService.cpp
	@author RCSobility
	@date 2011

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

#include "RequestException.h"
#include "Request.h"
#include "DateService.hpp"

#include <boost/date_time/posix_time/ptime.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::DateService>::FACTORY_KEY("date");
	
	namespace cms
	{
		const string DateService::PARAMETER_DAY("d");
		const string DateService::PARAMETER_WITH_TIME("t");



		DateService::DateService():
			_withTime(false)
		{}

		ParametersMap DateService::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_WITH_TIME, _withTime);
			return map;
		}

		void DateService::_setFromParametersMap(const ParametersMap& map)
		{
			string day(map.getDefault<string>(PARAMETER_DAY));
			_time = boost::posix_time::second_clock::local_time();
			if(day.size() > 1 && day[0] == '+')
			{
				_time += days(lexical_cast<int>(day.substr(1)));
			}
			else if(day.size() > 1 && day[0] == '-')
			{
				_time -= days(lexical_cast<int>(day.substr(1)));
			}

			_withTime = map.getDefault<bool>(PARAMETER_WITH_TIME, false);
		}

		void DateService::run(
			std::ostream& stream,
			const Request& request
		) const {
			stream << to_iso_extended_string(_time.date());
			if(_withTime)
			{
				stream << " " << to_simple_string(_time.time_of_day());
			}
		}
		
		
		
		bool DateService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string DateService::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
