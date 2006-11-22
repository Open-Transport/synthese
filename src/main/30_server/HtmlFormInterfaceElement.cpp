
#include <sstream>

#include "01_util/FactoryException.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

#include "30_server/Request.h"
#include "30_server/Action.h"
#include "30_server/HtmlFormInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
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

		string HtmlFormInterfaceElement::getValue(const interfaces::ParametersVector& parameters, const void* rootObject /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			stringstream stream;
			Request* targetRequest;
			try
			{
				targetRequest = Factory<Request>::create(_function_key->getValue(parameters));
				targetRequest->copy(request);
				if (_with_action)
				{
					Action* action = Factory<Action>::create(_action_key->getValue(parameters));
					targetRequest->setAction(action);
				}
				stream << targetRequest->getHTMLFormHeader(_name->getValue(parameters));
				Request::ParametersMap map = Request::parseString(_function_parameters->getValue(parameters));
				for (Request::ParametersMap::const_iterator it = map.begin(); it != map.end(); ++it)
					stream << "<input type=\"hidden\" name=\"" << it->first << "\" value=\"" << it->second << "\" />";
				map = Request::parseString(_action_parameters->getValue(parameters));
				for (Request::ParametersMap::const_iterator it = map.begin(); it != map.end(); ++it)
					stream << "<input type=\"hidden\" name=\"" << it->first << "\" value=\"" << it->second << "\" />";
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

		HtmlFormInterfaceElement::~HtmlFormInterfaceElement()
		{
			delete _name;
			delete _function_parameters;
			delete _function_key;
			delete _action_key;
			delete _action_parameters;
		}
	}
}
