
/** DynamicRequest class implementation.
	@file DynamicRequest.cpp

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

// At first to avoid the Windows bug "WinSock.h has already been included"
#include "ServerModule.h"

#include "DynamicRequest.h"
#include "RequestException.h"
#include "HTTPRequest.hpp"
#include "Action.h"
#include "Function.h"
#include "SessionException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	namespace server
	{
		using namespace util;



		void DynamicRequest::_loadAction(
		){
			if(_action.get())
			{
				_action->_setFromParametersMap(_parametersMap);
			}
		}



		void DynamicRequest::_loadFunction(
 			const string& errorMessage,
			optional<RegistryKeyType> actionCreatedId
		){
			// Last action error
			if(!errorMessage.empty())
			{
				_parametersMap.insert(Request::PARAMETER_ACTION_FAILED, true);
				_parametersMap.insert(Request::PARAMETER_ERROR_MESSAGE, errorMessage);
			}
			if(actionCreatedId)
			{
				_parametersMap.insert(Request::PARAMETER_OBJECT_ID, *actionCreatedId);
			}
			_parametersMap.remove(Request::PARAMETER_ACTION_WILL_CREATE_OBJECT);

			// Function parameters
			if(_function.get())
			{
				_function->_setFromParametersMap(_parametersMap);
			}
		}



		DynamicRequest::DynamicRequest( const HTTPRequest& httpRequest ):
			Request()
		{
			_ip = httpRequest.ipaddr;

			// IP
			if (_ip.empty())
			{
				util::Log::GetInstance().warn("Query without IP : a bad client is attempting to connect, or there was an attack.");
				throw RequestException("Client IP not found in parameters.");
			}

			// Host
			HTTPRequest::Headers::const_iterator it(httpRequest.headers.find("X-Forwarded-Host"));
			if(it != httpRequest.headers.end() && !it->second.empty())
			{
				_hostName = it->second;
			} else {
				it = httpRequest.headers.find("Host");
				if(it != httpRequest.headers.end())
				{
					_hostName = it->second;
				}
			}

			// Client URL
			const string& uri(httpRequest.uri);

			size_t separator(uri.find_first_of(PARAMETER_STARTER));
			if(separator == string::npos)
			{
				_clientURL = uri;
			}
			else
			{
				_clientURL = uri.substr(0, separator);
			}

			// Parameters
			_parametersMap = ParametersMap(httpRequest.postData);
			if(separator+1 < uri.length())
			{
				ParametersMap getMap(uri.substr(separator+1));
				_parametersMap.merge(getMap);
			}

			// Action will create object
			if(_parametersMap.getDefault<bool>(Request::PARAMETER_ACTION_WILL_CREATE_OBJECT, false))
			{
				_actionWillCreateObject = true;
			}

			if(_parametersMap.getOptional<bool>(Request::PARAMETER_NO_REDIRECT_AFTER_ACTION))
			{
				_redirectAfterAction = !_parametersMap.get<bool>(Request::PARAMETER_NO_REDIRECT_AFTER_ACTION);
			}

			// Session
			string sid(_parametersMap.getDefault<string>(Request::PARAMETER_SESSION));
			if (sid.empty())
			{
				_session = NULL;
			}
			else
			{
				ServerModule::SessionMap::iterator sit = ServerModule::getSessions().find(sid);
				if (sit == ServerModule::getSessions().end())
				{
					_session = NULL;
				}
				else
				{
					try
					{
						sit->second->controlAndRefresh(_ip);
						_session = sit->second;
					}
					catch (SessionException e)
					{
						deleteSession();
					}
				}
			}

			// Action name
			std::string actionName(_parametersMap.getDefault<std::string>(Request::PARAMETER_ACTION));
			if (!actionName.empty())
			{
				if (!util::Factory<Action>::contains(actionName))
				{
					throw RequestException("Action not found");
				}
				_action = boost::shared_ptr<Action>(util::Factory<Action>::create(actionName));
			}

			// Function name
			std::string functionName(_parametersMap.getDefault<std::string>(Request::PARAMETER_FUNCTION));
			if(!functionName.empty())
			{
				if(	!util::Factory<Function>::contains(functionName))
				{
					throw RequestException("Function not found");
				}
				_function = boost::shared_ptr<Function>(util::Factory<Function>::create(functionName));
			}
		}
	}
}
