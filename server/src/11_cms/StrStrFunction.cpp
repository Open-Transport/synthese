
/** StrStrFunction class implementation.
	@file StrStrFunction.cpp
	@date 2013

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
#include "StrStrFunction.hpp"

#include <boost/algorithm/string/find.hpp>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	
	template<> const string util::FactorableTemplate<Function,cms::StrStrFunction>::FACTORY_KEY("StrStr");

	namespace cms
	{
		const string StrStrFunction::PARAMETER_TEXT("text");
		const string StrStrFunction::PARAMETER_MATCH("match");
		const string StrStrFunction::PARAMETER_INSENSITIVE("insensitive");


		ParametersMap StrStrFunction::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void StrStrFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_text = map.getDefault<string>(PARAMETER_TEXT);
			_match = map.getDefault<string>(PARAMETER_MATCH);
			_insensitive = map.getDefault<bool>(PARAMETER_INSENSITIVE);
		}



		util::ParametersMap StrStrFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if (!_text.empty() && !_match.empty())
			{
				const char * text = _text.c_str();
				const char * match = _match.c_str();
				const char * ret = NULL;
				if (_insensitive) {
					boost::iterator_range<const char *> it = boost::ifind_first(text,match);
					if (it) {
						ret = it.begin();
					};
				}
				else
				{
					ret = strstr(text,match);
				}
				if (ret)
				{
					stream << ret - text + 1;
				}
				else
				{
					stream << "0";
				}
			}
			else
			{
				stream << "invalid parameters";
			}
				
			return util::ParametersMap();
		}



		bool StrStrFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string StrStrFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}
