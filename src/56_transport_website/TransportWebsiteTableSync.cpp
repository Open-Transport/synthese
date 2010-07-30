
/** TransportWebsiteTableSync class implementation.
	@file TransportWebsiteTableSync.cpp

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

#include "TransportWebsiteTableSync.h"
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
	using namespace cms;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,TransportWebsiteTableSync>::FACTORY_KEY("56.01 TransportWebsite");
	}

	namespace transportwebsite
	{
		const string TransportWebsiteTableSync::TABLE_COL_NAME = "name";
		const string TransportWebsiteTableSync::TABLE_COL_START_DATE = "start_date";
		const string TransportWebsiteTableSync::TABLE_COL_END_DATE = "end_date";
		const string TransportWebsiteTableSync::TABLE_COL_ONLINE_BOOKING = "online_booking";
		const string TransportWebsiteTableSync::TABLE_COL_USE_OLD_DATA = "use_old_data";
		const string TransportWebsiteTableSync::COL_MAX_CONNECTIONS = "max_connections";
		const string TransportWebsiteTableSync::COL_USE_DATES_RANGE("use_dates_range");
		const string TransportWebsiteTableSync::COL_PERIODS("periods");
		const string TransportWebsiteTableSync::COL_DISPLAY_ROAD_APPROACH_DETAILS("display_road_approach_detail");
		const string TransportWebsiteTableSync::COL_CLIENT_URL("cient_url");
		const string TransportWebsiteTableSync::COL_DEFAULT_PAGE_TEMPLATE_ID("default_page_template_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<TransportWebsiteTableSync>::TABLE(
			"t025_sites"
		);
		
		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<TransportWebsiteTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(TransportWebsiteTableSync::TABLE_COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(TransportWebsiteTableSync::TABLE_COL_START_DATE, SQL_DATE),
			SQLiteTableSync::Field(TransportWebsiteTableSync::TABLE_COL_END_DATE, SQL_DATE),
			SQLiteTableSync::Field(TransportWebsiteTableSync::TABLE_COL_ONLINE_BOOKING, SQL_INTEGER),
			SQLiteTableSync::Field(TransportWebsiteTableSync::TABLE_COL_USE_OLD_DATA, SQL_INTEGER),
			SQLiteTableSync::Field(TransportWebsiteTableSync::COL_MAX_CONNECTIONS, SQL_INTEGER),
			SQLiteTableSync::Field(TransportWebsiteTableSync::COL_USE_DATES_RANGE, SQL_INTEGER),
			SQLiteTableSync::Field(TransportWebsiteTableSync::COL_PERIODS, SQL_TEXT),
			SQLiteTableSync::Field(TransportWebsiteTableSync::COL_DISPLAY_ROAD_APPROACH_DETAILS, SQL_INTEGER),
			SQLiteTableSync::Field(TransportWebsiteTableSync::COL_CLIENT_URL, SQL_TEXT),
			SQLiteTableSync::Field(TransportWebsiteTableSync::COL_DEFAULT_PAGE_TEMPLATE_ID, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<TransportWebsiteTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<TransportWebsiteTableSync,TransportWebsite>::Load(
			TransportWebsite* site,
			const SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    site->setName(rows->getText (TransportWebsiteTableSync::TABLE_COL_NAME));
		    site->setStartDate(rows->getDate(TransportWebsiteTableSync::TABLE_COL_START_DATE));
		    site->setEndDate(rows->getDate(TransportWebsiteTableSync::TABLE_COL_END_DATE));
		    site->setOnlineBookingAllowed(rows->getBool(TransportWebsiteTableSync::TABLE_COL_ONLINE_BOOKING));
		    site->setPastSolutionsDisplayed(rows->getBool(TransportWebsiteTableSync::TABLE_COL_USE_OLD_DATA));
		    site->setMaxTransportConnectionsCount(rows->getInt(TransportWebsiteTableSync::COL_MAX_CONNECTIONS));
		    site->setUseDateRange(days(rows->getInt(TransportWebsiteTableSync::COL_USE_DATES_RANGE)));
			site->setDisplayRoadApproachDetail(rows->getBool(TransportWebsiteTableSync::COL_DISPLAY_ROAD_APPROACH_DETAILS));
			site->setClientURL(rows->getText(TransportWebsiteTableSync::COL_CLIENT_URL));
		    
		    string periodsStr(rows->getText(TransportWebsiteTableSync::COL_PERIODS));

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
				RegistryKeyType templateId(rows->getLongLong(TransportWebsiteTableSync::COL_DEFAULT_PAGE_TEMPLATE_ID));
				try
				{
					site->setDefaultTemplate(
						templateId == 0 ? NULL :
						WebPageTableSync::GetEditable(templateId, env, linkLevel).get()
					);
				}
				catch(ObjectNotFoundException<Webpage>& e)
				{
					Log::GetInstance().warn("No such webpage in site", e);
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<TransportWebsiteTableSync,TransportWebsite>::Unlink(
			TransportWebsite* obj
		){
		}


		template<> void SQLiteDirectTableSyncTemplate<TransportWebsiteTableSync,TransportWebsite>::Save(
			TransportWebsite* site,
			optional<SQLiteTransaction&> transaction
		){
			// Preparation
			stringstream periodstr;
			const TransportWebsite::Periods& periods(site->getPeriods());
			for(TransportWebsite::Periods::const_iterator it(periods.begin()); it != periods.end(); ++it)
			{
				if (it != periods.begin())
					periodstr << ",";
				periodstr << to_simple_string(it->getBeginHour())
					<< "|" << to_simple_string(it->getEndHour())
					<< "|" << it->getCaption();
			}

			// Query
			ReplaceQuery<TransportWebsiteTableSync> query(*site);
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
		TransportWebsiteTableSync::SearchResult TransportWebsiteTableSync::Search(
			Env& env,
			std::string name
			, int first /*= 0*/,
			boost::optional<std::size_t> number
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<TransportWebsiteTableSync> query;
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
