
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
#include "Site.h"
#include "Conversion.h"
#include "SQLiteResult.h"
#include "Date.h"
#include "Interface.h"
#include "InterfaceTableSync.h"

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
	}

	namespace db
	{
		template<> const SQLiteTableFormat SQLiteTableSyncTemplate<SiteTableSync>::TABLE(
			SiteTableSync::CreateFormat(
				"t025_sites",
				SQLiteTableFormat::CreateFields(
				SQLiteTableFormat::Field(SiteTableSync::TABLE_COL_NAME, TEXT),
				SQLiteTableFormat::Field(SiteTableSync::COL_INTERFACE_ID, INTEGER),
				SQLiteTableFormat::Field(SiteTableSync::TABLE_COL_START_DATE, DATE),
				SQLiteTableFormat::Field(SiteTableSync::TABLE_COL_END_DATE, DATE),
				SQLiteTableFormat::Field(SiteTableSync::TABLE_COL_ONLINE_BOOKING, INTEGER),
				SQLiteTableFormat::Field(SiteTableSync::TABLE_COL_USE_OLD_DATA, INTEGER),
				SQLiteTableFormat::Field(SiteTableSync::COL_MAX_CONNECTIONS, INTEGER),
				SQLiteTableFormat::Field(SiteTableSync::COL_USE_DATES_RANGE, INTEGER),
				SQLiteTableFormat::Field(SiteTableSync::COL_PERIODS, TEXT),
				SQLiteTableFormat::Field()
			), SQLiteTableFormat::Indexes()
		)	);


		template<> void SQLiteDirectTableSyncTemplate<SiteTableSync,Site>::Load(
			Site* site,
			const SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
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

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				uid id(rows->getLongLong(SiteTableSync::COL_INTERFACE_ID));
				if (id != UNKNOWN_VALUE)
					site->setInterface(InterfaceTableSync::Get(id, env, linkLevel).get());
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<SiteTableSync,Site>::Unlink(
			Site* obj,
			Env* env
		){
			obj->setInterface(NULL);
		}


		template<> void SQLiteDirectTableSyncTemplate<SiteTableSync,Site>::Save(Site* site)
		{
			stringstream query;
			query << " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< site->getKey()
				<< "," << Conversion::ToSQLiteString(site->getName())
				<< "," << (site->getInterface() ? site->getInterface()->getKey() : static_cast<uid>(UNKNOWN_VALUE))
				<< "," << site->getStartDate().toSQLString()
				<< "," << site->getEndDate().toSQLString()
				<< "," << Conversion::ToString(site->getOnlineBookingAllowed())
				<< "," << Conversion::ToString(site->getPastSolutionsDisplayed())
				<< "," << site->getMaxTransportConnectionsCount()
				<< "," << site->getUseDatesRange();
			
			const Site::Periods& periods(site->getPeriods());
			query << ",'";
			for(Site::Periods::const_iterator it(periods.begin()); it != periods.end(); ++it)
			{
				if (it != periods.begin())
					query << ",";
				query << it->getBeginHour().toSQLString(false)
					<< "|" << it->getEndHour().toSQLString(false)
					<< "|" << Conversion::ToSQLiteString(it->getCaption(), false);
			}
			query << "'"
				<< ")";
			DBModule::GetSQLite()->execUpdate(query.str());
		}
	}

	namespace transportwebsite
	{
		SiteTableSync::SiteTableSync()
			: db::SQLiteRegistryTableSyncTemplate<SiteTableSync,Site>()
		{
		}


		void SiteTableSync::Search(
			Env& env,
			std::string name
			, int first /*= 0*/, int number /*= 0*/ 
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (!name.empty())
			 	query << " AND " << TABLE_COL_NAME << " LIKE '" << Conversion::ToSQLiteString(name, false) << "'";
				;
			if (orderByName)
				query << " ORDER BY " << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
