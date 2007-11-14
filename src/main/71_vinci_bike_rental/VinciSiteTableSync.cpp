
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
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,VinciSiteTableSync>::FACTORY_KEY("71.03 Vinci Site");
	}
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciSiteTableSync,VinciSite>::TABLE_NAME = "t034_vinci_site";
		template<> const int SQLiteTableSyncTemplate<VinciSiteTableSync,VinciSite>::TABLE_ID = 34;
		template<> const bool SQLiteTableSyncTemplate<VinciSiteTableSync,VinciSite>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<VinciSiteTableSync,VinciSite>::load(VinciSite* vs, const SQLiteResultSPtr& rows)
		{
			vs->setKey(rows->getLongLong (TABLE_COL_ID));
			vs->setName(rows->getText ( VinciSiteTableSync::COL_NAME));
			vs->setAddress(rows->getText ( VinciSiteTableSync::COL_ADDRESS));
			vs->setPhone(rows->getText ( VinciSiteTableSync::COL_PHONE));
		}

		template<> void SQLiteTableSyncTemplate<VinciSiteTableSync,VinciSite>::_link(VinciSite* vs, const SQLiteResultSPtr& rows, GetSource temporary)
		{

		}

		template<> void SQLiteTableSyncTemplate<VinciSiteTableSync,VinciSite>::_unlink(VinciSite* vs)
		{

		}

		template<> void SQLiteTableSyncTemplate<VinciSiteTableSync,VinciSite>::save(VinciSite* vs)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (vs->getKey() <= 0)
				vs->setKey(getId());

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
			: SQLiteNoSyncTableSyncTemplate<VinciSiteTableSync,VinciSite>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_NAME, "TEXT", true);
			addTableColumn(COL_ADDRESS, "TEXT", true);
			addTableColumn(COL_PHONE, "TEXT", true);

			addTableIndex(COL_NAME);
		}



		std::vector<boost::shared_ptr<VinciSite> > VinciSiteTableSync::search( int first /*= 0*/, int number /*= 0 */, bool orderByName/*=true */, bool raisingOrder/*=true */ )
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;

			query
				<< "SELECT *"
				<< " FROM "
				<< TABLE_NAME;
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");

			SQLiteResultSPtr rows = sqlite->execQuery(query.str());
			vector<shared_ptr<VinciSite> > sites;

			while(rows->next())
			{
				shared_ptr<VinciSite> site(new VinciSite);
				load(site.get(), rows);
				sites.push_back(site);
			}
			return sites;
		}
	}
}
