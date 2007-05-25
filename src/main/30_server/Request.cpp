
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

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <string>

#include "01_util/Exception.h"
#include "01_util/FactoryException.h"
#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include "05_html/HTMLForm.h"

#include "30_server/ActionException.h"
#include "30_server/ServerModule.h"
#include "30_server/Session.h"
#include "30_server/SessionException.h"
#include "30_server/Action.h"
#include "30_server/Function.h"
#include "30_server/RequestException.h"
#include "30_server/Request.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace util;
	using namespace html;
	using namespace security;

	namespace server
	{

		const std::string Request::PARAMETER_ACTION ("a");
		const std::string Request::PARAMETER_SEPARATOR ("&");
		const std::string Request::PARAMETER_STARTER ("?");
		const std::string Request::PARAMETER_ASSIGNMENT ("=");
		const int Request::MAX_REQUEST_SIZE (4096);
		const std::string Request::PARAMETER_FUNCTION = "fonction";
		const std::string Request::PARAMETER_SESSION = "sid";
		const std::string Request::PARAMETER_IP = "ipaddr";
		const std::string Request::PARAMETER_CLIENT_URL = "clienturl";
		const std::string Request::PARAMETER_OBJECT_ID = "roid";
		const std::string Request::PARAMETER_ACTION_FAILED = "raf";
		const std::string Request::PARAMETER_ERROR_MESSAGE = "rem";
		const std::string Request::PARAMETER_ERROR_LEVEL = "rel";
		const uid Request::UID_WILL_BE_GENERATED_BY_THE_ACTION = -2;

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

		std::string Request::_normalizeQueryString(const std::string& requestString)
		{
			if (requestString.empty())
				return string();

			std::string s (requestString);

			// The + characters are added by the web browsers instead of spaces
			boost::algorithm::replace_all (s, "+", " ");

			// Deletes the end of line code
			size_t pos(s.size() - 1);
			for (; pos && (s.substr(pos, 1) == "\r" || s.substr(pos, 1) == "\n"); --pos);
			s = s.substr(0, pos+1);

			/* ?? what do we do with this code ?
			if (s.size () > MAX_REQUEST_SIZE) {
			bool parameterTruncated = (s.substr (MAX_REQUEST_SIZE, 1) != PARAMETER_SEPARATOR);
			s = s.substr (0, MAX_REQUEST_SIZE);

			// Filter last parameter which if it has been truncated
			if (parameterTruncated) 
			{
			s = s.substr (0, s.rfind (PARAMETER_SEPARATOR));
			}
			}
			*/
			return s;
		}

		std::string Request::getQueryString() const
		{
			// Serialize the parameter lists in a synthese querystring
			std::stringstream ss;

			ParametersMap map = _getParametersMap();
			for (ParametersMap::const_iterator iter = map.begin(); 
				iter != map.end(); 
				++iter )
			{
				if (iter != map.begin ()) ss << PARAMETER_SEPARATOR;
				ss << iter->first << PARAMETER_ASSIGNMENT << iter->second;
			}

			return _normalizeQueryString(ss.str());

		}

		void Request::run( std::ostream& stream )
		{
			// Handle of the action
			if (_action.get() && !_actionException)
			{
				if (!_action->_beforeSessionControl()
					&& _session == NULL && _action->_runBeforeActionIfNoSession())
						return;

				try
				{
					if (!_action->_isAuthorized())
					{
						stream << "Forbidden";
						return;
					}

					// Run of the action
					_action->run();
					
					// Run after the action
					if (_function->_runAfterSucceededAction(stream))	// Overloaded method
						return;
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


		std::string Request::getURL() const
		{
			stringstream str;
			str << _clientURL << PARAMETER_STARTER << getQueryString();
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

		ParametersMap Request::_parseString( const std::string& text )
		{
			ParametersMap map;
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep(PARAMETER_SEPARATOR.c_str ());

			// Parsing
			tokenizer parametersTokens (text, sep);
			for (tokenizer::iterator parameterToken = parametersTokens.begin();
				parameterToken != parametersTokens.end (); ++ parameterToken)
			{
				size_t pos = parameterToken->find (PARAMETER_ASSIGNMENT);
				if (pos == string::npos) continue;

				std::string parameterName (parameterToken->substr (0, pos));
				std::string parameterValue (parameterToken->substr (pos+1));

				map.insert (make_pair (parameterName, parameterValue));
			}
			return map;
		}

		HTMLForm Request::getHTMLForm(std::string name) const
		{
			HTMLForm form(name, _clientURL);
			ParametersMap map = _getParametersMap();
			for (ParametersMap::const_iterator it = map.begin(); it != map.end(); ++it)
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
			ParametersMap map;
			
			// Function
			if (_function.get())
			{
				map.insert(make_pair(PARAMETER_FUNCTION, _function->getFactoryKey()));
				ParametersMap functionMap = _function->_getParametersMap();
				for (ParametersMap::const_iterator it = functionMap.begin(); it != functionMap.end(); ++it)
					map.insert(make_pair(it->first, it->second));
			}
			
			// Action name and parameters
			if (_action != NULL)
			{
				map.insert(make_pair(PARAMETER_ACTION, _action->getFactoryKey()));
				ParametersMap actionMap = _action->getParametersMap();
				for (ParametersMap::const_iterator it = actionMap.begin(); it != actionMap.end(); ++it)
					map.insert(make_pair(it->first, it->second));
			}

			// Session
			if (_session != NULL)
				map.insert(make_pair(PARAMETER_SESSION, _session->getKey()));

			// Object ID
			if (_object_id)
				map.insert(make_pair(PARAMETER_OBJECT_ID, Conversion::ToString(_object_id)));

			return map;
		}

		Request::Request(const std::string& text )
			: _session(NULL)
			, _actionException(false)
			, _errorLevel(REQUEST_ERROR_NONE)
			, _object_id(0)
		{
			std::string s (_normalizeQueryString(text));
			ParametersMap map = _parseString(s);

			// Function name
			ParametersMap::iterator it = map.find(PARAMETER_FUNCTION);
			if (it == map.end())
				throw RequestException("Function not specified");
			if (!Factory<Function>::contains(it->second))
				throw RequestException("Function not found");
			_setFunction(Factory<Function>::create(it->second));

			// Object ID
			it = map.find(PARAMETER_OBJECT_ID);
			if (it != map.end())
			{
				_object_id = Conversion::ToLongLong(it->second);
			}

			// Action name
			it = map.find(PARAMETER_ACTION);
			if (it != map.end())
			{
				if (!Factory<Action>::contains(it->second))
					throw RequestException("Action not found");
				_setAction(Factory<Action>::create(it->second));

				// Action parameters
				try
				{
					_action->_setFromParametersMap(map);

					// Object ID update
					map[PARAMETER_OBJECT_ID] = Conversion::ToString(_object_id);
				}
				catch (ActionException& e)	// Action parameters error
				{
					_actionException = true;
					_errorLevel = REQUEST_ERROR_WARNING;
					_errorMessage = "Action error : "+ e.getMessage();
				}
			}
			// IP
			it = map.find(PARAMETER_IP);
			if (it == map.end())
			{
				util::Log::GetInstance().warn("Query without IP : a bad client is attempting to connect, or there was an attack.");
				throw RequestException("Client IP not found in parameters.");
			}
			_ip = it->second;

			// Session
			it = map.find(PARAMETER_SESSION);
			if (it == map.end())
			{
				_session = NULL;
				_sessionBroken = false;
			}
			else
			{
				ServerModule::SessionMap::iterator sit = ServerModule::getSessions().find(it->second);
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

			// Client URL
			it = map.find(PARAMETER_CLIENT_URL);
			if (it != map.end())
			{
				_clientURL = it->second;
			}

			// Last action error
			it = map.find(PARAMETER_ACTION_FAILED);
			if (it != map.end())
			{
				_actionException = Conversion::ToBool(it->second);
			}

			// Error message
			it = map.find(PARAMETER_ERROR_MESSAGE);
			if (it != map.end())
			{
				_errorMessage = it->second;
				_errorLevel = REQUEST_ERROR_WARNING;	// Default error level if non empty message
			}

			// Error level
			it = map.find(PARAMETER_ERROR_LEVEL);
			if (it != map.end())
			{
				_errorLevel = (ErrorLevel) Conversion::ToInt(it->second);
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
	}
}
