
/** DateService class implementation.
	@file DateService.cpp
	@author Hugues Romain
	@date 2011

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
#include "DateService.hpp"

#include <boost/date_time/posix_time/ptime.hpp>
#include <locale>

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
		const string DateService::PARAMETER_OFFSET("d");
		const string DateService::PARAMETER_WITH_TIME("t");
		const string DateService::PARAMETER_BASE("b");
		const string DateService::PARAMETER_UNIT("u");
		const string DateService::PARAMETER_STRFTIME_FORMAT("strftime_format");
		const string DateService::PARAMETER_LANG("lang");


		DateService::DateService():
			_withTime(false)
		{}



		ParametersMap DateService::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_WITH_TIME, _withTime);
			map.insert(PARAMETER_STRFTIME_FORMAT, _strftimeFormat);
			map.insert(PARAMETER_LANG, _strftimeLang);
			return map;
		}



		void DateService::_setFromParametersMap(const ParametersMap& map)
		{
			_offset = map.getDefault<string>(PARAMETER_OFFSET);
			string base = map.getDefault<string>(PARAMETER_BASE);

			if(!base.empty())
			{
				try
				{
					if(base.find(' ') == string::npos)
					{
						_time = ptime(from_string(base));
					}
					else
					{
						_time = time_from_string(base);
					}
				}
				catch(...)
				{
					throw RequestException("Bad format for parameter b, must be YYYY-MM-DD or YYYY-MM-DD HH:MM");
				}
			}
			else
			{
				_time = second_clock::local_time();
			}
			_withTime = map.getDefault<bool>(PARAMETER_WITH_TIME, false);
			string stringUnit = map.getDefault<string>(PARAMETER_UNIT, "day");

			if(stringUnit == "second")
				_unit = SECOND;
			else if(stringUnit == "minute")
				_unit = MINUTE;
			else if(stringUnit == "hour")
				_unit = HOUR;
			else if(stringUnit == "day")
				_unit = DAY;
			else
				throw RequestException("Bad unit for parameter u, must be in {'second', 'minute', 'hour', 'day'}");

			_strftimeFormat = map.getDefault<string>(PARAMETER_STRFTIME_FORMAT);
			_strftimeLang = map.getDefault<string>(PARAMETER_LANG);
		}



		util::ParametersMap DateService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ptime final_time;
			if(_offset.size() > 1)
			{
				time_duration offsetAbs(0,0,0,0);
				switch(_unit)
				{
					case SECOND:
						offsetAbs += seconds(lexical_cast<int>(_offset.substr(1)));
						break;
					case MINUTE:
						offsetAbs += minutes(lexical_cast<int>(_offset.substr(1)));
						break;
					case HOUR:
						offsetAbs += hours(lexical_cast<int>(_offset.substr(1)));
						break;
					case DAY:
						offsetAbs += hours(24*lexical_cast<int>(_offset.substr(1)));
						break;
				}

				if(_offset[0] == '+')
					final_time = _time + offsetAbs;
				else if(_offset[0] == '-')
					final_time = _time - offsetAbs;
				else
					throw RequestException("Bad format for parameter d, must be +X or -X, X natural number");
			}
			else
				final_time = _time;

			if(_strftimeFormat.empty())
			{
				stream << to_iso_extended_string(final_time.date());
				if(_withTime)
				{
					stream << " " << to_simple_string(final_time.time_of_day());
				}
			}
			else
			{
				const tm tm_time = to_tm(final_time);
				char str_date[100];

				if(!_strftimeLang.empty())
				{
					//Locale given : localise strftime
					try
					{
						locale currentLocale(std::locale::global(std::locale( _strftimeLang.c_str())));
						if(strftime(str_date, 100, _strftimeFormat.c_str(), &tm_time))
						{
							stream << str_date;
						}
						std::locale::global(currentLocale);
					}
					catch(...)
					{
						throw RequestException("Failed to set locale to " + _strftimeLang);
					}
				}
				else
				{
					if(strftime(str_date, 100, _strftimeFormat.c_str(), &tm_time))
					{
						stream << str_date;
					}
				}
			}
			return util::ParametersMap();
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
