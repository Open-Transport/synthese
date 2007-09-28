
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

#include "SiteTableSync.h"

#include "36_places_list/Site.h"

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"

#include "04_time/Date.h"

#include "11_interfaces/Interface.h"

#include <sstream>
#include <boost/tokenizer.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace util;
	using namespace db;
	using namespace time;
	using namespace transportwebsite;

	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<Site>::TABLE_NAME = "t025_sites";
		template<> const int SQLiteTableSyncTemplate<Site>::TABLE_ID = 25;
		template<> const bool SQLiteTableSyncTemplate<Site>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Site>::load(Site* site, const SQLiteResultSPtr& rows)
		{
		    site->setKey(rows->getLongLong (TABLE_COL_ID));
		    site->setName(rows->getText (SiteTableSync::TABLE_COL_NAME));
			shared_ptr<const Interface> interf = Interface::Get(rows->getLongLong(SiteTableSync::COL_INTERFACE_ID));
		    site->setInterface(interf);
		    site->setStartDate(Date::FromSQLDate(rows->getText (SiteTableSync::TABLE_COL_START_DATE)));
		    site->setEndDate(Date::FromSQLDate(rows->getText(SiteTableSync::TABLE_COL_END_DATE)));
		    site->setOnlineBookingAllowed(rows->getBool(SiteTableSync::TABLE_COL_ONLINE_BOOKING));
		    site->setPastSolutionsDisplayed(rows->getBool(SiteTableSync::TABLE_COL_USE_OLD_DATA));
		    site->setMaxTransportConnectionsCount(rows->getInt(SiteTableSync::COL_MAX_CONNECTIONS));
		    site->setUseDateRange(rows->getInt(SiteTableSync::COL_USE_DATES_RANGE));
		    
		    string periodsStr(rows->getText(SiteTableSync::COL_PERIODS));

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

			boost::char_separator<char> sep1 (",");
			boost::char_separator<char> sep2 ("|");
			tokenizer tripletTokens (periodsStr, sep1);
			for (tokenizer::iterator tripletIter = tripletTokens.begin();
				tripletIter != tripletTokens.end (); ++tripletIter)
			{
				tokenizer valueTokens (*tripletIter, sep2);
				tokenizer::iterator valueIter = valueTokens.begin();

				// (beginHour|endHour|Caption)
				Hour beginHour(Hour::FromSQLTime(*valueIter));
				Hour endHour(Hour::FromSQLTime(*(++valueIter)));
				HourPeriod period(*(++valueIter), beginHour, endHour);

				site->addHourPeriod(period);
			}


		}

		template<> void SQLiteTableSyncTemplate<Site>::save(Site* site)
		{
			stringstream query;
			query << " REPLACE INTO " << TABLE_NAME << " VALUES("
				
				<< ")";
			DBModule::GetSQLite()->execUpdate(query.str());
		}
	}

	namespace transportwebsite
	{
		const string SiteTableSync::TABLE_COL_NAME = "name";
		const string SiteTableSync::COL_INTERFACE_ID = "interface_id";
		const string SiteTableSync::TABLE_COL_START_DATE = "start_date";
		const string SiteTableSync::TABLE_COL_END_DATE = "end_date";
		const string SiteTableSync::TABLE_COL_ONLINE_BOOKING = "online_booking";
		const string SiteTableSync::TABLE_COL_USE_OLD_DATA = "use_old_data";
		const string SiteTableSync::COL_MAX_CONNECTIONS = "max_connections";
		const string SiteTableSync::COL_USE_DATES_RANGE("use_dates_range");
		const string SiteTableSync::COL_PERIODS("periods");

		

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
			addTableColumn(COL_USE_DATES_RANGE, "INTEGER", true);
			addTableColumn(COL_PERIODS, "TEXT", true);
		}


		void SiteTableSync::rowsUpdated( SQLite* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows )
		{
		    while (rows->next ())
		    {
			uid id = rows->getLongLong (TABLE_COL_ID);
			if (Site::Contains(id))
				load(Site::GetUpdateable(id).get(), rows);
		    }
		}



		void SiteTableSync::rowsAdded( SQLite* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows, bool isFirstSync)
		{
		    while (rows->next ())
		    {
			uid id = rows->getLongLong (TABLE_COL_ID);
			if (Site::Contains(id))
			{
				shared_ptr<Site> site(Site::GetUpdateable(id));
			    load(site.get(), rows);
			}
			else
			{
			    Site* site(new Site);
			    load(site, rows);
				site->store();
			}
		    }
		}


		void SiteTableSync::rowsRemoved( SQLite* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows )
		{
		    while (rows->next ())
		    {
			uid id = rows->getLongLong (TABLE_COL_ID);
			if (Site::Contains(id))
				Site::Remove(id);
		    }
		}
	}
}
