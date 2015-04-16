
//////////////////////////////////////////////////////////////////////////////////////////
///	TimeDurationService class implementation.
///	@file TimeDurationService.cpp
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

#include "TimeDurationService.hpp"

#include "RequestException.h"
#include "Request.h"

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,cms::TimeDurationService>::FACTORY_KEY = "time_duration";

	namespace cms
	{
		const string TimeDurationService::PARAMETER_HOURS = "h";
		const string TimeDurationService::PARAMETER_MINUTES = "m";
		const string TimeDurationService::PARAMETER_SECONDS = "s";



		ParametersMap TimeDurationService::_getParametersMap() const
		{
			ParametersMap map;
			if(!_value.is_not_a_date_time())
			{
				map.insert(PARAMETER_HOURS, _value.hours());
				map.insert(PARAMETER_MINUTES, _value.minutes());
				map.insert(PARAMETER_SECONDS, _value.seconds());
			}
			return map;
		}



		void TimeDurationService::_setFromParametersMap(const ParametersMap& map)
		{
			_value = seconds(map.getDefault<long>(PARAMETER_SECONDS, 0));
			_value += minutes(map.getDefault<long>(PARAMETER_MINUTES, 0));
			_value += hours(map.getDefault<long>(PARAMETER_HOURS, 0));
		}



		ParametersMap TimeDurationService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			stream << _value;
			return map;
		}



		bool TimeDurationService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string TimeDurationService::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
