
/** Session class implementation.
	@file Session.cpp

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

#include <stdlib.h>
#include <boost/lexical_cast.hpp>

#include "StringUtils.hpp"
#include "Request.h"
#include "Session.h"
#include "SessionException.h"
#include "ServerModule.h"
#include "User.h"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace security;

	namespace server
	{
		const string Session::ATTR_ID = "id";
		const string Session::TAG_USER = "user";
		const string Session::ATTR_IP = "ip";
		const string Session::ATTR_LAST_USE = "last_use";

		Session::SessionMap	Session::_sessionMap;
		boost::mutex Session::_sessionMapMutex;

		const size_t Session::KEY_LENGTH = 20;
		const string Session::COOKIE_SESSIONID = "sid";



		Session::Session(
			const string& ip,
			string key
		):	_key(key.empty() ? StringUtils::GenerateRandomString(Session::KEY_LENGTH) : key),
			_ip(ip),
			_lastUse(second_clock::local_time())
		{}



		bool Session::hasProfile() const
		{
			recursive_mutex::scoped_lock lock(_mutex);
			return
				_user != NULL &&
				_user->getProfile() != NULL
			;
		}



		void Session::setSessionIdCookie(Request &request) const
		{
			recursive_mutex::scoped_lock lock(_mutex);
			request.setCookie(
				COOKIE_SESSIONID,
				this->getKey(),
				ServerModule::GetSessionMaxDuration().total_seconds()
			);
		}



		void Session::removeSessionIdCookie( Request &request ) const
		{
			recursive_mutex::scoped_lock lock(_mutex);
			request.removeCookie(COOKIE_SESSIONID);
		}



		void Session::setSessionVariable( const std::string& variable, const std::string& value )
		{
			recursive_mutex::scoped_lock lock(_mutex);
			if(!value.empty())
			{
				_sessionVariables[variable] = value;
			}
			else
			{
				SessionVariables::iterator it(_sessionVariables.find(variable));
				if(it != _sessionVariables.end())
				{
					_sessionVariables.erase(it);
				}
			}
		}



		std::string Session::getSessionVariable( const std::string& variable ) const
		{
			recursive_mutex::scoped_lock lock(_mutex);
			SessionVariables::const_iterator it(_sessionVariables.find(variable));
			return (it == _sessionVariables.end()) ? string() : it->second;
		}



		boost::shared_ptr<Session> Session::New(
			const string& ip,
			string key
		){
			mutex::scoped_lock session_lock(_sessionMapMutex);
			boost::shared_ptr<Session> session(new Session(ip, key));
			mutex::scoped_lock request_lock(session->_requestsListMutex);
			_sessionMap.insert(make_pair(session->_key, session));
			return session;
		}

		
		// Remove the given session from the map
		// @return true if the session was found and removed
		bool Session::_removeSessionFromMap()
		{
			mutex::scoped_lock lock(_sessionMapMutex);
			SessionMap::iterator it(_sessionMap.find(_key));
			if(it != _sessionMap.end())
			{
				_sessionMap.erase(it);
				return true;
			}
			return false;
		}


		void Session::Delete( boost::shared_ptr<Session> session )
		{
			session->_removeSessionFromMap();
		}


		boost::shared_ptr<Session> Session::Get(
			const std::string& key,
			const std::string& ip,
			bool exceptionIfNotFound
		){
			boost::shared_ptr<Session> session;
			{
				mutex::scoped_lock lock(_sessionMapMutex);
				SessionMap::iterator it(_sessionMap.find(key));
				if(it != _sessionMap.end())
				{
					session = it->second;
				}
			}
			if(session.get())
			{
				ptime now(second_clock::local_time());
				{
					recursive_mutex::scoped_lock lock(session->_mutex);
					if (ip != session->_ip)
					{
						throw SessionException("IP has changed during the session.");
					}
					
					if( (now - session->_lastUse) > ServerModule::GetSessionMaxDuration())
					{
						session->_removeSessionFromMap();
						throw SessionException("Session is too old");
					}
				}
				session->_lastUse = now;
			}
			else
			{
				if(exceptionIfNotFound)
				{
					throw SessionException("No such session");
				}
				else
				{
					return boost::shared_ptr<Session>();
				}
			}
			return session;
		}



		void Session::registerRequest( const Request& request )
		{
			mutex::scoped_lock lock(_requestsListMutex);
			_requests.insert(&request);
		}



		void Session::unregisterRequest( const Request& request )
		{
			mutex::scoped_lock lock(_requestsListMutex);
			_requests.erase(&request);
		}



		void Session::toParametersMap(
			util::ParametersMap& pm
		) const {

			// Id
			pm.insert(ATTR_ID, getKey());
			pm.insert("session_id", getKey()); // Backward compatiblity

			// User
			if(getUser())
			{
				boost::shared_ptr<ParametersMap> userPM(new ParametersMap);
				getUser()->toParametersMap(*userPM, true);
				pm.insert(TAG_USER, userPM);
			}

			// IP
			pm.insert(ATTR_IP, _ip);

			// Last use
			pm.insert(ATTR_LAST_USE, to_iso_extended_string(_lastUse));

			// Variables
			BOOST_FOREACH(const SessionVariables::value_type& it, _sessionVariables)
			{
				pm.insert(it.first, it.second);
			}
		}
}	}
