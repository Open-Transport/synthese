
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
#include "RequestException.h"

#include "Exception.h"
#include "FactoryException.h"
#include "Conversion.h"
#include "Log.h"

#include "HTMLForm.h"

#include "DateTime.h"
#include "HTTPRequest.hpp"

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
		const string Request::PARAMETER_SEPARATOR ("&");
		const string Request::PARAMETER_ASSIGNMENT ("=");
		const string Request::PARAMETER_STARTER ("?");
		const string Request::PARAMETER_ACTION ("a");
		const string Request::PARAMETER_FUNCTION = "fonction";
		const string Request::PARAMETER_SESSION = "sid";
		const string Request::PARAMETER_OBJECT_ID = "roid";
		const string Request::PARAMETER_ACTION_FAILED = "raf";
		const string Request::PARAMETER_ERROR_MESSAGE = "rem";
		const string Request::PARAMETER_ERROR_LEVEL = "rel";
		const string Request::PARAMETER_ACTION_WILL_CREATE_OBJECT = "co";
		const string Request::PARAMETER_NO_REDIRECT_AFTER_ACTION = "nr";


		Request::Request(
			const Request* request/*=NULL*/,
			shared_ptr<Function> function, shared_ptr<Action> action
		):	_session(NULL)
			, _actionException(false)
			, _errorLevel(REQUEST_ERROR_NONE)
			, _actionWillCreateObject(false),
			_redirectAfterAction(request ? request->_redirectAfterAction : true)
		{
			if (action.get())
				_setAction(action);
			if (function.get())
				_setFunction(function);
			if (request)
			{
				_clientURL = request->_clientURL;
				_session = request->_session;
				if (_function.get() && _function->getFactoryKey() == function->getFactoryKey())
					_function->_copy(request->_function);
			}
		}



		Request::Request(
			const HTTPRequest& httpRequest
		):	_session(NULL),
			_ip(httpRequest.ipaddr)
			, _actionException(false)
			, _errorLevel(REQUEST_ERROR_NONE)
			, _actionWillCreateObject(false),
			_redirectAfterAction(true)
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

			// Function name
			string functionName(map.getDefault<string>(Request::PARAMETER_FUNCTION));
			if (functionName.empty())
				throw RequestException("Function not specified");
			if (!Factory<Function>::contains(functionName))
				throw RequestException("Function not found");
			_setFunction(shared_ptr<Function>(Factory<Function>::create(functionName)));

			// Action name
			string actionName(map.getDefault<string>(Request::PARAMETER_ACTION));
			if (!actionName.empty())
			{
				if (!Factory<Action>::contains(actionName))
					throw RequestException("Action not found");
				_setAction(shared_ptr<Action>(Factory<Action>::create(actionName)));

				// Action parameters
				try
				{
					_action->_setFromParametersMap(map);
				}
				catch (ActionException& e)	// Action parameters error
				{
					_actionException = true;
					_errorLevel = REQUEST_ERROR_WARNING;
					_errorMessage = "Action error : "+ e.getMessage();
				}

				if(_action->_isSessionRequired() &&	_session == NULL)
				{
					_actionException = true;
					_errorMessage = "Active session required for this action! Action dropped.";
					_errorLevel = REQUEST_ERROR_WARNING;
				}
				else if (!_action->_isAuthorized())
				{
					_actionException = true;
					_errorMessage = "Forbidden Action";
					_errorLevel = REQUEST_ERROR_WARNING;
				}
			}

			// Last action error
			optional<int> num(map.getOptional<int>(Request::PARAMETER_ERROR_LEVEL));
			if (!_actionException || (num && static_cast<ErrorLevel>(*num) > _errorLevel))
			{
				_actionException = map.getDefault<bool>(Request::PARAMETER_ACTION_FAILED, false);

				// Error message
				_errorMessage = map.getDefault<string>(Request::PARAMETER_ERROR_MESSAGE);
				if (!_errorMessage.empty() && _errorLevel < REQUEST_ERROR_WARNING)
					_errorLevel = REQUEST_ERROR_WARNING;	// Default error level if non empty message

				// Error level
				if (num)
				{
					_errorLevel = static_cast<ErrorLevel>(*num);
				}
			}

			// Function parameters
			_function->_setFromParametersMap(map);

			if (!_function->_isAuthorized())
			{
				throw ForbiddenRequestException();
			}
		}



		std::string Request::getURI() const
		{
			return _getParametersMap().getURI();
		}


		
		void Request::run( std::ostream& stream )
		{
			// Handle of the action
			if (_action.get() && !_actionException)
			{
				try
				{
					// Run of the action
					ServerModule::SetCurrentThreadRunningAction();
					_action->run();
					
					// Run after the action
					if (_redirectAfterAction)
					{
						deleteAction();
						throw RedirectException(getURL());
					}
				}
				catch (ActionException e)	// Action run error
				{
					_actionException = true;
					_errorMessage = e.getMessage();
					_errorLevel = REQUEST_ERROR_WARNING;
				}
			}

			// No session is active.
			if (_session == NULL && _function->_runBeforeDisplayIfNoSession(stream))
				return;
			
			// Run the display
			ServerModule::SetCurrentThreadRunningFunction();
			_function->_run(stream);
		}



		void Request::_setAction(shared_ptr<Action> action )
		{
			_action = action;
			action->_request = this;
		}

		void Request::deleteSession()
		{
			delete _session;
			_session = NULL;
		}


		std::string Request::getURL(bool normalize) const
		{
			stringstream str;
			str << _clientURL << Request::PARAMETER_STARTER << getURI();
			return str.str();
		}



		const std::string& Request::getClientURL() const
		{
			return _clientURL;
		}

		const std::string& Request::getIP() const
		{
			return _ip;
		}

		void Request::setSession( Session* session )
		{
			_session = session;
		}



		HTMLForm Request::getHTMLForm(std::string name) const
		{
			HTMLForm form(name, _clientURL);
			ParametersMap::Map map(_getParametersMap().getMap());
			for (ParametersMap::Map::const_iterator it = map.begin(); it != map.end(); ++it)
				form.addHiddenField(it->first, it->second);
			return form;
		}

		shared_ptr<const Action> Request::_getAction() const
		{
			return _action;
		}

		boost::shared_ptr<Action> Request::_getAction()
		{
			return _action;
		}


		const std::string& Request::getErrorMessage() const
		{
			return _errorMessage;
		}

		ParametersMap Request::_getParametersMap() const
		{
			ParametersMap result;
			
			// Function
			if (_function.get())
			{
				result.insert(Request::PARAMETER_FUNCTION, _function->getFactoryKey());
				ParametersMap::Map functionMap(_function->_getParametersMap().getMap());
				for (ParametersMap::Map::const_iterator it = functionMap.begin(); it != functionMap.end(); ++it)
					result.insert(it->first, it->second);
			}
			
			// Action name and parameters
			if (_action.get())
			{
				result.insert(Request::PARAMETER_ACTION, _action->getFactoryKey());
				ParametersMap::Map actionMap(_action->getParametersMap().getMap());
				for (ParametersMap::Map::const_iterator it = actionMap.begin(); it != actionMap.end(); ++it)
					result.insert(it->first, it->second);
				result.insert(Request::PARAMETER_ACTION_WILL_CREATE_OBJECT, _actionWillCreateObject);
			}

			// Session
			result.insert(Request::PARAMETER_SESSION, _session ? _session->getKey() : string());

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



		bool Request::getActionException() const
		{
			return _actionException;
		}

		void Request::_setErrorMessage( const std::string& message )
		{
			_errorMessage = message;
		}

		void Request::_setActionException( bool value )
		{
			_actionException = value;
		}

		void Request::_setErrorLevel( const ErrorLevel& level )
		{
			_errorLevel = level;
		}

		void Request::_setFunction(shared_ptr<Function> function )
		{
			_function = function;
			function->_request = this;
		}

		void Request::setClientURL( const std::string& url )
		{
			_clientURL = url;
		}

		shared_ptr<Function> Request::_getFunction()
		{
			return _function;
		}

		shared_ptr<const Function> Request::_getFunction() const
		{
			return _function;
		}

		void Request::deleteAction()
		{
			_action.reset();
		}

		shared_ptr<const User> Request::getUser() const
		{
			if (_session)
				return _session->getUser();
			return shared_ptr<User>();

		}

		bool Request::isActionFunctionAuthorized() const
		{
			return (!_action.get() || _action->_isAuthorized())
				&& (!_function.get() || _function->_isAuthorized());
		}

		const Session* Request::getSession() const
		{
			return _session;
		}



		std::string Request::getOutputMimeType()
		{
			return _function.get() ? _function->getOutputMimeType() : string();
		}

		void Request::setHostName( const std::string& value )
		{
			_hostName = value;
		}

		const std::string& Request::getHostName() const
		{
			return _hostName;
		}
		
		const optional<RegistryKeyType>& Request::getActionCreatedId() const
		{
			return _actionCreatedId;
		}
		
		bool Request::getActionWillCreateObject() const
		{
			return _actionWillCreateObject;
		}
	
		void Request::setActionCreatedId(util::RegistryKeyType id)
		{
			if(_actionWillCreateObject) _actionCreatedId = id;
		}
		
		void Request::setActionWillCreateObject()
		{
			_actionWillCreateObject = true;
		}



		Request::RedirectException::RedirectException( const std::string& location )
			: _location(location)
		{

		}



		const std::string& Request::RedirectException::getLocation()
		{
			return _location;
		}
	}
}
