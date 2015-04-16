
/** OrService class implementation.
	@file OrService.cpp
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
#include "OrService.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::OrService>::FACTORY_KEY("or");

	namespace cms
	{
		const string OrService::PARAMETER_LEFT("l");
		const string OrService::PARAMETER_RIGHT("r");



		ParametersMap OrService::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_LEFT, _left);
			map.insert(PARAMETER_RIGHT, _right);
			return map;
		}



		void OrService::_setFromParametersMap(const ParametersMap& map)
		{
			_left = map.isTrue(PARAMETER_LEFT);
			_right = map.isTrue(PARAMETER_RIGHT);
		}



		util::ParametersMap OrService::run(
			std::ostream& stream,
			const Request& request
		) const {
			stream << (_left || _right);
			return util::ParametersMap();
		}



		bool OrService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string OrService::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
