
/** EqualFunction class implementation.
	@file EqualFunction.cpp
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
#include "EqualFunction.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::EqualFunction>::FACTORY_KEY("=");

	namespace cms
	{
		const string EqualFunction::PARAMETER_L("l");
		const string EqualFunction::PARAMETER_R("r");



		ParametersMap EqualFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_L, _left);
			map.insert(PARAMETER_R, _right);
			return map;
		}



		void EqualFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_left = map.getDefault<string>(PARAMETER_L);
			_right = map.getDefault<string>(PARAMETER_R);
		}



		util::ParametersMap EqualFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			stream << (_right == _left);
			return util::ParametersMap();
		}



		bool EqualFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string EqualFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
