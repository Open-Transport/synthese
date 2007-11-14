
/** VinciAntivolTableSync class implementation.
	@file VinciAntivolTableSync.cpp

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

#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "VinciAntivol.h"
#include "VinciAntivolTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace vinci;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,VinciAntivolTableSync>::FACTORY_KEY("71.04 Vinci Locks");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciAntivolTableSync,VinciAntivol>::TABLE_NAME = "t043_vinci_locks";
		template<> const int SQLiteTableSyncTemplate<VinciAntivolTableSync,VinciAntivol>::TABLE_ID = 43;
		template<> const bool SQLiteTableSyncTemplate<VinciAntivolTableSync,VinciAntivol>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<VinciAntivolTableSync,VinciAntivol>::load(VinciAntivol* object, const db::SQLiteResultSPtr& rows )
		{
			object->setKey(rows->getLongLong (TABLE_COL_ID));
			object->setMarkedNumber(rows->getText ( VinciAntivolTableSync::COL_MARKED_NUMBER));
		}

		template<> void SQLiteTableSyncTemplate<VinciAntivolTableSync,VinciAntivol>::_link(VinciAntivol* object, const db::SQLiteResultSPtr& rows, GetSource temporary)
		{

		}

		template<> void SQLiteTableSyncTemplate<VinciAntivolTableSync,VinciAntivol>::_unlink(VinciAntivol* object)
		{

		}

		template<> void SQLiteTableSyncTemplate<VinciAntivolTableSync,VinciAntivol>::save(VinciAntivol* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getMarkedNumber())
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace vinci
	{
		const std::string VinciAntivolTableSync::COL_MARKED_NUMBER = "marked_number";

		VinciAntivolTableSync::VinciAntivolTableSync()
			: SQLiteNoSyncTableSyncTemplate<VinciAntivolTableSync,VinciAntivol>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_MARKED_NUMBER, "TEXT", true);
		}



		vector<shared_ptr<VinciAntivol> > VinciAntivolTableSync::search(const std::string markedNumber, int first /*= 0*/, int number /*= 0*/ )
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				<< COL_MARKED_NUMBER << "=" << Conversion::ToSQLiteString(markedNumber)
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<VinciAntivol> > objects;
				while (rows->next ())
				{
					shared_ptr<VinciAntivol> object(new VinciAntivol());
					load(object.get (), rows);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
