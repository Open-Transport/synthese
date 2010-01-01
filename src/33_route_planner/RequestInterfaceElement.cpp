
/** RequestInterfaceElement class implementation.
	@file RequestInterfaceElement.cpp

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

#include "RequestInterfaceElement.h"
#include "RoutePlannerFunction.h"
#include "FunctionRequest.h"
#include "ValueElementList.h"
#include "HTMLForm.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace server;
	using namespace html;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,routeplanner::RequestInterfaceElement>::FACTORY_KEY("route_planner_form");

	namespace routeplanner
	{
		void RequestInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_formName = vel.front();
			_html = vel.front();
		}

		string RequestInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {

			string formName(_formName->getValue(parameters, variables, object, request));
			string html(_html->getValue(parameters, variables, object, request));
			
			if(request)
			{
				FunctionRequest<RoutePlannerFunction> rprequest(*request);
				rprequest.getFunction()->setMaxSolutions(
					dynamic_cast<const FunctionRequest<RoutePlannerFunction>* >(
						request
					)->getFunction()->getMaxSolutions()
				);

				HTMLForm form(rprequest.getHTMLForm(formName));
				stream << form.open(html);
				stream << form.getHiddenFields();
			}

			return string();
		}

		RequestInterfaceElement::~RequestInterfaceElement()
		{
		}
	}
}
