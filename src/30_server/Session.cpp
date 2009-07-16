
/** Session class implementation.
	@file Session.cpp

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

#include <time.h>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>

#include "DateTime.h"

#include "Session.h"
#include "SessionException.h"
#include "ServerModule.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace time;
	using namespace security;

	namespace server
	{
		const size_t Session::KEY_LENGTH = 20;
		const int Session::MAX_MINUTES_DURATION = 30;

		Session::Session(const std::string& ip)
			: _ip(ip)
			, _key(generateKey())
			, _lastUse(TIME_CURRENT)
		{
			ServerModule::getSessions().insert(make_pair(_key, this));
		}

		void Session::controlAndRefresh(const std::string& ip)
		{
			if (ip != _ip)
				throw SessionException("IP has changed during the session.");

			DateTime now(TIME_CURRENT);
			if (now - _lastUse > MAX_MINUTES_DURATION)
				throw SessionException("Session is too old");

			_lastUse = now;
		}

		std::string Session::generateKey()
		{
			std::string key;
			srand( (unsigned) ::time( NULL ) );
			for (size_t i=0; i< Session::KEY_LENGTH; ++i)
			{
				key += lexical_cast<string>(rand());
			}
			return key;
		}

		Session::~Session()
		{
			ServerModule::SessionMap::iterator it = ServerModule::getSessions().find(_key);
			if (it != ServerModule::getSessions().end())
				ServerModule::getSessions().erase(it);
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
	}
}
