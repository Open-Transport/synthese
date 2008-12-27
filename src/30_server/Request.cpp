
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

#include "Request.h"

#include "30_server/ActionException.h"
#include "30_server/ServerModule.h"
#include "30_server/Session.h"
#include "30_server/SessionException.h"
#include "30_server/Action.h"
#include "30_server/Function.h"
#include "30_server/RequestException.h"
#include "30_server/RequestMissingParameterException.h"
#include "30_server/QueryString.h"

#include "01_util/Exception.h"
#include "01_util/FactoryException.h"
#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include "05_html/HTMLForm.h"

#include "04_time/DateTime.h"

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
		Request::Request( const Request* request/*=NULL*/
			, shared_ptr<Function> function, shared_ptr<Action> action )
			: _session(NULL)
			, _actionException(false)
			, _errorLevel(REQUEST_ERROR_NONE)
			, _object_id(0)
		{
			if (action.get())
				_setAction(action);
			if (function.get())
				_setFunction(function);
			if (request)
			{
				_clientURL = request->_clientURL;
				_session = request->_session;
				if (_function.get())
					_function->_copy(request->_function);
			}
		}



		QueryString Request::getQueryString(bool normalize) const
		{
			return _getParametersMap().getQueryString(normalize);
		}


		
		void Request::run( std::ostream& stream )
		{
			// Handle of the action
			if (_action.get() && !_actionException)
			{
				if(	!_action->_beforeSessionControl()
				&&	_session == NULL && _action->_runBeforeActionIfNoSession()
				){
					_actionException = true;
					_errorMessage = "Active session required for this action! Action dropped.";
					_errorLevel = REQUEST_ERROR_WARNING;
				}
				else
					try
					{
						if (!_action->_isAuthorized())
						{
							_actionException = true;
							_errorMessage = "Forbidden Action";
							_errorLevel = REQUEST_ERROR_WARNING;
						}
						else
						{
							// Run of the action
							_action->run();

							// Run after the action
							if (_function->_runAfterSucceededAction(stream))	// Overloaded method
								return;
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
			
			if (!_function->_isAuthorized())
			{
				stream << "Forbidden";
				return;
			}

			// Run the display
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
			str << _clientURL << QueryString::PARAMETER_STARTER << getQueryString(normalize).getContent();
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


		void Request::setObjectId( uid id )
		{
			_object_id = id;
		}

		uid Request::getObjectId() const
		{
			return _object_id;
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
				result.insert(QueryString::PARAMETER_FUNCTION, _function->getFactoryKey());
				ParametersMap::Map functionMap(_function->_getParametersMap().getMap());
				for (ParametersMap::Map::const_iterator it = functionMap.begin(); it != functionMap.end(); ++it)
					result.insert(it->first, it->second);
			}
			
			// Action name and parameters
			if (_action != NULL)
			{
				result.insert(QueryString::PARAMETER_ACTION, _action->getFactoryKey());
				ParametersMap::Map actionMap(_action->getParametersMap().getMap());
				for (ParametersMap::Map::const_iterator it = actionMap.begin(); it != actionMap.end(); ++it)
					result.insert(it->first, it->second);
			}

			// Session
			result.insert(QueryString::PARAMETER_SESSION, _session ? _session->getKey() : string());

			// Object ID
			if (_object_id)
				result.insert(QueryString::PARAMETER_OBJECT_ID, _object_id);

			// Internal parameters
			ParametersMap::Map internalMap(_internalParameters.getMap());
			for (ParametersMap::Map::const_iterator it(internalMap.begin()); it != internalMap.end(); ++it)
				result.insert(it->first, it->second);
			return result;
		}


		Request::Request(const QueryString& text )
			: _session(NULL)
			, _actionException(false)
			, _errorLevel(REQUEST_ERROR_NONE)
			, _object_id(0)
		{
			ParametersMap map(text);

			// IP
			_ip = map.getString(QueryString::PARAMETER_IP, false, "Request");
			if (_ip.empty())
			{
				util::Log::GetInstance().warn("Query without IP : a bad client is attempting to connect, or there was an attack.");
				throw RequestException("Client IP not found in parameters.");
			}

			// Session
			string sid(map.getString(QueryString::PARAMETER_SESSION, false, "Request"));
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
			string functionName(map.getString(QueryString::PARAMETER_FUNCTION, false, "Request"));
			if (functionName.empty())
				throw RequestException("Function not specified");
			if (!Factory<Function>::contains(functionName))
				throw RequestException("Function not found");
			_setFunction(shared_ptr<Function>(Factory<Function>::create(functionName)));

			// Object ID
			_object_id = map.getUid(QueryString::PARAMETER_OBJECT_ID, false, "Request");

			// Action name
			string actionName(map.getString(QueryString::PARAMETER_ACTION, false, "Request"));
			if (!actionName.empty())
			{
				if (!Factory<Action>::contains(actionName))
					throw RequestException("Action not found");
				_setAction(shared_ptr<Action>(Factory<Action>::create(actionName)));

				// Action parameters
				try
				{
					_action->_setFromParametersMap(map);

					// Object ID update
					map.insert(QueryString::PARAMETER_OBJECT_ID, _object_id);
				}
				catch (ActionException& e)	// Action parameters error
				{
					_actionException = true;
					_errorLevel = REQUEST_ERROR_WARNING;
					_errorMessage = "Action error : "+ e.getMessage();
				}
			}

			// Client URL
			_clientURL = map.getString(QueryString::PARAMETER_CLIENT_URL, false, "Request");

			// Last action error
			int num = map.getInt(QueryString::PARAMETER_ERROR_LEVEL, false, string());
			if (!_actionException || (num != UNKNOWN_VALUE && static_cast<ErrorLevel>(num) > _errorLevel))
			{
				_actionException = map.getBool(QueryString::PARAMETER_ACTION_FAILED, false, false, "Request");

				// Error message
				_errorMessage = map.getString(QueryString::PARAMETER_ERROR_MESSAGE, false, "Request");
				if (!_errorMessage.empty() && _errorLevel < REQUEST_ERROR_WARNING)
					_errorLevel = REQUEST_ERROR_WARNING;	// Default error level if non empty message

				// Error level
				if (num != UNKNOWN_VALUE)
				{
					_errorLevel = static_cast<ErrorLevel>(num);
				}
			}

			// Function parameters
			_function->_setFromParametersMap(map);

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

		ParametersMap& Request::getInternalParameters()
		{
			return _internalParameters;
		}



		Request::~Request(

			) {

		}
	}
}
