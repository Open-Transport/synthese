
/** CityListRequestInterfaceElement class implementation.
	@file CityListRequestInterfaceElement.cpp

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

#include "CityListRequestInterfaceElement.h"

#include "36_places_list/CityListRequest.h"

#include "30_server/FunctionRequest.h"

#include "11_interfaces/ValueElementList.h"

#include "05_html/HTMLForm.h"

#include "01_util/Conversion.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace server;
	using namespace html;
	using namespace util;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,transportwebsite::CityListRequestInterfaceElement>::FACTORY_KEY("city_list_request");

	namespace transportwebsite
	{
		void CityListRequestInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_txtField = vel.front();
			_isForOrigin = vel.front();
			_number = vel.front();
		}

		string CityListRequestInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			
			FunctionRequest<CityListRequest> clrequest(request);
			clrequest.getFunction()->setTextInput(_txtField->getValue(parameters, variables, object, request));
			clrequest.getFunction()->setIsForOrigin(Conversion::ToBool(_isForOrigin->getValue(parameters, variables, object, request)));
			clrequest.getFunction()->setNumber(Conversion::ToInt(_number->getValue(parameters, variables, object, request)));

			stream << clrequest.getURL(false);

			return string();
		}

		CityListRequestInterfaceElement::~CityListRequestInterfaceElement()
		{
		}
	}
}
