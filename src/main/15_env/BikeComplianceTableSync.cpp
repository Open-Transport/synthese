
/** BikeComplianceTableSync class implementation.
	@file BikeComplianceTableSync.cpp

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

#include "15_env/BikeCompliance.h"
#include "15_env/BikeComplianceTableSync.h"
#include "15_env/EnvModule.h"

using namespace std;
using boost::logic::tribool;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<BikeCompliance>::TABLE_NAME = "t020_bike_compliances";
		template<> const int SQLiteTableSyncTemplate<BikeCompliance>::TABLE_ID = 20;
		template<> const bool SQLiteTableSyncTemplate<BikeCompliance>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<BikeCompliance>::load(BikeCompliance* cmp, const db::SQLiteResult& rows, int rowIndex/*=0*/ )
		{
			cmp->setKey(Conversion::ToLongLong(rows.getColumn(rowIndex, TABLE_COL_ID)));

			tribool status = true;
			int statusInt (
				Conversion::ToInt (rows.getColumn (rowIndex, BikeComplianceTableSync::COL_STATUS)));
			if (statusInt < 0)
			{
				status = boost::logic::indeterminate;
			}
			else if (statusInt == 0)
			{
				status = false;
			}

			int capacity (
				Conversion::ToInt (rows.getColumn (rowIndex, BikeComplianceTableSync::COL_CAPACITY)));

			cmp->setCompliant (status);
			cmp->setCapacity (capacity);
		}

		template<> void SQLiteTableSyncTemplate<BikeCompliance>::save(BikeCompliance* object)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
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
		const std::string BikeComplianceTableSync::COL_STATUS ("status");
		const std::string BikeComplianceTableSync::COL_CAPACITY ("capacity");

		BikeComplianceTableSync::BikeComplianceTableSync()
			: SQLiteTableSyncTemplate<BikeCompliance>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_STATUS, "INTEGER");
			addTableColumn (COL_CAPACITY, "INTEGER");
		}

		void BikeComplianceTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				shared_ptr<BikeCompliance> object(new BikeCompliance());
				load(object.get(), rows, i);
				EnvModule::getBikeCompliances().add(object);
			}
		}

		void BikeComplianceTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getBikeCompliances().contains(id))
				{
					shared_ptr<BikeCompliance> object = EnvModule::getBikeCompliances().getUpdateable(id);
					load(object.get(), rows, i);
				}
			}
		}

		void BikeComplianceTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getBikeCompliances().contains(id))
				{
					EnvModule::getBikeCompliances().remove(id);
				}
			}
		}

		std::vector<shared_ptr<BikeCompliance> > BikeComplianceTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
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
				vector<shared_ptr<BikeCompliance> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<BikeCompliance> object(new BikeCompliance());
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

