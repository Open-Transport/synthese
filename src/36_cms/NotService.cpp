
//////////////////////////////////////////////////////////////////////////////////////////
///	NotService class implementation.
///	@file NotService.cpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "RequestException.h"
#include "Request.h"
#include "NotService.hpp"

#include "BitAndFunction.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::NotService>::FACTORY_KEY("not");
	
	namespace cms
	{
		const string NotService::PARAMETER_P = "p";
		


		ParametersMap NotService::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_P, _parameter);
			return map;
		}



		void NotService::_setFromParametersMap(const ParametersMap& map)
		{
			// The parameter
			string pStr(trim_copy_if(map.get<string>(PARAMETER_P), is_any_of(" \r\n")));
			if(!pStr.empty())
			{
				_parameter = lexical_cast<bool>(pStr);
			}
		}



		void NotService::run(
			std::ostream& stream,
			const Request& request
		) const {
			stream << !_parameter;
		}
		
		
		
		bool NotService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string NotService::getOutputMimeType() const
		{
			return "text/plain";
		}



		NotService::NotService():
			_parameter(false)
		{}
}	}
