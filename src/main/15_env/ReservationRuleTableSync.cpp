

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

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "15_env/EnvModule.h"
#include "ReservationRule.h"
#include "ReservationRuleTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<ReservationRule>::TABLE_NAME = "t021_reservation_rules";
		template<> const int SQLiteTableSyncTemplate<ReservationRule>::TABLE_ID = 21;
		template<> const bool SQLiteTableSyncTemplate<ReservationRule>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<ReservationRule>::load(ReservationRule* rr, const db::SQLiteResult& rows, int rowIndex/*=0*/ )
		{
			rr->setKey(Conversion::ToLongLong(rows.getColumn(rowIndex, TABLE_COL_ID)));
			ReservationRule::ReservationType type = (ReservationRule::ReservationType) Conversion::ToInt (rows.getColumn (rowIndex,ReservationRuleTableSync::COL_TYPE));

			bool online (
				Conversion::ToBool (rows.getColumn (rowIndex,ReservationRuleTableSync::COL_ONLINE)));

			bool originIsReference (
				Conversion::ToBool (rows.getColumn (rowIndex,ReservationRuleTableSync::COL_ORIGINISREFERENCE)));

			int minDelayMinutes = Conversion::ToInt (rows.getColumn (rowIndex,ReservationRuleTableSync::COL_MINDELAYMINUTES));
			int minDelayDays = Conversion::ToInt (rows.getColumn (rowIndex,ReservationRuleTableSync::COL_MINDELAYDAYS));
			int maxDelayDays = Conversion::ToInt (rows.getColumn (rowIndex,ReservationRuleTableSync::COL_MAXDELAYDAYS));

			synthese::time::Hour hourDeadline = synthese::time::Hour::FromSQLTime (rows.getColumn (rowIndex,ReservationRuleTableSync::COL_HOURDEADLINE));

			std::string phoneExchangeNumber (
				rows.getColumn (rowIndex,ReservationRuleTableSync::COL_PHONEEXCHANGENUMBER));

			std::string phoneExchangeOpeningHours (
				rows.getColumn (rowIndex,ReservationRuleTableSync::COL_PHONEEXCHANGEOPENINGHOURS));

			std::string description (
				rows.getColumn (rowIndex,ReservationRuleTableSync::COL_DESCRIPTION));

			std::string webSiteUrl (
				rows.getColumn (rowIndex,ReservationRuleTableSync::COL_WEBSITEURL));

			rr->setType (type);
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

		template<> void SQLiteTableSyncTemplate<ReservationRule>::save(ReservationRule* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE_NAME << " SET "
					/// @todo fill fields [,]FIELD=VALUE
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId(1,1));
                query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					/// @todo fill other fields separated by ,
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		const std::string ReservationRuleTableSync::COL_TYPE ("reservation_type");
		const std::string ReservationRuleTableSync::COL_ONLINE ("online");
		const std::string ReservationRuleTableSync::COL_ORIGINISREFERENCE ("origin_is_reference");
		const std::string ReservationRuleTableSync::COL_MINDELAYMINUTES ("min_delay_minutes");
		const std::string ReservationRuleTableSync::COL_MINDELAYDAYS ("min_delay_days");
		const std::string ReservationRuleTableSync::COL_MAXDELAYDAYS ("max_delay_days");
		const std::string ReservationRuleTableSync::COL_HOURDEADLINE ("hour_deadline");
		const std::string ReservationRuleTableSync::COL_PHONEEXCHANGENUMBER ("phone_exchange_number");
		const std::string ReservationRuleTableSync::COL_PHONEEXCHANGEOPENINGHOURS ("phone_exchange_opening_hours");
		const std::string ReservationRuleTableSync::COL_DESCRIPTION ("description");
		const std::string ReservationRuleTableSync::COL_WEBSITEURL ("web_site_url");

		ReservationRuleTableSync::ReservationRuleTableSync()
			: SQLiteTableSyncTemplate<ReservationRule>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_TYPE, "INTEGER", true);
			addTableColumn (COL_ONLINE, "BOOLEAN", true);
			addTableColumn (COL_ORIGINISREFERENCE, "BOOLEAN", true);
			addTableColumn (COL_MINDELAYMINUTES, "INTEGER", true);
			addTableColumn (COL_MINDELAYDAYS, "INTEGER", true);
			addTableColumn (COL_MAXDELAYDAYS, "INTEGER", true);
			addTableColumn (COL_HOURDEADLINE, "TIME", true);
			addTableColumn (COL_PHONEEXCHANGENUMBER, "TEXT", true);
			addTableColumn (COL_PHONEEXCHANGEOPENINGHOURS, "TEXT", true);
			addTableColumn (COL_DESCRIPTION, "TEXT", true);
			addTableColumn (COL_WEBSITEURL, "TEXT", true);
		}

		void ReservationRuleTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				ReservationRule* object = new ReservationRule();
				load(object, rows, i);
				EnvModule::getReservationRules().add(object);
			}
		}

		void ReservationRuleTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getReservationRules().contains(id))
				{
					ReservationRule* object = EnvModule::getReservationRules().get(id);
					load(object, rows, i);
				}
			}
		}

		void ReservationRuleTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getReservationRules().contains(id))
				{
					EnvModule::getReservationRules().remove(id);
				}
			}
		}

		std::vector<ReservationRule*> ReservationRuleTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<ReservationRule*> objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					ReservationRule* object = new ReservationRule();
					load(object, result, i);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
