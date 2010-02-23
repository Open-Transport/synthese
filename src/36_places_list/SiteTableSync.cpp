
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
#include "ReplaceQuery.h"
#include "SQLiteResult.h"
#include "Interface.h"
#include "InterfaceTableSync.h"

#include <sstream>
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace util;
	using namespace db;
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
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<SiteTableSync>::TABLE(
				"t025_sites"
		);
		
		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<SiteTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(SiteTableSync::TABLE_COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(SiteTableSync::COL_INTERFACE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(SiteTableSync::TABLE_COL_START_DATE, SQL_DATE),
			SQLiteTableSync::Field(SiteTableSync::TABLE_COL_END_DATE, SQL_DATE),
			SQLiteTableSync::Field(SiteTableSync::TABLE_COL_ONLINE_BOOKING, SQL_INTEGER),
			SQLiteTableSync::Field(SiteTableSync::TABLE_COL_USE_OLD_DATA, SQL_INTEGER),
			SQLiteTableSync::Field(SiteTableSync::COL_MAX_CONNECTIONS, SQL_INTEGER),
			SQLiteTableSync::Field(SiteTableSync::COL_USE_DATES_RANGE, SQL_INTEGER),
			SQLiteTableSync::Field(SiteTableSync::COL_PERIODS, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<SiteTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<SiteTableSync,Site>::Load(
			Site* site,
			const SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    site->setName(rows->getText (SiteTableSync::TABLE_COL_NAME));
		    site->setStartDate(rows->getDate(SiteTableSync::TABLE_COL_START_DATE));
		    site->setEndDate(rows->getDate(SiteTableSync::TABLE_COL_END_DATE));
		    site->setOnlineBookingAllowed(rows->getBool(SiteTableSync::TABLE_COL_ONLINE_BOOKING));
		    site->setPastSolutionsDisplayed(rows->getBool(SiteTableSync::TABLE_COL_USE_OLD_DATA));
		    site->setMaxTransportConnectionsCount(rows->getInt(SiteTableSync::COL_MAX_CONNECTIONS));
		    site->setUseDateRange(days(rows->getInt(SiteTableSync::COL_USE_DATES_RANGE)));
		    
		    string periodsStr(rows->getText(SiteTableSync::COL_PERIODS));

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

			boost::char_separator<char> sep1 (",");
			boost::char_separator<char> sep2 ("|");
			tokenizer tripletTokens (periodsStr, sep1);
			site->clearHourPeriods();
			for (tokenizer::iterator tripletIter = tripletTokens.begin();
				tripletIter != tripletTokens.end (); ++tripletIter)
			{
				tokenizer valueTokens (*tripletIter, sep2);
				tokenizer::iterator valueIter = valueTokens.begin();

				// (beginHour|endHour|Caption)
				time_duration beginHour(duration_from_string(*valueIter));
				time_duration endHour(duration_from_string(*(++valueIter)));
				HourPeriod period(*(++valueIter), beginHour, endHour);

				site->addHourPeriod(period);
			}

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				uid id(rows->getLongLong(SiteTableSync::COL_INTERFACE_ID));
				if (id > 0)
				{
					try
					{
						site->setInterface(InterfaceTableSync::Get(id, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<Interface>& e)
					{
						Log::GetInstance().warn(
							"Data corrupted in "+ TABLE.NAME + " on site " + lexical_cast<string>(site->getKey()) +" : interface " +
							lexical_cast<string>(id) + " not found"
						);
					}
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<SiteTableSync,Site>::Unlink(
			Site* obj
		){
			obj->setInterface(NULL);
		}


		template<> void SQLiteDirectTableSyncTemplate<SiteTableSync,Site>::Save(
			Site* site,
			optional<SQLiteTransaction&> transaction
		){
			// Preparation
			stringstream periodstr;
			const Site::Periods& periods(site->getPeriods());
			for(Site::Periods::const_iterator it(periods.begin()); it != periods.end(); ++it)
			{
				if (it != periods.begin())
					periodstr << ",";
				periodstr << to_simple_string(it->getBeginHour())
					<< "|" << to_simple_string(it->getEndHour())
					<< "|" << it->getCaption();
			}

			// Query
			ReplaceQuery<SiteTableSync> query(*site);
			query.addField(site->getName());
			query.addField(site->getInterface() ? site->getInterface()->getKey() : RegistryKeyType(0));
			query.addField(site->getStartDate());
			query.addField(site->getEndDate());
			query.addField(site->getOnlineBookingAllowed());
			query.addField(site->getPastSolutionsDisplayed());
			query.addField(site->getMaxTransportConnectionsCount());
			query.addField(static_cast<int>(site->getUseDatesRange().days()));
			query.addField(periodstr.str());
			query.execute(transaction);
		}
	}

	namespace transportwebsite
	{
		SiteTableSync::SearchResult SiteTableSync::Search(
			Env& env,
			std::string name
			, int first /*= 0*/,
			boost::optional<std::size_t> number
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
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
