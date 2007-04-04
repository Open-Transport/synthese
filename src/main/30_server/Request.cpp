
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
#include "01_util/Html.h"

#include "30_server/ActionException.h"
#include "30_server/ServerModule.h"
#include "30_server/Session.h"
#include "30_server/SessionException.h"
#include "30_server/Action.h"
#include "30_server/RequestException.h"
#include "30_server/Request.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace server
	{

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

		Request::Request()
			: _session(NULL)
			, _action(NULL)
			, _actionException(false)
			, _errorLevel(REQUEST_ERROR_NONE)
			, _object_id(0)
		{
		}

		std::string Request::truncateStringIfNeeded (const std::string& requestString)
		{
			std::string s (requestString);

			// The + characters are added by the web browsers instead of spaces
			boost::algorithm::replace_all (s, "+", " ");

			// Deletes the end of line code
			size_t pos = s.find ("\r");
			if (pos != string::npos)
				s = s.substr(0, pos);
			pos = s.find ("\n");
			if (pos != string::npos)
				s = s.substr(0, pos);


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

		Request* Request::createFromString(const std::string& text )
		{
			std::string s (truncateStringIfNeeded (text));
			ParametersMap map = parseString(s);

			// Function name
			ParametersMap::iterator it = map.find(PARAMETER_FUNCTION);
			if (it == map.end())
				throw RequestException("Function not specified");
			if (!Factory<Request>::contains(it->second))
				throw RequestException("Function not found");

			// Request instantiation
			Request* request;
			try
			{
				request = Factory<Request>::create(it->second);
			}
			catch (FactoryException<Request> e)
			{
				throw RequestException(e.getMessage());
			}
			map.erase(it);
			
			// IP
			it = map.find(PARAMETER_IP);
			if (it == map.end())
			{
				util::Log::GetInstance().warn("Query without IP : a bad client is attempting to connect, or there was an attack.");
				throw RequestException("Client IP not found in parameters.");
			}
			request->_ip = it->second;
			map.erase(it);

			// Session
			it = map.find(PARAMETER_SESSION);
			if (it == map.end())
			{
				request->_session = NULL;
				request->_sessionBroken = false;
			}
			else
			{
				ServerModule::SessionMap::iterator sit = ServerModule::getSessions().find(it->second);
				if (sit == ServerModule::getSessions().end())
				{
					request->_session = NULL;
					request->_sessionBroken = true;
				}
				else
				{
					try
					{
						sit->second->controlAndRefresh(request->_ip);
						request->_session = sit->second;
						request->_sessionBroken = false;
					}
					catch (SessionException e)
					{
						request->deleteSession();
						request->_sessionBroken = true;
					}
				}
			}

			// Client URL
			it = map.find(PARAMETER_CLIENT_URL);
			if (it != map.end())
			{
				request->_clientURL = it->second;
				map.erase(it);
			}

			// Object ID
			it = map.find(PARAMETER_OBJECT_ID);
			if (it != map.end())
			{
				request->_object_id = Conversion::ToLongLong(it->second);
			}

			// Last action error
			it = map.find(PARAMETER_ACTION_FAILED);
			if (it != map.end())
			{
				request->_actionException = Conversion::ToBool(it->second);
				map.erase(it);
			}

			// Error message
			it = map.find(PARAMETER_ERROR_MESSAGE);
			if (it != map.end())
			{
				request->_errorMessage = it->second;
				map.erase(it);
				request->_errorLevel = REQUEST_ERROR_WARNING;	// Default error level if non empty message
			}

			// Error level
			it = map.find(PARAMETER_ERROR_LEVEL);
			if (it != map.end())
			{
				request->_errorLevel = (ErrorLevel) Conversion::ToInt(it->second);
				map.erase(it);
			}

			try
			{
				// Action
				request->_action = Action::create(request, map);
			}
			catch (ActionException& e)	// Action parameters error
			{
				request->_actionException = true;
				request->_errorLevel = REQUEST_ERROR_WARNING;
				request->_errorMessage = "Action error : "+ e.getMessage();
			}
			request->setFromParametersMap(map);

			return request;
		}

		std::string Request::getQueryString() const
		{
			// Adding function name
			ParametersMap map = getParametersMap();
			map.insert(make_pair(PARAMETER_FUNCTION, getFactoryKey()));
			if (_action != NULL)
			{
				map.insert(make_pair(Action::PARAMETER_ACTION, _action->getFactoryKey()));
				ParametersMap actionMap = _action->getParametersMap();
				for (ParametersMap::const_iterator it = actionMap.begin(); it != actionMap.end(); ++it)
				{
					map.insert(make_pair(Action_PARAMETER_PREFIX + it->first, it->second));
				}
			}
			if (_session != NULL)
			{
				map.insert(make_pair(PARAMETER_SESSION, _session->getKey()));
			}

			// Serialize the parameter lists in a synthese querystring
			std::stringstream ss;

			for (ParametersMap::const_iterator iter = map.begin(); 
				iter != map.end(); 
				++iter )
			{
				if (iter != map.begin ()) ss << PARAMETER_SEPARATOR;
				ss << iter->first << PARAMETER_ASSIGNMENT << iter->second;
			}

			return truncateStringIfNeeded(ss.str());

		}

		void Request::runActionAndFunction( std::ostream& stream )
		{
			// Handle of the action
			if (_action != NULL)
			{
				try
				{
					// Run of the action
					_action->run();
					
					// Run after the action
					if (runAfterAction(stream))	// Overloaded method
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
			if ((_session == NULL) && runBeforeDisplayIfNoSession(stream))
				return;
			
			// Run the display
			run(stream);
		}

		Request::~Request()
		{
			delete _action;
		}

		void Request::setAction( Action* action )
		{
			_action = action;
		}

		void Request::deleteSession()
		{
			delete _session;
			_session = NULL;
		}

		void Request::copy( const Request* request )
		{
			_clientURL = request->_clientURL;
			_session = request->_session;
		}

		std::string Request::getHTMLLink(const std::string& content) const
		{
			std::stringstream str;
			str << "<a href=\"" << getURL() << "\">"
				<< content << "</a>";
			return str.str();
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

		Request::ParametersMap Request::parseString( const std::string& text )
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

		std::string Request::getHTMLFormHeader(const std::string& name) const
		{
			std::stringstream str;
			str	<< "<form name=\"" << name << "\" action=\"" << _clientURL << "\" method=\"post\">"
				<< Html::getHiddenInput(PARAMETER_FUNCTION, getFactoryKey());
			if (_session != NULL)
				str << Html::getHiddenInput(PARAMETER_SESSION, Conversion::ToString(_session->getKey()));
			if (_action != NULL)
				str << Html::getHiddenInput(Action::PARAMETER_ACTION, _action->getFactoryKey());
			return str.str();
		}

		const Action* Request::getAction() const
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

		bool Request::runAfterAction(ostream& stream)
		{
			return false;
		}

		bool Request::runBeforeDisplayIfNoSession( std::ostream& stream )
		{
			return false;
		}
	}
}
