
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
#include "11_interfaces/InterfaceTableSync.h"

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

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,SiteTableSync>::FACTORY_KEY("36.01 Site");
	}

	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<SiteTableSync>::TABLE_NAME = "t025_sites";
		template<> const int SQLiteTableSyncTemplate<SiteTableSync>::TABLE_ID = 25;
		template<> const bool SQLiteTableSyncTemplate<SiteTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<SiteTableSync,Site>::load(Site* site, const SQLiteResultSPtr& rows)
		{
		    site->setKey(rows->getLongLong (TABLE_COL_ID));
		    site->setName(rows->getText (SiteTableSync::TABLE_COL_NAME));
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



		template<> void SQLiteDirectTableSyncTemplate<SiteTableSync,Site>::_link(Site* obj, const db::SQLiteResultSPtr& rows, GetSource temporary)
		{
			obj->setInterface(InterfaceTableSync::Get(rows->getLongLong(SiteTableSync::COL_INTERFACE_ID), obj, false, temporary));
		}



		template<> void SQLiteDirectTableSyncTemplate<SiteTableSync,Site>::_unlink(Site* obj)
		{
			obj->setInterface(NULL);
		}


		template<> void SQLiteDirectTableSyncTemplate<SiteTableSync,Site>::save(Site* site)
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
			: db::SQLiteRegistryTableSyncTemplate<SiteTableSync,Site>()
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
	}
}
