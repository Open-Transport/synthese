
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
#include "CityListRequest.h"
#include "StaticFunctionRequest.h"
#include "ValueElementList.h"
#include "HTMLForm.h"
#include "Conversion.h"

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
			
			if(request)
			{
				StaticFunctionRequest<CityListRequest> clrequest(*request, true);
				if(dynamic_cast<const FunctionWithSite*>(request->getFunction().get()))
				{
					clrequest.getFunction()->setSite(dynamic_cast<const FunctionWithSite*>(request->getFunction().get())->getSite());
				}
				clrequest.getFunction()->setTextInput(_txtField->getValue(parameters, variables, object, request));
				clrequest.getFunction()->setIsForOrigin(Conversion::ToBool(_isForOrigin->getValue(parameters, variables, object, request)));
				clrequest.getFunction()->setNumber(Conversion::ToInt(_number->getValue(parameters, variables, object, request)));

				stream << clrequest.getURL(false);
			}

			return string();
		}

		CityListRequestInterfaceElement::~CityListRequestInterfaceElement()
		{
		}
	}
}
