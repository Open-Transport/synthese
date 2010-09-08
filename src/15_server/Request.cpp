
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

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace html;
	using namespace security;

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



		void Request::deleteSession()
		{
			delete _session;
			_session = NULL;
		}



		ParametersMap Request::_getParametersMap(
		) const	{
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

			if(!_actionErrorMessage.empty())
			{
				result.insert(Request::PARAMETER_ACTION_FAILED, true);
				result.insert(Request::PARAMETER_ERROR_MESSAGE, _actionErrorMessage);
			}

			return result;
		}



		void Request::run( std::ostream& stream )
		{
			// Handle of the action
			if (_action.get())
			{
				try
				{
					_loadAction();

					if(	!_action->isAuthorized(_session)
					){
						_actionErrorMessage = "Forbidden Action";
					}
					else
					{
						ServerModule::SetCurrentThreadRunningAction();

						// Run of the action
						_action->run(*this);
					}
				}
				catch(ActionException& e)
				{
					_actionErrorMessage = "Action error : "+ e.getMessage();
				}
			}

			if(_function.get())
			{
				_loadFunction(_actionErrorMessage, _actionCreatedId);

				// Run after the action
				if (_action.get() && _redirectAfterAction)
				{
					_deleteAction();
					throw RedirectException(getURL(), false);
				}

				if (!_function->isAuthorized(_session))
				{
					throw ForbiddenRequestException();
				}

				// Run the display
				ServerModule::SetCurrentThreadRunningFunction();
				_function->run(stream, *this);
			}
		}



		bool Request::isActionFunctionAuthorized() const
		{
			if (_session == NULL ||
				_session->getUser() == NULL ||
				_session->getUser()->getProfile() == NULL
			){
				return false;
			}
			return (!_action.get() || _action->isAuthorized(_session))
				&& (!_function.get() || _function->isAuthorized(_session));
		}



		boost::shared_ptr<const security::User> Request::getUser() const
		{
			if (_session)
				return _session->getUser();
			return boost::shared_ptr<User>();
		}



		std::string Request::getOutputMimeType()
		{
			return _function.get() ? _function->getOutputMimeType() : string();
		}



		std::string Request::getURL( bool normalize /*= true*/, bool absolute ) const
		{
			std::stringstream str;
			if(absolute)
			{
				str << "http://" << _hostName;
				if(_clientURL.empty() || _clientURL[0] != '/')
				{
					str << "/";
				}
			}
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
			_redirectAfterAction(true)
		{
		}



		Request::Request(
			const Request& request,
			boost::shared_ptr<Action> action,
			boost::shared_ptr<Function> function
		):
			_session(request._session),
			_actionWillCreateObject(false),
			_redirectAfterAction(request._redirectAfterAction),
			_clientURL(request._clientURL),
			_hostName(request._hostName),
			_ip(request._ip),
			_action(action),
			_function(function)
		{
		}



		void Request::setSession( Session* session )
		{
			_session = session;
		}
	}
}
