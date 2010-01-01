
/** Request class implementation.
	@file Request.cpp

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

#include "Request.h"

#include "ActionException.h"
#include "Session.h"
#include "SessionException.h"
#include "Action.h"
#include "Function.h"

#include "Exception.h"
#include "FactoryException.h"
#include "Conversion.h"
#include "Log.h"

#include "DateTime.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace html;
	using namespace security;
	using namespace time;

	namespace server
	{


		const string Request::PARAMETER_SEPARATOR("&");
		const string Request::PARAMETER_ASSIGNMENT("=");

		const string Request::PARAMETER_STARTER("?");
		const string Request::PARAMETER_FUNCTION("fonction");
		const string Request::PARAMETER_SESSION("sid");
		const string Request::PARAMETER_OBJECT_ID("roid");
		const string Request::PARAMETER_ACTION("a");
		const string Request::PARAMETER_ACTION_FAILED("raf");
		const string Request::PARAMETER_ERROR_MESSAGE("rem");
		const string Request::PARAMETER_ACTION_WILL_CREATE_OBJECT("co");
		const string Request::PARAMETER_NO_REDIRECT_AFTER_ACTION("nr");




// 		Request::Request(
// 			const Request& request,
// 			shared_ptr<Action> action,
// 			shared_ptr<Function> function
// 		):	_session(request._session),
// 			_actionException(false),
// 			_errorLevel(REQUEST_ERROR_NONE),
// 			_actionWillCreateObject(false),
// 			_redirectAfterAction(request._redirectAfterAction),
// 			_clientURL(request._clientURL)
// 		{
// 			_setAction(action);
// 			_setFunction(function);
// 			if(!request._getFunction().get() || !_function.get())
// 			{
// 				return;
// 			}
// 			if (_function->getFactoryKey() == request._getFunction()->getFactoryKey())
// 			{
// 				_function->_copy(request._getFunction());
// 			}
// 			_function->setEnv(request._getFunction()->getEnv());
// 		}



		void Request::deleteSession()
		{
			delete _session;
			_session = NULL;
		}



		ParametersMap Request::_getParametersMap() const
		{
			ParametersMap result;

			// Function
			if (_getFunction().get())
			{
				result.insert(Request::PARAMETER_FUNCTION, _getFunction()->getFactoryKey());
				ParametersMap::Map functionMap(_getFunction()->_getParametersMap().getMap());
				for (ParametersMap::Map::const_iterator it = functionMap.begin(); it != functionMap.end(); ++it)
					result.insert(it->first, it->second);
			}

			// Action name and parameters
			if (_getAction().get())
			{
				result.insert(Request::PARAMETER_ACTION, _getAction()->getFactoryKey());
				ParametersMap::Map actionMap(_getAction()->getParametersMap().getMap());
				for (ParametersMap::Map::const_iterator it = actionMap.begin(); it != actionMap.end(); ++it)
					result.insert(it->first, it->second);
				result.insert(Request::PARAMETER_ACTION_WILL_CREATE_OBJECT, _actionWillCreateObject);
			}

			// Session
			if(_session)
			{
				result.insert(Request::PARAMETER_SESSION, _session->getKey());
			}

			// Object ID
			if (_actionCreatedId)
				result.insert(Request::PARAMETER_OBJECT_ID, *_actionCreatedId);

			// No redirection
			if(!_redirectAfterAction)
			{
				result.insert(Request::PARAMETER_NO_REDIRECT_AFTER_ACTION, 1);
			}

			return result;
		}



		void Request::run( std::ostream& stream )
		{
			bool actionException(false);

			// Handle of the action
			try
			{
				_loadAction();

				if (_getAction().get())
				{
					if(	_getAction()->_isSessionRequired() &&
						(!_session || !_session->getUser() || !_session->getUser()->getProfile())
					){
						actionException = true;
						_errorMessage = "Active session required for this action! Action dropped.";
					}
					else if (!_getAction()->_isAuthorized())
					{
						actionException = true;
						_errorMessage = "Forbidden Action";
					}
					else
					{
						ServerModule::SetCurrentThreadRunningAction();

						// Run of the action
						_getAction()->run(*this);
					}
				}
				catch(ActionException& e)
				{
					actionException = true;
					_errorMessage = "Action error : "+ e.getMessage();
				}
			}

			_loadFunction(actionException, _errorMessage, _actionCreatedId);

			// Run after the action
			if (_getAction().get() && _redirectAfterAction)
			{
				_deleteAction();
				throw RedirectException(getURL());
			}

			if (!_getFunction()->_isAuthorized())
			{
				throw ForbiddenRequestException();
			}

			// Run the display
			ServerModule::SetCurrentThreadRunningFunction();
			_getFunction()->run(stream, *this);
		}



		bool Request::isActionFunctionAuthorized() const
		{
			if (_session == NULL ||
				_session->getUser() == NULL ||
				_session->getUser()->getProfile() == NULL
			){
				return false;
			}
			return (!_getAction().get() || _getAction()->isAuthorized(*_session->getUser()->getProfile()))
				&& (!_getFunction.get() || _getFunction()->isAuthorized(*_session->getUser()->getProfile()));
		}



		boost::shared_ptr<const security::User> Request::getUser() const
		{
			if (_session)
				return _session->getUser();
			return boost::shared_ptr<User>();
		}



		std::string Request::getOutputMimeType()
		{
			return _getFunction().get() ? _getFunction()->getOutputMimeType() : string();
		}



		std::string Request::getURL( bool normalize /*= true*/ ) const
		{
			std::stringstream str;
			str << _clientURL << Request::PARAMETER_STARTER << getURI();
			return str.str();
		}



		html::HTMLForm Request::getHTMLForm( std::string name/*=std::string()*/ ) const
		{
			html::HTMLForm form(name, _clientURL);
			ParametersMap::Map map(_getParametersMap().getMap());
			for (ParametersMap::Map::const_iterator it = map.begin(); it != map.end(); ++it)
				form.addHiddenField(it->first, it->second);
			return form;
		}



		std::string Request::getURI() const
		{
			return _getParametersMap().getURI();
		}



		Request::Request(
		):	_session(NULL),
			_actionWillCreateObject(false),
			_redirectAfterAction(true),
			_actionException(false),
		{
		}



		Request::Request( const Request& request):
			_session(request._session),
			_actionWillCreateObject(false),
			_redirectAfterAction(request._redirectAfterAction),
			_clientURL(request._clientURL),
			_actionException(false)
		{
		}



		Request::Request(
			const HTTPRequest& httpRequest
		):	_ip(httpRequest.ipaddr)
		{
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
			ParametersMap map(httpRequest.postData);
			if(separator+1 < uri.length())
			{
				ParametersMap getMap(uri.substr(separator+1));
				map.merge(getMap);
			}

			// Action will create object
			if(map.getDefault<bool>(Request::PARAMETER_ACTION_WILL_CREATE_OBJECT, false))
			{
				_actionWillCreateObject = true;
			}

			if(map.getOptional<bool>(Request::PARAMETER_NO_REDIRECT_AFTER_ACTION))
			{
				_redirectAfterAction = !map.get<bool>(Request::PARAMETER_NO_REDIRECT_AFTER_ACTION);
			}

			// Session
			string sid(map.getDefault<string>(Request::PARAMETER_SESSION));
			if (sid.empty())
			{
				_session = NULL;
				_sessionBroken = false;
			}
			else
			{
				ServerModule::SessionMap::iterator sit = ServerModule::getSessions().find(sid);
				if (sit == ServerModule::getSessions().end())
				{
					_session = NULL;
					_sessionBroken = true;
				}
				else
				{
					try
					{
						sit->second->controlAndRefresh(_ip);
						_session = sit->second;
						_sessionBroken = false;
					}
					catch (SessionException e)
					{
						deleteSession();
						_sessionBroken = true;
					}
				}
			}

		}
	}
}
