
//////////////////////////////////////////////////////////////////////////////////////////
///	PrecisionService class implementation.
///	@file PrecisionService.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "PrecisionService.hpp"

#include "RequestException.h"
#include "Request.h"

#include <boost/format.hpp>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,cms::PrecisionService>::FACTORY_KEY = "format";

	namespace cms
	{
		const string PrecisionService::PARAMETER_NUMBER = "n";
		const string PrecisionService::PARAMETER_FORMAT = "f";



		ParametersMap PrecisionService::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NUMBER, _value);
			map.insert(PARAMETER_FORMAT, _format);
			return map;
		}



		void PrecisionService::_setFromParametersMap(const ParametersMap& map)
		{
			_value = map.getDefault<double>(PARAMETER_NUMBER, 0.0);
			_format = map.getDefault<string>(PARAMETER_FORMAT, "1%");
		}



		ParametersMap PrecisionService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			stream << boost::format("%"+ _format) % _value;
			return map;
		}



		bool PrecisionService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PrecisionService::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
