
/** PlacesListRequestInterfaceElement class implementation.
	@file PlacesListRequestInterfaceElement.cpp

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

#include "PlacesListRequestInterfaceElement.h"

#include "36_places_list/PlacesListFunction.h"

#include "30_server/FunctionRequest.h"

#include "11_interfaces/ValueElementList.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace server;

	namespace transportwebsite
	{
		void PlacesListRequestInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_txtField = vel.front();
			_cityTxtField = vel.front();
			_cityIdField = vel.front();
			_isForOrigin = vel.front();
			_number = vel.front();
		}

		string PlacesListRequestInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			FunctionRequest<PlacesListFunction> plrequest(request);
			plrequest.getFunction()->setTextInput(_txtField->getValue(parameters, variables, object, request));
			plrequest.getFunction()->setIsForOrigin(Conversion::ToBool(_isForOrigin->getValue(parameters, variables, object, request)));
			plrequest.getFunction()->setNumber(Conversion::ToInt(_number->getValue(parameters, variables, object, request)));
			plrequest.getFunction()->setCityIdInput(_cityIdField->getValue(parameters, variables, object, request));
			plrequest.getFunction()->setCityTextInput(_cityTxtField->getValue(parameters, variables, object, request));

			stream << plrequest.getURL(false);

			return string();
		}

		PlacesListRequestInterfaceElement::~PlacesListRequestInterfaceElement()
		{
		}
	}
}
