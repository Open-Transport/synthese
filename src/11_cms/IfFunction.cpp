
/** IfFunction class implementation.
	@file IfFunction.cpp
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
#include "IfFunction.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::IfFunction>::FACTORY_KEY("if");

	namespace cms
	{
		const string IfFunction::PARAMETER_CONDITION("cond");
		const string IfFunction::PARAMETER_THEN("then");
		const string IfFunction::PARAMETER_ELSE("else");

		ParametersMap IfFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_CONDITION, _condition);
			map.insert(PARAMETER_THEN, _then);
			map.insert(PARAMETER_ELSE, _else);
			return map;
		}



		void IfFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_condition = map.isTrue(PARAMETER_CONDITION);
			if(_condition)
			{
				_then = map.getDefault<string>(PARAMETER_THEN, string(), false);
			}
			else
			{
				_else = map.getDefault<string>(PARAMETER_ELSE, string(), false);
			}
		}



		util::ParametersMap IfFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if(_condition)
			{
				stream << _then;
			}
			else
			{
				stream << _else;
			}
			return util::ParametersMap();
		}



		bool IfFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string IfFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
