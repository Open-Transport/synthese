
/** SubStrFunction class implementation.
	@file SubStrFunction.cpp
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
#include "SubStrFunction.hpp"
#include "IConv.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::SubStrFunction>::FACTORY_KEY("substr");

	namespace cms
	{
		const string SubStrFunction::PARAMETER_TEXT("t");
		const string SubStrFunction::PARAMETER_LENGTH("n");
		const string SubStrFunction::PARAMETER_FIRST("f");



		ParametersMap SubStrFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_TEXT, _text);
			map.insert(PARAMETER_FIRST, _first);
			if(_size)
			{
				map.insert(PARAMETER_LENGTH, *_size);
			}
			return map;
		}



		void SubStrFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_text = IConv("UTF-8", "CP1252").convert(
				map.getDefault<string>(PARAMETER_TEXT, string(), false)
			);
			_first = map.getDefault<size_t>(PARAMETER_FIRST, 0);
			_size = map.getOptional<size_t>(PARAMETER_LENGTH);
		}



		util::ParametersMap SubStrFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if(_size && *_size < _text.size() - _first)
			{
				stream << IConv("CP1252", "UTF-8").convert(_text.substr(_first, *_size));
			}
			else if(_first < _text.size())
			{
				stream << IConv("CP1252", "UTF-8").convert(_text.substr(_first));
			}
			return util::ParametersMap();
		}



		bool SubStrFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string SubStrFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
