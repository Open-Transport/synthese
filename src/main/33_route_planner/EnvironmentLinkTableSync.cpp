
/** EnvironmentLinkTableSync class implementation.
	@file EnvironmentLinkTableSync.cpp

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

#include "33_route_planner/EnvironmentLinkTableSync.h"
#include "33_route_planner/SiteCommercialLineLink.h"
#include "33_route_planner/RoutePlannerModule.h"

#include "02_db/SQLiteException.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "01_util/Conversion.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace routeplanner;

	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<SiteCommercialLineLink>::TABLE_NAME ("t001_environment_links");
		template<> const int SQLiteTableSyncTemplate<SiteCommercialLineLink>::TABLE_ID (1);
		template<> const bool SQLiteTableSyncTemplate<SiteCommercialLineLink>::HAS_AUTO_INCREMENT (true);

		template<> void SQLiteTableSyncTemplate<SiteCommercialLineLink>::load(SiteCommercialLineLink* obj, const db::SQLiteResult& rows, int rowId/* =0 */)
		{
			obj->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			obj->setSiteId(Conversion::ToLongLong(rows.getColumn(rowId, EnvironmentLinkTableSync::COL_SITE_ID)));
			obj->setCommercialLineId(Conversion::ToLongLong(rows.getColumn(rowId, EnvironmentLinkTableSync::COL_COMMERCIAL_LINE_ID)));
		}
		
		template<> void SQLiteTableSyncTemplate<SiteCommercialLineLink>::save(SiteCommercialLineLink* obj)
		{
			if (!obj->getKey())
				obj->setKey(getId());
			stringstream query;
			query << " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(obj->getKey())
				<< "," << Conversion::ToString(obj->getSiteId())
				<< "," << Conversion::ToString(obj->getCommercialLineId())
				<< ")";
			DBModule::GetSQLite()->execUpdate(query.str());
		}
	}

	namespace routeplanner
	{
		const std::string EnvironmentLinkTableSync::COL_SITE_ID ("site_id");
		const std::string EnvironmentLinkTableSync::COL_COMMERCIAL_LINE_ID ("commercial_line_id");


		EnvironmentLinkTableSync::EnvironmentLinkTableSync ()
		: SQLiteTableSyncTemplate<SiteCommercialLineLink> (true, true, db::TRIGGERS_ENABLED_CLAUSE)
		{	
			{
				addTableColumn (TABLE_COL_ID, "INTEGER", false);
				addTableColumn (COL_SITE_ID, "INTEGER", false);
				addTableColumn (COL_COMMERCIAL_LINE_ID, "INTEGER", false);
			}
			// The preceding block is used by doxygen, please do not remove
		}

				    
		void 
		EnvironmentLinkTableSync::rowsAdded (const synthese::db::SQLiteQueueThreadExec* sqlite, 
							 synthese::db::SQLiteSync* sync,
							 const synthese::db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows (); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (RoutePlannerModule::getSiteLineLinks().contains(id))
				{
					load(RoutePlannerModule::getSiteLineLinks().getUpdateable(id).get(), rows, i);
				}
				else
				{
					shared_ptr<SiteCommercialLineLink> scl(new SiteCommercialLineLink);
					load(scl.get(), rows, i);
					RoutePlannerModule::getSiteLineLinks().add(scl);
				}
			}
		}



		void 
		EnvironmentLinkTableSync::rowsUpdated (const synthese::db::SQLiteQueueThreadExec* sqlite, 
						 synthese::db::SQLiteSync* sync,
						 const synthese::db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows (); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (RoutePlannerModule::getSiteLineLinks().contains(id))
				{
					load(RoutePlannerModule::getSiteLineLinks().getUpdateable(id).get(), rows, i);
				}
			}
		}



		void 
		EnvironmentLinkTableSync::rowsRemoved (const synthese::db::SQLiteQueueThreadExec* sqlite, 
							   synthese::db::SQLiteSync* sync,
							   const synthese::db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows (); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (RoutePlannerModule::getSiteLineLinks().contains(id))
				{
					RoutePlannerModule::getSiteLineLinks().remove(id);
				}
			}
		}
			
	}
}
