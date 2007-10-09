
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

#include "13_dblog/DBLogEntry.h"

#include <sstream>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace time;

	namespace util
	{
		template<> typename Registrable<uid,dblog::DBLogEntry>::Registry Registrable<uid,dblog::DBLogEntry>::_registry;
	}

	namespace dblog
	{
		void DBLogEntry::setDate( const time::DateTime& date )
		{
			_date = date;
		}

		void DBLogEntry::setUser(const security::User* user)
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

		const security::User* DBLogEntry::getUser() const
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

		uid DBLogEntry::getObjectId() const
		{
			return _objectId;
		}

		void DBLogEntry::setObjectId( uid id )
		{
			_objectId = id;
		}

		string DBLogEntry::getStringContent() const
		{
			stringstream s;
			for (Content::const_iterator it = _content.begin(); it != _content.end(); ++it)
			{
				s << *it << " ";
			}
			return s.str();
		}

		DBLogEntry::DBLogEntry()
			: _date(TIME_CURRENT)
		{

		}
	}
}
