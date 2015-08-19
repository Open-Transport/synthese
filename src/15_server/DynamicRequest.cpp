
/** DynamicRequest class implementation.
	@file DynamicRequest.cpp

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

// At first to avoid the Windows bug "WinSock.h has already been included"
#include "ServerModule.h"

#include "Action.h"
#include "DynamicRequest.h"
#include "Function.h"
#include "HTTPRequest.hpp"
#include "RequestException.h"
#include "ServerConstants.h"
#include "Session.h"
#include "SessionException.h"
#include "User.h"
#include "UserException.h"
#include "UserTableSync.h"
#include "WebPageDisplayFunction.h"

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace cms;
	
	namespace server
	{
		using namespace util;
		using namespace security;



		void DynamicRequest::_loadAction(
		){
			if(_action.get())
			{
				_action->_setFromParametersMap(_allParametersMap);
				name = _action->getFactoryKey();
			}
		}



		void DynamicRequest::_loadFunction(
 			const string& errorMessage,
			optional<RegistryKeyType> actionCreatedId
		){
			// Last action error
			if(!errorMessage.empty())
			{
				_allParametersMap.insert(Request::PARAMETER_ACTION_FAILED, true);
				_allParametersMap.insert(Request::PARAMETER_ERROR_MESSAGE, errorMessage);
			}
			if(actionCreatedId)
			{
				_allParametersMap.insert(Request::PARAMETER_OBJECT_ID, *actionCreatedId);
				_getPostParametersMap.insert(Request::PARAMETER_OBJECT_ID, *actionCreatedId);
			}
			_allParametersMap.remove(Request::PARAMETER_ACTION_WILL_CREATE_OBJECT);
			_getPostParametersMap.remove(Request::PARAMETER_ACTION_WILL_CREATE_OBJECT);

			// Function parameters
			if(_function.get())
			{
				ParametersMap templateParametersMap;
				BOOST_FOREACH(const ParametersMap::Map::value_type& it, _getPostParametersMap.getMap())
				{
					if(	it.first != Request::PARAMETER_FUNCTION &&
						it.first != Request::PARAMETER_SERVICE &&
						it.first != Request::PARAMETER_ACTION &&
						(it.first.size() < Action_PARAMETER_PREFIX.size() || it.first.substr(0, Action_PARAMETER_PREFIX.size()) != Action_PARAMETER_PREFIX)
					){
						templateParametersMap.insert(it.first, it.second);
					}
				}
				_function->setTemplateParameters(templateParametersMap);
				_function->_setFromParametersMap(_allParametersMap);
				name = _function->getFactoryKey();
			}
		}



		void DynamicRequest::_setupSession()
		{
			// Reinitialization of the session pointer (useless in standard situation)
			if(_session.get())
			{
				setSession(boost::shared_ptr<Session>());
			}

			// Fetching of the session id 
			string sid(_allParametersMap.getDefault<string>(Request::PARAMETER_SESSION));
			if(sid.empty())
			{
				return;
			}

			try
			{
				// Try to open an existing session
				boost::shared_ptr<Session> session(Session::Get(sid, _ip, false));

				if(session.get())
				{
					setSession(session);
				}
				// If not found try to create a session from a "user:password" sid.
				else
				{
					// Decode the string
					size_t colonIndex = sid.find(":");
					if(colonIndex == string::npos)
					{
						return;
					}
					string login(sid.substr(0, colonIndex));
					string password(sid.substr(colonIndex + 1));

					// Checks if the login and the password are not empty
					if(login.empty() || password.empty())
					{
						return;
					}

					// Loading of the user from the table
					try
					{
						boost::shared_ptr<User> user = UserTableSync::getUserFromLogin(login);
						user->verifyPassword(password);

						if(!user->getConnectionAllowed())
						{
							Log::GetInstance().warn("Connection not allowed");
							return;
						}

						setSession(boost::shared_ptr<Session>(Session::New(_ip, sid)));
						_session->setUser(user);
					}
					catch(UserException&)
					{
						Log::GetInstance().warn("Wrong password");
					}
					catch(...)
					{
						Log::GetInstance().warn("User not found");
					}
				}
			}
			catch(SessionException& e)
			{
				Log::GetInstance().debug("Session "+ sid +" rejected", e);
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
				// If there are multiple hosts in the x-forwarded-host chain, use the first one.
				size_t commaPos = it->second.find(",");
				if(commaPos != string::npos)
				{
					_hostName = it->second.substr(0, commaPos);
				}
				else
				{
					_hostName = it->second;
				}
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
			it = httpRequest.headers.find("Content-Type");
			if(it != httpRequest.headers.end())
			{
				vector<string> parts;
				split(parts, it->second, is_any_of(";"));
				if(	parts.size() >= 2 &&
					trim_copy(parts[0]) == "multipart/form-data"
				){
					vector<string> parts1;
					split(parts1, parts[1], is_any_of("="));
					if(parts1.size() >= 2)
					{
						_getPostParametersMap = ParametersMap(
							httpRequest.postData,
							parts1[1]
						);
					}
				}
				else if(
					parts.size() >= 1 &&
					trim_copy(parts[0]) == "application/x-www-form-urlencoded"
				){
					_getPostParametersMap = ParametersMap(httpRequest.postData);
				}
				else
				{
					if(	parts.size() >= 1)
					{
						_getPostParametersMap.insert(
							PARAMETER_POST_DATA_MIME_TYPE,
							trim_copy(parts[0])
						);
					}
					if(parts.size() >= 2)
					{
						vector<string> parts1;
						split(parts1, parts[1], is_any_of("="));
						if(parts1.size() >= 2 &&
							trim_copy(parts1[0]) == "charset"
						){
							_getPostParametersMap.insert(
								PARAMETER_POST_DATA_CHARSET,
								parts1[1]
							);
						}
					}
					_getPostParametersMap.insert(
						PARAMETER_POST_DATA,
						httpRequest.postData
					);
				}
			}
			if(separator+1 < uri.length())
			{
				ParametersMap getMap(uri.substr(separator+1));
				_getPostParametersMap.merge(getMap);
			}
			_getPostParametersMap.insert(
				PARAMETER_CLIENT_ADDRESS,
				httpRequest.ipaddr
			);

			// Cookies
			_allParametersMap = _getPostParametersMap;
			it = httpRequest.headers.find("Cookie");
			if(it != httpRequest.headers.end())
			{
				vector<string> cookies;
				boost::algorithm::split(cookies, it->second, is_any_of(";"));
				BOOST_FOREACH(const string& cookie, cookies)
				{
					vector<string> nameValue;
					string trimmedCookie(cookie);
					boost::trim(trimmedCookie);
					boost::algorithm::split(nameValue, trimmedCookie, is_any_of("="));

					// Don't override existing parameters (GET and POST have precedence).
					if (nameValue.size() != 2 || _allParametersMap.isDefined(nameValue[0]))
						continue;

					// TODO: proper unescaping of cookie values
					_allParametersMap.insert(nameValue[0], nameValue[1]);
				}
			}

			// Action will create object
			if(_getPostParametersMap.getDefault<bool>(Request::PARAMETER_ACTION_WILL_CREATE_OBJECT, false))
			{
				_actionWillCreateObject = true;
			}

			if(_getPostParametersMap.getOptional<bool>(Request::PARAMETER_NO_REDIRECT_AFTER_ACTION))
			{
				_redirectAfterAction = !_allParametersMap.get<bool>(Request::PARAMETER_NO_REDIRECT_AFTER_ACTION);
			}

			// Session
			_setupSession();
			if(!_session.get() && !ServerModule::GetAutoLoginUser().empty())
			{
				boost::shared_ptr<User> user = UserTableSync::getUserFromLogin(ServerModule::GetAutoLoginUser());
				setSession(Session::New(_ip));
				_session->setUser(user);
			}
			if(_session.get())
			{
				try
				{
					_session->setSessionIdCookie(*this);
				}
				catch (SessionException e)
				{
					deleteSession();
				}
			}

			// Action name
			std::string actionName(_getPostParametersMap.getDefault<std::string>(Request::PARAMETER_ACTION));
			if (!actionName.empty())
			{
				if (!util::Factory<Action>::contains(actionName))
				{
					throw RequestException("Action not found");
				}
				_action = boost::shared_ptr<Action>(util::Factory<Action>::create(actionName));
			}

			// Function name
			std::string functionName(_getPostParametersMap.getDefault<std::string>(Request::PARAMETER_SERVICE));
			if(functionName.empty())
			{
				functionName = _getPostParametersMap.getDefault<std::string>(Request::PARAMETER_FUNCTION);
				_getPostParametersMap.insert(Request::PARAMETER_SERVICE, functionName);
				_allParametersMap.insert(Request::PARAMETER_SERVICE, functionName);
			}
			if(functionName.empty() && _redirectAfterAction && !_clientURL.empty())
			{
				functionName = WebPageDisplayFunction::FACTORY_KEY;
				_getPostParametersMap.insert(Request::PARAMETER_SERVICE, functionName);
				_allParametersMap.insert(Request::PARAMETER_SERVICE, functionName);
				_getPostParametersMap.insert(WebPageDisplayFunction::PARAMETER_SMART_URL, _clientURL);
				_getPostParametersMap.insert(WebPageDisplayFunction::PARAMETER_HOST_NAME, _hostName);
				_allParametersMap.insert(WebPageDisplayFunction::PARAMETER_SMART_URL, _clientURL);
				_allParametersMap.insert(WebPageDisplayFunction::PARAMETER_HOST_NAME, _hostName);
			}
			if(!functionName.empty())
			{
				if(	!util::Factory<Function>::contains(functionName))
				{
					throw RequestException("Function not found");
				}
				_function = boost::shared_ptr<Function>(util::Factory<Function>::create(functionName));
			}
		}



		ParametersMap DynamicRequest::getParametersMap() const
		{
			return _allParametersMap;
		}
}	}
