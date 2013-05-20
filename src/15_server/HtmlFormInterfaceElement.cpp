
/** HtmlFormInterfaceElement class implementation.
	@file HtmlFormInterfaceElement.cpp

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

#include "HtmlFormInterfaceElement.h"

#include "DynamicRequest.h"
#include "Session.h"
#include "HTTPRequest.hpp"
#include "Action.h"
#include "FactoryException.h"
#include "HTMLForm.h"
#include "URI.hpp"
#include "ValueElementList.h"
#include "InterfacePageException.h"
#include "RequestWithInterface.h"
#include "InterfacePage.h"
#include "Interface.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace html;
	using namespace util;
	using namespace server;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, HtmlFormInterfaceElement>::FACTORY_KEY("htmlform");
	}

	namespace server
	{
		void HtmlFormInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 1)
				throw InterfacePageException("Not enough arguments in HTML Form interface element");

			_name = vel.front();
			if (!vel.isEmpty())
			{
				_function_key = vel.front();
				_function_parameters = vel.front();
			}
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

			while(!vel.isEmpty())
				_fieldsToAvoid.push_back(vel.front());
		}

		string HtmlFormInterfaceElement::display(
			ostream& stream
			, const interfaces::ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const	{
			try
			{
				string functionKey;
				if (_function_key.get())
					functionKey = _function_key->getValue(parameters, variables, object, request);
				if (functionKey.empty())
					functionKey = request->getFunction()->getFactoryKey();
				string actionKey;
				if (_action_key.get())
					actionKey = _action_key->getValue(parameters, variables, object, request);
				string actionParameters;
				if (_action_parameters.get())
					actionParameters = _action_parameters->getValue(parameters, variables, object, request);
				string functionParameters;
				if (_function_parameters.get())
					functionParameters = _function_parameters->getValue(parameters, variables, object, request);

				stringstream s;
				s	<<
					request->getClientURL() << Request::PARAMETER_STARTER <<
					Request::PARAMETER_SERVICE << URI::PARAMETER_ASSIGNMENT << functionKey
					<< URI::PARAMETER_SEPARATOR << RequestWithInterface::PARAMETER_INTERFACE << URI::PARAMETER_ASSIGNMENT << _page->getInterface()->getKey()
					;


				if (!functionParameters.empty())
					s << URI::PARAMETER_SEPARATOR << functionParameters;
				if (!actionKey.empty())
				{
					s << URI::PARAMETER_SEPARATOR << Request::PARAMETER_ACTION << URI::PARAMETER_ASSIGNMENT << actionKey;
					if (!actionParameters.empty())
						s << URI::PARAMETER_SEPARATOR << actionParameters;
				}
				if (request->getSession())
					s << URI::PARAMETER_SEPARATOR << Request::PARAMETER_SESSION << URI::PARAMETER_ASSIGNMENT << request->getSession()->getKey();

				HTTPRequest q;
				q.uri = s.str();
				q.ipaddr = request->getIP();
				DynamicRequest r(q);
				r.setClientURL(request->getClientURL());


				HTMLForm f(r.getHTMLForm(_name->getValue(parameters, variables, object, request)));

				for (vector<boost::shared_ptr<LibraryInterfaceElement> >::const_iterator it = _fieldsToAvoid.begin(); it != _fieldsToAvoid.end(); ++it)
				{
					string fieldName((*it)->getValue(parameters, variables, object, request));
					string fieldValue;
					f.removeHiddenFieldIfExists(fieldName, fieldValue);
				}
				stream << f.open();
				stream << f.getHiddenFields();

				return string();
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

