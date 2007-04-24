
/** DBLog class implementation.
	@file DBLog.cpp

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

#include "13_dblog/DBLog.h"
#include "13_dblog/DBLogEntryTableSync.h"

using namespace boost;

namespace synthese
{
	using namespace util;

	namespace dblog
	{
		DBLog::DBLog( const std::string& name )
			: Factorable(), _name(name)
		{

		}

		const std::string& DBLog::getName() const
		{
			return _name;
		}

		void DBLog::_addEntry(
			DBLogEntry::Level level
			, const DBLogEntry::Content& content
			, shared_ptr<const security::User> user /*= NULL*/ 
			, uid objectId
		){
			shared_ptr<DBLogEntry> e(new DBLogEntry);
			e->setLevel(level);
			e->setUser(user);
			e->setLogKey(getFactoryKey());
			e->setContent(content);
			e->setObjectId(objectId);
			DBLogEntryTableSync::save(e.get());
		}

		DBLog::ColumnsVector DBLog::parse( const DBLogEntry::Content& cols ) const
		{
			return (ColumnsVector) cols;
		}

		std::string DBLog::getObjectName( uid id ) const
		{
			return Conversion::ToString(id);
		}
	}
}
