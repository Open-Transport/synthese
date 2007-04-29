
/** VinciSiteTableSync class implementation.
	@file VinciSiteTableSync.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#include "02_db/DBModule.h"

#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace vinci;
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciSite>::TABLE_NAME = "t034_vinci_site";
		template<> const int SQLiteTableSyncTemplate<VinciSite>::TABLE_ID = 34;
		template<> const bool SQLiteTableSyncTemplate<VinciSite>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<VinciSite>::load(VinciSite* vs, const SQLiteResult& rows, int rowId)
		{
			vs->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			vs->setName(rows.getColumn(rowId, VinciSiteTableSync::COL_NAME));
			vs->setAddress(rows.getColumn(rowId, VinciSiteTableSync::COL_ADDRESS));
			vs->setPhone(rows.getColumn(rowId, VinciSiteTableSync::COL_PHONE));
		}

		template<> void SQLiteTableSyncTemplate<VinciSite>::save(VinciSite* vs)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (vs->getKey() == 0)
				vs->setKey(getId(0,0)); /// @todo Handle grid id

			// INSERT
			query << "REPLACE INTO " << TABLE_NAME << " VALUES("
				<< vs->getKey()
				<< "," << Conversion::ToSQLiteString(vs->getName())
				<< "," << Conversion::ToSQLiteString(vs->getAddress())
				<< "," << Conversion::ToSQLiteString(vs->getPhone())
				<< ")";
			
			sqlite->execUpdate(query.str());
		}
	}

	namespace vinci
	{
		const std::string VinciSiteTableSync::COL_NAME = "name";
		const std::string VinciSiteTableSync::COL_ADDRESS = "address";
		const std::string VinciSiteTableSync::COL_PHONE = "phone";

		VinciSiteTableSync::VinciSiteTableSync()
			: SQLiteTableSyncTemplate<VinciSite>(true, true, TRIGGERS_ENABLED_CLAUSE, true)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_NAME, "TEXT", true);
			addTableColumn(COL_ADDRESS, "TEXT", true);
			addTableColumn(COL_PHONE, "TEXT", true);

			addTableIndex(COL_NAME);
		}

		void VinciSiteTableSync::rowsAdded( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{

		}

		void VinciSiteTableSync::rowsUpdated( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void VinciSiteTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		std::vector<boost::shared_ptr<VinciSite> > VinciSiteTableSync::search( int first /*= 0*/, int number /*= 0 */, bool orderByName/*=true */, bool raisingOrder/*=true */ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;

			query
				<< "SELECT *"
				<< " FROM "
				<< TABLE_NAME;
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");

			SQLiteResult result = sqlite->execQuery(query.str());
			vector<shared_ptr<VinciSite> > sites;

			for (int i=0; i<result.getNbRows(); ++i)
			{
				shared_ptr<VinciSite> site(new VinciSite);
				load(site.get(), result, i);
				sites.push_back(site);
			}
			return sites;
		}
	}
}
