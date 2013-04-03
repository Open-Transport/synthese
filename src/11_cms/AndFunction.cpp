
/** AndFunction class implementation.
	@file AndFunction.cpp
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
#include "AndFunction.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::AndFunction>::FACTORY_KEY("and");

	namespace cms
	{
		const string AndFunction::PARAMETER_LEFT("l");
		const string AndFunction::PARAMETER_RIGHT("r");

		Function::API AndFunction::getAPI() const
		{
			Function::API api(
						"CMS Language",
						"And operator",
						"Example:\n"
						"<?and&l=1&r=1?> returns true.\n"
						"");
			api.addParams(APIParam(AndFunction::PARAMETER_LEFT, "Left value to test", true));
			api.addParams(APIParam(AndFunction::PARAMETER_RIGHT, "Right value to test", true));
			return api;
		}


		ParametersMap AndFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_LEFT, _left);
			map.insert(PARAMETER_RIGHT, _right);
			return map;
		}



		void AndFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_left = map.isTrue(PARAMETER_LEFT);
			_right = map.isTrue(PARAMETER_RIGHT);
		}



		util::ParametersMap AndFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			stream << (_left && _right);
			return util::ParametersMap();
		}



		bool AndFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string AndFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
