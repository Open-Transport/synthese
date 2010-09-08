
/** PlacesListRequestFunction class implementation.
	@file PlacesListRequestFunction.cpp
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
#include "PlacesListFunction.h"
#include "PlacesListRequestFunction.hpp"
#include "StaticFunctionRequest.h"
#include "CMSModule.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,pt_website::PlacesListRequestFunction>::FACTORY_KEY("places_list_request");
	
	namespace pt_website
	{
		const string PlacesListRequestFunction::PARAMETER_CITY_TEXT_FIELD("city_text_field");
		const string PlacesListRequestFunction::PARAMETER_FOR_DEPARTURE("for_departure");
		const string PlacesListRequestFunction::PARAMETER_NUMBER("number");
		const string PlacesListRequestFunction::PARAMETER_TEXT_FIELD("text_field");
		
		ParametersMap PlacesListRequestFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_CITY_TEXT_FIELD, _cityTextField);
			map.insert(PARAMETER_FOR_DEPARTURE, _forDeparture);
			map.insert(PARAMETER_NUMBER, _number);
			map.insert(PARAMETER_TEXT_FIELD, _textField);
			return map;
		}

		void PlacesListRequestFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_cityTextField = map.get<string>(PARAMETER_CITY_TEXT_FIELD);
			_number = map.get<size_t>(PARAMETER_NUMBER);
			_forDeparture = map.get<bool>(PARAMETER_FOR_DEPARTURE);
			_textField = map.get<string>(PARAMETER_TEXT_FIELD);
		}

		void PlacesListRequestFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			StaticFunctionRequest<PlacesListFunction> plrequest(request, true);
			plrequest.getFunction()->setSite(CMSModule::GetSite(request));
			plrequest.getFunction()->setTextInput(_textField);
			plrequest.getFunction()->setIsForOrigin(_forDeparture);
			plrequest.getFunction()->setNumber(_number);
			plrequest.getFunction()->setCityTextInput(_cityTextField);

			stream << plrequest.getURL(false);
		}
		
		
		
		bool PlacesListRequestFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PlacesListRequestFunction::getOutputMimeType() const
		{
			return "text/html";
		}
	}
}
