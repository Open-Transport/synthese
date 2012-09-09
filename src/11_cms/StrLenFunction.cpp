
/** StrLenFunction class implementation.
	@file StrLenFunction.cpp
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
#include "StrLenFunction.hpp"
#include "IConv.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::StrLenFunction>::FACTORY_KEY("strlen");

	namespace cms
	{
		const string StrLenFunction::PARAMETER_TEXT("t");

		ParametersMap StrLenFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_TEXT, _text);
			return map;
		}

		void StrLenFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_text = IConv("UTF-8", "CP1252").convert(
				map.getDefault<string>(PARAMETER_TEXT, string(), false)
			);
		}

		util::ParametersMap StrLenFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			stream << _text.size();
			return util::ParametersMap();
		}



		bool StrLenFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string StrLenFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
