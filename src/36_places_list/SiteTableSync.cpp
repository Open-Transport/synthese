
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
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "WebPageTableSync.h"

#include <sstream>
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
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
		const string SiteTableSync::TABLE_COL_START_DATE = "start_date";
		const string SiteTableSync::TABLE_COL_END_DATE = "end_date";
		const string SiteTableSync::TABLE_COL_ONLINE_BOOKING = "online_booking";
		const string SiteTableSync::TABLE_COL_USE_OLD_DATA = "use_old_data";
		const string SiteTableSync::COL_MAX_CONNECTIONS = "max_connections";
		const string SiteTableSync::COL_USE_DATES_RANGE("use_dates_range");
		const string SiteTableSync::COL_PERIODS("periods");
		const string SiteTableSync::COL_DISPLAY_ROAD_APPROACH_DETAILS("display_road_approach_detail");
		const string SiteTableSync::COL_CLIENT_URL("cient_url");
		const string SiteTableSync::COL_DEFAULT_PAGE_TEMPLATE_ID("default_page_template_id");
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
			SQLiteTableSync::Field(SiteTableSync::TABLE_COL_START_DATE, SQL_DATE),
			SQLiteTableSync::Field(SiteTableSync::TABLE_COL_END_DATE, SQL_DATE),
			SQLiteTableSync::Field(SiteTableSync::TABLE_COL_ONLINE_BOOKING, SQL_INTEGER),
			SQLiteTableSync::Field(SiteTableSync::TABLE_COL_USE_OLD_DATA, SQL_INTEGER),
			SQLiteTableSync::Field(SiteTableSync::COL_MAX_CONNECTIONS, SQL_INTEGER),
			SQLiteTableSync::Field(SiteTableSync::COL_USE_DATES_RANGE, SQL_INTEGER),
			SQLiteTableSync::Field(SiteTableSync::COL_PERIODS, SQL_TEXT),
			SQLiteTableSync::Field(SiteTableSync::COL_DISPLAY_ROAD_APPROACH_DETAILS, SQL_INTEGER),
			SQLiteTableSync::Field(SiteTableSync::COL_CLIENT_URL, SQL_TEXT),
			SQLiteTableSync::Field(SiteTableSync::COL_DEFAULT_PAGE_TEMPLATE_ID, SQL_TEXT),
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
			site->setDisplayRoadApproachDetail(rows->getBool(SiteTableSync::COL_DISPLAY_ROAD_APPROACH_DETAILS));
			site->setClientURL(rows->getText(SiteTableSync::COL_CLIENT_URL));
		    
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

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType templateId(rows->getLongLong(SiteTableSync::COL_DEFAULT_PAGE_TEMPLATE_ID));
				try
				{
					site->setDefaultTemplate(
						templateId == 0 ? NULL :
						WebPageTableSync::GetEditable(templateId, env, linkLevel).get()
					);
				}
				catch(ObjectNotFoundException<WebPage>& e)
				{
					Log::GetInstance().warn("No such webpage in site", e);
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<SiteTableSync,Site>::Unlink(
			Site* obj
		){
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
			query.addField(site->getStartDate());
			query.addField(site->getEndDate());
			query.addField(site->getOnlineBookingAllowed());
			query.addField(site->getPastSolutionsDisplayed());
			query.addField(site->getMaxTransportConnectionsCount());
			query.addField(static_cast<int>(site->getUseDatesRange().days()));
			query.addField(periodstr.str());
			query.addField(site->getDisplayRoadApproachDetail());
			query.addField(site->getClientURL());
			query.addField(site->getDefaultTemplate() ? site->getDefaultTemplate()->getKey() : RegistryKeyType(0));
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
			SelectQuery<SiteTableSync> query;
			if (!name.empty())
			{
				query.addWhereField(TABLE_COL_NAME, name, ComposedExpression::OP_LIKE);
			}
			if (orderByName)
			{
				query.addOrderField(TABLE_COL_NAME, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
