
/** DBLogEntry class implementation.
	@file DBLogEntry.cpp

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

#include "DBLogEntry.h"

using namespace boost;

namespace synthese
{
	namespace dblog
	{
		void DBLogEntry::setDate( const time::DateTime& date )
		{
			_date = date;
		}

		void DBLogEntry::setUser(shared_ptr<const security::User> user )
		{
			_user = user;
		}

		void DBLogEntry::setLogKey( const std::string& key )
		{
			_logKey = key;
		}
		void DBLogEntry::setContent( const Content& content )
		{
			_content = content;
		}

		time::DateTime DBLogEntry::getDate() const
		{
			return _date;
		}

		shared_ptr<const security::User> DBLogEntry::getUser() const
		{
			return _user;
		}

		const DBLogEntry::Content& DBLogEntry::getContent() const
		{
			return _content;
		}

		const std::string& DBLogEntry::getLogKey() const
		{
			return _logKey;
		}

		DBLogEntry::Level DBLogEntry::getLevel() const
		{
			return _level;
		}

		void DBLogEntry::setLevel( Level level )
		{
			_level = level;
		}
	}
}
