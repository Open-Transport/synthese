
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
		const size_t Session::KEY_LENGTH = 20;
		const string Session::COOKIE_SESSIONID = "sid";


		Session::Session(const std::string& ip)
			: _ip(ip)
			, _key(StringUtils::GenerateRandomString(Session::KEY_LENGTH))
			, _lastUse(second_clock::local_time())
		{
			ServerModule::getSessions().insert(make_pair(_key, this));
		}



		void Session::checkAndRefresh(const std::string& ip)
		{
			if (ip != _ip)
				throw SessionException("IP has changed during the session.");

			ptime now(second_clock::local_time());
			if( (now - _lastUse) > ServerModule::GetSessionMaxDuration())
			{
				throw SessionException("Session is too old");
			}

			_lastUse = now;
		}



		Session::~Session()
		{
			ServerModule::SessionMap::iterator it = ServerModule::getSessions().find(_key);
			if (it != ServerModule::getSessions().end())
			{
				ServerModule::getSessions().erase(it);
			}
		}



		void Session::setUser(shared_ptr<const User> user )
		{
			_user = user;
		}



		const std::string Session::getKey() const
		{
			return _key;
		}



		shared_ptr<const security::User> Session::getUser() const
		{
			return _user;
		}



		bool Session::hasProfile() const
		{
			return
				_user != NULL &&
				_user->getProfile() != NULL
			;
		}



		void Session::setSessionIdCookie(Request &request) const
		{
			request.setCookie(
				COOKIE_SESSIONID,
				this->getKey(),
				ServerModule::GetSessionMaxDuration().total_seconds()
			);
		}



		void Session::removeSessionIdCookie( Request &request ) const
		{
			request.removeCookie(COOKIE_SESSIONID);
		}



		void Session::setSessionVariable( const std::string& variable, const std::string& value )
		{
			mutex::scoped_lock lock(_mutex);
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
			mutex::scoped_lock lock(_mutex);
			SessionVariables::const_iterator it(_sessionVariables.find(variable));
			return (it == _sessionVariables.end()) ? string() : it->second;
		}
}	}
