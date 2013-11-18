
/** StrReplaceFunction class implementation.
	@file StrReplaceFunction.cpp
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
#include "StrReplaceFunction.hpp"

#include <boost/regex.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	
	template<> const string util::FactorableTemplate<Function,cms::StrReplaceFunction>::FACTORY_KEY("StrReplace");

	namespace cms
	{
		const string StrReplaceFunction::PARAMETER_TEXT("text");
		const string StrReplaceFunction::PARAMETER_SEARCH("search");
		const string StrReplaceFunction::PARAMETER_REPLACE("replace");


		ParametersMap StrReplaceFunction::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void StrReplaceFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_text = map.getDefault<string>(PARAMETER_TEXT);
			_search = map.getDefault<string>(PARAMETER_SEARCH, string());
			_replace = map.getDefault<string>(PARAMETER_REPLACE, string());
		}



		util::ParametersMap StrReplaceFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if (!_text.empty())
			{
				boost::regex reg_search(_search);
				stream << boost::regex_replace(_text, reg_search, _replace);
			}
			else
			{
				stream << "invalid parameters";
			}
				
			return util::ParametersMap();
		}



		bool StrReplaceFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string StrReplaceFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}
