
/** FareTableSync class implementation.
	@file FareTableSync.cpp

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
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "Fare.h"
#include "FareTableSync.h"
#include "15_env/EnvModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Fare>::TABLE_NAME = "t008_fares";
		template<> const int SQLiteTableSyncTemplate<Fare>::TABLE_ID = 8;
		template<> const bool SQLiteTableSyncTemplate<Fare>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Fare>::load(Fare* fare, const db::SQLiteResult& rows, int rowIndex/*=0*/ )
		{
			fare->setKey(Conversion::ToLongLong(rows.getColumn(rowIndex, TABLE_COL_ID)));
			fare->setName (rows.getColumn (rowIndex, FareTableSync::COL_NAME));
			fare->setType ((synthese::env::Fare::FareType) Conversion::ToInt (rows.getColumn (rowIndex, FareTableSync::COL_FARETYPE)));
		}

		template<> void SQLiteTableSyncTemplate<Fare>::save(Fare* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE_NAME << " SET "
					/// @todo fill fields [,]FIELD=VALUE
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId());
                query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					/// @todo fill other fields separated by ,
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		const std::string FareTableSync::COL_NAME ("name");
		const std::string FareTableSync::COL_FARETYPE ("fare_type");

		FareTableSync::FareTableSync()
			: SQLiteTableSyncTemplate<Fare>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_NAME, "TEXT", true);
			addTableColumn (COL_FARETYPE, "INTEGER", true);
		}

		void FareTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				shared_ptr<Fare> object(new Fare());
				load(object.get(), rows, i);
				EnvModule::getFares().add(object);
			}
		}

		void FareTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getFares().contains(id))
				{
					shared_ptr<Fare> object = EnvModule::getFares().getUpdateable(id);
					load(object.get(), rows, i);
				}
			}
		}

		void FareTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getFares().contains(id))
				{
					EnvModule::getFares().remove(id);
				}
			}
		}

		std::vector<shared_ptr<Fare> > FareTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<Fare> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<Fare> object(new Fare());
					load(object.get(), result, i);
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
