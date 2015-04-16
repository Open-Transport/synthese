
/** ChrFunction class implementation.
	@file ChrFunction.cpp
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
#include "ChrFunction.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::ChrFunction>::FACTORY_KEY("char");

	namespace cms
	{
		const string ChrFunction::PARAMETER_CODE("code");



		ParametersMap ChrFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_CODE, _code);
			return map;
		}



		void ChrFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_code = map.get<string>(PARAMETER_CODE);
		}



		util::ParametersMap ChrFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if(!_code.empty())
			{
				stream << char(atoi(_code.c_str()));
			}
			return util::ParametersMap();
		}



		bool ChrFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ChrFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
