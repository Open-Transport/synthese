
/** BitAndFunction class implementation.
	@file BitAndFunction.cpp
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
#include "BitAndFunction.hpp"
#include "CMSModule.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::BitAndFunction>::FACTORY_KEY("bit_and");

	namespace cms
	{
		const string BitAndFunction::PARAMETER_LEFT("l");
		const string BitAndFunction::PARAMETER_RIGHT("r");



		ParametersMap BitAndFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_LEFT, _left);
			map.insert(PARAMETER_RIGHT, _right);
			return map;
		}



		void BitAndFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Left
			string leftStr(CMSModule::Trim(map.get<string>(PARAMETER_LEFT)));
			if(!leftStr.empty())
			{
				_left = lexical_cast<int>(leftStr);
			}

			// Right
			string rightStr(CMSModule::Trim(map.get<string>(PARAMETER_RIGHT)));
			if(!rightStr.empty())
			{
				_right = lexical_cast<int>(rightStr);
			}
		}



		void BitAndFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			stream << (_left & _right);
		}



		bool BitAndFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string BitAndFunction::getOutputMimeType() const
		{
			return "text/plain";
		}



		BitAndFunction::BitAndFunction():
			_left(0),
			_right(0)
		{}
}	}
