
/** HtmlFormInterfaceElement class implementation.
	@file HtmlFormInterfaceElement.cpp

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

#include <sstream>

#include <boost/shared_ptr.hpp>

#include "01_util/FactoryException.h"

#include "05_html/HTMLForm.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/RequestWithInterface.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/Interface.h"

#include "30_server/Request.h"
#include "30_server/Action.h"
#include "30_server/HtmlFormInterfaceElement.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace interfaces;
	using namespace html;
	using namespace util;

	namespace server
	{
		void HtmlFormInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 3)
				throw InterfacePageException("Not enough arguments in HTML Form interface element");

			_name = vel.front();
			_function_key = vel.front();
			_function_parameters = vel.front();
			if (!vel.isEmpty())
			{
				_with_action = true;
				_action_key = vel.front();
				_action_parameters = vel.front();
			}
			else
			{
				_with_action = false;
			}
		}

		string HtmlFormInterfaceElement::getValue(
			const interfaces::ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			stringstream stream;
			try
			{
				string actionKey;
				if (_action_key)
					actionKey = _action_key->getValue(parameters, variables, object, request);
				string actionParameters;
				if (_action_parameters)
					actionParameters = _action_parameters->getValue(parameters, variables, object, request);
				string functionParameters;
				if (_function_parameters)
					functionParameters = _function_parameters->getValue(parameters, variables, object, request);
				
				stringstream s;
				s	<< Request::PARAMETER_FUNCTION << Request::PARAMETER_ASSIGNMENT << _function_key->getValue(parameters, variables, object, request)
					<< Request::PARAMETER_SEPARATOR << Request::PARAMETER_IP << Request::PARAMETER_ASSIGNMENT
					<< Request::PARAMETER_SEPARATOR << RequestWithInterface::PARAMETER_INTERFACE << Request::PARAMETER_ASSIGNMENT << _page->getInterface()->getKey()
					;
					

				if (!functionParameters.empty())
					s << Request::PARAMETER_SEPARATOR << functionParameters;
				if (!actionKey.empty())
				{
					s << Request::PARAMETER_SEPARATOR << Request::PARAMETER_ACTION << Request::PARAMETER_ASSIGNMENT << actionKey;
					if (!actionParameters.empty())
						s << Request::PARAMETER_SEPARATOR << actionParameters;
				}

				Request r(s.str());
				r.setClientURL(request->getClientURL());
				
				
				HTMLForm f(r.getHTMLForm(_name->getValue(parameters, variables, object, request)));
				stream << f.open();
				stream << f.getHiddenFields();
				
				return stream.str();
			}
			catch(FactoryException<Request> e)
			{
				return "";
			}
			catch(FactoryException<Action> e)
			{
				return "";
			}

		}
	}
}

