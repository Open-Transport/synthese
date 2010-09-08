
/** CityListRequestFunction class implementation.
	@file CityListRequestFunction.cpp
	@author Hugues Romain
	@date 2010

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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
#include "CityListRequestFunction.hpp"
#include "CityListFunction.h"
#include "StaticFunctionRequest.h"
#include "CMSModule.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,pt_website::CityListRequestFunction>::FACTORY_KEY("city_list_request");
	
	namespace pt_website
	{
		const string CityListRequestFunction::PARAMETER_FOR_DEPARTURE("for_departure");
		const string CityListRequestFunction::PARAMETER_NUMBER("number");
		const string CityListRequestFunction::PARAMETER_TEXT("text_field");
		
		ParametersMap CityListRequestFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_FOR_DEPARTURE, _forDeparture);
			map.insert(PARAMETER_NUMBER, _number);
			map.insert(PARAMETER_TEXT, _text);
			return map;
		}

		void CityListRequestFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_forDeparture = map.get<bool>(PARAMETER_FOR_DEPARTURE);
			_number = map.get<size_t>(PARAMETER_NUMBER);
			_text = map.get<string>(PARAMETER_TEXT);
		}

		void CityListRequestFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			StaticFunctionRequest<CityListFunction> clrequest(request, true);
			clrequest.getFunction()->setSite(CMSModule::GetSite(request));
			clrequest.getFunction()->setTextInput(_text);
			clrequest.getFunction()->setIsForOrigin(_forDeparture);
			clrequest.getFunction()->setNumber(_number);

			stream << clrequest.getURL(false);
		}
		
		
		
		bool CityListRequestFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string CityListRequestFunction::getOutputMimeType() const
		{
			return "text/html";
		}
	}
}
