
/** SiteTableSync class implementation.
	@file SiteTableSync.cpp

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

#include "33_route_planner/SiteTableSync.h"
#include "33_route_planner/Site.h"
#include "33_route_planner/RoutePlannerModule.h"

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"

#include "04_time/Date.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/InterfaceModule.h"

#include <sstream>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace util;
	using namespace db;
	using namespace time;
	using namespace routeplanner;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Site>::TABLE_NAME = "t025_sites";
		template<> const int SQLiteTableSyncTemplate<Site>::TABLE_ID = 25;
		template<> const bool SQLiteTableSyncTemplate<Site>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Site>::load(Site* site, const SQLiteResult& rows, int i)
		{
			site->setKey(Conversion::ToLongLong(rows.getColumn(i,TABLE_COL_ID)));
			site->setName(rows.getColumn(i, SiteTableSync::TABLE_COL_NAME));
			shared_ptr<const Interface> interf = InterfaceModule::getInterfaces().get(Conversion::ToLongLong(rows.getColumn(i, SiteTableSync::COL_INTERFACE_ID)));
			site->setInterface(interf);
			site->setStartDate(Date::FromSQLDate(rows.getColumn(i, SiteTableSync::TABLE_COL_START_DATE)));
			site->setEndDate(Date::FromSQLDate(rows.getColumn(i, SiteTableSync::TABLE_COL_END_DATE)));
			site->setOnlineBookingAllowed(Conversion::ToBool(rows.getColumn(i, SiteTableSync::TABLE_COL_ONLINE_BOOKING)));
			site->setPastSolutionsDisplayed(Conversion::ToBool(rows.getColumn(i, SiteTableSync::TABLE_COL_USE_OLD_DATA)));
			site->setMaxTransportConnectionsCount(Conversion::ToInt(rows.getColumn(i, SiteTableSync::COL_MAX_CONNECTIONS)));
		}

		template<> void SQLiteTableSyncTemplate<Site>::save(Site* site)
		{
			stringstream query;
			query << " REPLACE INTO " << TABLE_NAME << " VALUES("
				
				<< ")";
			DBModule::GetSQLite()->execUpdate(query.str());
		}
	}

	namespace routeplanner
	{
		const std::string SiteTableSync::TABLE_COL_NAME = "name";
		const std::string SiteTableSync::COL_INTERFACE_ID = "interface_id";
		const std::string SiteTableSync::TABLE_COL_START_DATE = "start_date";
		const std::string SiteTableSync::TABLE_COL_END_DATE = "end_date";
		const std::string SiteTableSync::TABLE_COL_ONLINE_BOOKING = "online_booking";
		const std::string SiteTableSync::TABLE_COL_USE_OLD_DATA = "use_old_data";
		const std::string SiteTableSync::COL_MAX_CONNECTIONS = "max_connections";
		

		SiteTableSync::SiteTableSync()
			: db::SQLiteTableSyncTemplate<Site> ( true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(COL_INTERFACE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_START_DATE, "DATE", true);
			addTableColumn(TABLE_COL_END_DATE, "DATE", true);
			addTableColumn(TABLE_COL_ONLINE_BOOKING, "INTEGER", true);
			addTableColumn(TABLE_COL_USE_OLD_DATA, "INTEGER", true);
			addTableColumn(COL_MAX_CONNECTIONS, "INTEGER", true);
		}


		void SiteTableSync::rowsUpdated( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (RoutePlannerModule::getSites().contains(id))
					load(RoutePlannerModule::getSites().getUpdateable(id).get(), rows, i);
			}
		}


		void SiteTableSync::rowsAdded( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				shared_ptr<Site> site;
				if (RoutePlannerModule::getSites().contains(id))
				{
					site = RoutePlannerModule::getSites().getUpdateable(id);
					load(site.get(), rows, i);
				}
				else
				{
					site.reset(new Site);
					load(site.get(), rows, i);
					RoutePlannerModule::getSites().add(site);
				}
			}
		}


		void SiteTableSync::rowsRemoved( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (RoutePlannerModule::getSites().contains(id))
					RoutePlannerModule::getSites().remove(id);
			}
		}
	}
}
