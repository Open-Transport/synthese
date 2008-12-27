

/** ReservationRuleTableSync class implementation.
	@file ReservationRuleTableSync.cpp

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

#include <sstream>

#include "ReservationRuleTableSync.h"
#include "ReservationRule.h"

#include "Conversion.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	template<> const string util::FactorableTemplate<SQLiteTableSync,ReservationRuleTableSync>::FACTORY_KEY("15.10.06 Reservation rules");

	namespace env
	{
		const string ReservationRuleTableSync::COL_TYPE ("reservation_type");
		const string ReservationRuleTableSync::COL_ONLINE ("online");
		const string ReservationRuleTableSync::COL_ORIGINISREFERENCE ("origin_is_reference");
		const string ReservationRuleTableSync::COL_MINDELAYMINUTES ("min_delay_minutes");
		const string ReservationRuleTableSync::COL_MINDELAYDAYS ("min_delay_days");
		const string ReservationRuleTableSync::COL_MAXDELAYDAYS ("max_delay_days");
		const string ReservationRuleTableSync::COL_HOURDEADLINE ("hour_deadline");
		const string ReservationRuleTableSync::COL_PHONEEXCHANGENUMBER ("phone_exchange_number");
		const string ReservationRuleTableSync::COL_PHONEEXCHANGEOPENINGHOURS ("phone_exchange_opening_hours");
		const string ReservationRuleTableSync::COL_DESCRIPTION ("description");
		const string ReservationRuleTableSync::COL_WEBSITEURL ("web_site_url");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<ReservationRuleTableSync>::TABLE(
			ReservationRuleTableSync::CreateFormat(
				"t021_reservation_rules",
				SQLiteTableFormat::CreateFields(
					SQLiteTableSync::Field(ReservationRuleTableSync::COL_TYPE, SQL_INTEGER),
					SQLiteTableSync::Field(ReservationRuleTableSync::COL_ONLINE, SQL_BOOLEAN),
					SQLiteTableSync::Field(ReservationRuleTableSync::COL_ORIGINISREFERENCE, SQL_BOOLEAN),
					SQLiteTableSync::Field(ReservationRuleTableSync::COL_MINDELAYMINUTES, SQL_INTEGER),
					SQLiteTableSync::Field(ReservationRuleTableSync::COL_MINDELAYDAYS, SQL_INTEGER),
					SQLiteTableSync::Field(ReservationRuleTableSync::COL_MAXDELAYDAYS, SQL_INTEGER),
					SQLiteTableSync::Field(ReservationRuleTableSync::COL_HOURDEADLINE, SQL_TIME),
					SQLiteTableSync::Field(ReservationRuleTableSync::COL_PHONEEXCHANGENUMBER, SQL_TEXT),
					SQLiteTableSync::Field(ReservationRuleTableSync::COL_PHONEEXCHANGEOPENINGHOURS, SQL_TEXT),
					SQLiteTableSync::Field(ReservationRuleTableSync::COL_DESCRIPTION, SQL_TEXT),
					SQLiteTableSync::Field(ReservationRuleTableSync::COL_WEBSITEURL, SQL_TEXT),
					SQLiteTableSync::Field()
				), SQLiteTableFormat::Indexes()
		)	);


		template<> void SQLiteDirectTableSyncTemplate<ReservationRuleTableSync,ReservationRule>::Load(
			ReservationRule* rr,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    bool online (rows->getBool (ReservationRuleTableSync::COL_ONLINE));

		    bool originIsReference (rows->getBool (ReservationRuleTableSync::COL_ORIGINISREFERENCE));
		    
		    int minDelayMinutes = rows->getInt (ReservationRuleTableSync::COL_MINDELAYMINUTES);
		    int minDelayDays = rows->getInt (ReservationRuleTableSync::COL_MINDELAYDAYS);
		    int maxDelayDays = rows->getInt (ReservationRuleTableSync::COL_MAXDELAYDAYS);
		    
		    synthese::time::Hour hourDeadline = 
			synthese::time::Hour::FromSQLTime (rows->getText (ReservationRuleTableSync::COL_HOURDEADLINE));
		    
		    string phoneExchangeNumber (
			rows->getText (ReservationRuleTableSync::COL_PHONEEXCHANGENUMBER));

		    string phoneExchangeOpeningHours (
			rows->getText (ReservationRuleTableSync::COL_PHONEEXCHANGEOPENINGHOURS));

		    string description (
			rows->getText (ReservationRuleTableSync::COL_DESCRIPTION));

		    string webSiteUrl (
			rows->getText (ReservationRuleTableSync::COL_WEBSITEURL));

			ReservationRuleType ruleType(static_cast<ReservationRuleType>(rows->getInt(ReservationRuleTableSync::COL_TYPE)));

			rr->setType(ruleType);
		    rr->setOnline (online);
		    rr->setOriginIsReference (originIsReference);
		    rr->setMinDelayMinutes (minDelayMinutes);
		    rr->setMinDelayDays (minDelayDays);
		    rr->setMaxDelayDays (maxDelayDays);
		    rr->setHourDeadLine (hourDeadline);
		    rr->setPhoneExchangeNumber (phoneExchangeNumber);
		    rr->setPhoneExchangeOpeningHours (phoneExchangeOpeningHours);
		    rr->setDescription (description);
		    rr->setWebSiteUrl (webSiteUrl);
		}


		template<> void SQLiteDirectTableSyncTemplate<ReservationRuleTableSync,ReservationRule>::Save(ReservationRule* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() == UNKNOWN_VALUE)
				object->setKey(getId());
            query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey()) << ","
				<< static_cast<int>(object->getType()) << ","
				<< Conversion::ToString(object->getOnline()) << ","
				<< object->getMinDelayMinutes() << ","
				<< object->getMinDelayDays() << ","
				<< object->getMaxDelayDays() << ","
				<< object->getHourDeadLine().toSQLString() << ","
				<< Conversion::ToSQLiteString(object->getPhoneExchangeNumber()) << ","
				<< Conversion::ToSQLiteString(object->getPhoneExchangeOpeningHours()) << ","
				<< Conversion::ToSQLiteString(object->getDescription()) << ","
				<< Conversion::ToSQLiteString(object->getWebSiteUrl())
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<ReservationRuleTableSync,ReservationRule>::Unlink(
			ReservationRule* obj
		){

		}

	}

	namespace env
	{
		ReservationRuleTableSync::ReservationRuleTableSync()
			: SQLiteRegistryTableSyncTemplate<ReservationRuleTableSync,ReservationRule>()
		{
		}



		void ReservationRuleTableSync::Search(
			Env& env,
			int first, /*= 0*/
			int number, /*= 0*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
