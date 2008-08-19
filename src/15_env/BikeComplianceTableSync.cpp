
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

#include "Conversion.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "BikeCompliance.h"
#include "BikeComplianceTableSync.h"

using namespace std;
using boost::logic::tribool;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,BikeComplianceTableSync>::FACTORY_KEY("15.10.03 Bike compliances");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<BikeComplianceTableSync>::TABLE_NAME = "t020_bike_compliances";
		template<> const int SQLiteTableSyncTemplate<BikeComplianceTableSync>::TABLE_ID = 20;
		template<> const bool SQLiteTableSyncTemplate<BikeComplianceTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<BikeComplianceTableSync,BikeCompliance>::load(BikeCompliance* cmp, const db::SQLiteResultSPtr& rows)
		{
			// Columns reading
			tribool status = true;
			int statusInt (rows->getInt (BikeComplianceTableSync::COL_STATUS));
			if (statusInt < 0)
			{
				status = boost::logic::indeterminate;
			}
			else if (statusInt == 0)
			{
				status = false;
			}

			int capacity (rows->getInt (BikeComplianceTableSync::COL_CAPACITY));
			
			// Properties
			cmp->setKey(rows->getLongLong (TABLE_COL_ID));
			cmp->setCompliant (status);
			cmp->setCapacity (capacity);
		}

		template<> void SQLiteDirectTableSyncTemplate<BikeComplianceTableSync,BikeCompliance>::_link(BikeCompliance* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{

		}

		template<> void SQLiteDirectTableSyncTemplate<BikeComplianceTableSync,BikeCompliance>::_unlink(BikeCompliance* obj)
		{

		}

		template<> void SQLiteDirectTableSyncTemplate<BikeComplianceTableSync,BikeCompliance>::save(BikeCompliance* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
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
			: SQLiteRegistryTableSyncTemplate<BikeComplianceTableSync,BikeCompliance>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_STATUS, "INTEGER");
			addTableColumn (COL_CAPACITY, "INTEGER");
		}

		std::vector<shared_ptr<BikeCompliance> > BikeComplianceTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			SQLite* sqlite = DBModule::GetSQLite();
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
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<BikeCompliance> > objects;
				while (rows->next ())
				{
					shared_ptr<BikeCompliance> object(new BikeCompliance());
					load(object.get(), rows);
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

