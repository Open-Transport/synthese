
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

using namespace boost;

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
	}

	namespace routeplanner
	{
		const std::string SiteTableSync::TABLE_COL_NAME = "name";
		const std::string SiteTableSync::COL_INTERFACE_ID = "interface_id";
		const std::string SiteTableSync::TABLE_COL_START_DATE = "start_date";
		const std::string SiteTableSync::TABLE_COL_END_DATE = "end_date";
		const std::string SiteTableSync::TABLE_COL_ONLINE_BOOKING = "online_booking";
		const std::string SiteTableSync::TABLE_COL_USE_OLD_DATA = "use_old_data";
		

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
		}


		void SiteTableSync::rowsUpdated( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}


		void SiteTableSync::rowsAdded( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				shared_ptr<Site> site;
				try
				{
					site.reset(new Site(Conversion::ToLongLong(rows.getColumn(i,TABLE_COL_ID))));
					site->setName(rows.getColumn(i,TABLE_COL_NAME));
					shared_ptr<const Interface> interf = InterfaceModule::getInterfaces().get(Conversion::ToLongLong(rows.getColumn(i, COL_INTERFACE_ID)));
					site->setInterface(interf.get());
					site->setStartDate(Date::FromSQLDate(rows.getColumn(i, TABLE_COL_START_DATE)));
					site->setEndDate(Date::FromSQLDate(rows.getColumn(i, TABLE_COL_END_DATE)));
					site->setOnlineBookingAllowed(Conversion::ToBool(rows.getColumn(i, TABLE_COL_ONLINE_BOOKING)));
					site->setPastSolutionsDisplayed(Conversion::ToBool(rows.getColumn(i, TABLE_COL_USE_OLD_DATA)));
				}
				catch (Interface::RegistryKeyException e)
				{
					Log::GetInstance().warn("Corrupted data on "+ TABLE_NAME +" table : Interface not found", e);
					continue;
				}

				RoutePlannerModule::getSites().add(site);
			}
		}


		void SiteTableSync::rowsRemoved( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}
	}
}



