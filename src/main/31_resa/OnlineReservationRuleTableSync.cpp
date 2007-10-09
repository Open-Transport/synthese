
/** OnlineReservationRuleTableSync class implementation.
	@file OnlineReservationRuleTableSync.cpp

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

#include "OnlineReservationRuleTableSync.h"

#include "15_env/ReservationRuleTableSync.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace resa;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,OnlineReservationRuleTableSync>::FACTORY_KEY("31.5 Online Reservation Rule Table Sync");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::TABLE_NAME = "t047_online_reservation_rules";
		template<> const int SQLiteTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::TABLE_ID = 47;
		template<> const bool SQLiteTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::load(OnlineReservationRule* object, const db::SQLiteResultSPtr& rows )
		{
			object->setKey(rows->getLongLong (TABLE_COL_ID));
			object->setEMail(rows->getText(OnlineReservationRuleTableSync::COL_EMAIL));
			object->setCopyEMail(rows->getText(OnlineReservationRuleTableSync::COL_COPY_EMAIL));
			object->setMaxSeats(rows->getInt(OnlineReservationRuleTableSync::COL_MAX_SEATS));
			/// @todo Finish to implement the loader

		}


		template<> void SQLiteTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::_link(OnlineReservationRule* object, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			try
			{
				object->setReservationRule(ReservationRuleTableSync::Get(rows->getLongLong(OnlineReservationRuleTableSync::COL_RESERVATION_RULE_ID), object, true, GET_AUTO));
			}
			catch (...)
			{
				Log::GetInstance().warn("Reservation rule not found for online reservation rule "+ Conversion::ToString(object->getKey()));
			}
		}


		template<> void SQLiteTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::_unlink(OnlineReservationRule* object)
		{
		}


		template<> void SQLiteTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::save(OnlineReservationRule* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace resa
	{
		const string OnlineReservationRuleTableSync::COL_RESERVATION_RULE_ID = "reservation_rule_id";
		const string OnlineReservationRuleTableSync::COL_EMAIL = "email";
		const string OnlineReservationRuleTableSync::COL_COPY_EMAIL = "copy_email";
		const string OnlineReservationRuleTableSync::COL_NEEDS_SURNAME = "needs_surname";
		const string OnlineReservationRuleTableSync::COL_NEEDS_ADDRESS = "needs_address";
		const string OnlineReservationRuleTableSync::COL_NEEDS_PHONE = "needs_phone";
		const string OnlineReservationRuleTableSync::COL_NEEDS_EMAIL = "needs_email";
		const string OnlineReservationRuleTableSync::COL_NEEDS_CUSTOMER_NUMBER = "needs_customer_number";
		const string OnlineReservationRuleTableSync::COL_MAX_SEATS = "max_seat";
		const string OnlineReservationRuleTableSync::COL_THRESHOLDS = "thresholds";

		OnlineReservationRuleTableSync::OnlineReservationRuleTableSync()
			: SQLiteRegistryTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_RESERVATION_RULE_ID, "INTEGER");
			addTableColumn(COL_EMAIL, "TEXT");
			addTableColumn(COL_COPY_EMAIL, "TEXT");
			addTableColumn(COL_NEEDS_SURNAME, "INTEGER");
			addTableColumn(COL_NEEDS_ADDRESS, "INTEGER");
			addTableColumn(COL_NEEDS_PHONE, "INTEGER");
			addTableColumn(COL_NEEDS_EMAIL, "INTEGER");
			addTableColumn(COL_NEEDS_CUSTOMER_NUMBER, "INTEGER");
			addTableColumn(COL_MAX_SEATS, "INTEGER");
			addTableColumn(COL_THRESHOLDS, "INTEGER");

			addTableIndex(COL_RESERVATION_RULE_ID);
		}

/*		void OnlineReservationRuleTableSync::rowsAdded(
			SQLite* sqlite
			, SQLiteSync* sync
			, const SQLiteResultSPtr& rows
			, bool isFirstSync
		){
			// 
			while (rows->next ())
			{
				if (ResaModule::getOnlineReservationRules().contains(rows->getLongLong (TABLE_COL_ID)))
				{
					load(ResaModule::getOnlineReservationRules().getUpdateable(rows->getLongLong (TABLE_COL_ID)).get(), rows);
				}
				else
				{
					shared_ptr<OnlineReservationRule> object(new OnlineReservationRule);
					load(object.get(), rows);
					ResaModule::getOnlineReservationRules().add(object);
				}
			}
		}
		
		void OnlineReservationRuleTableSync::rowsUpdated(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (ResaModule::getOnlineReservationRules().contains(id))
				{
					shared_ptr<OnlineReservationRule> object(ResaModule::getOnlineReservationRules().getUpdateable(id));
					load(object.get(), rows);
				}
			}
		}

		void OnlineReservationRuleTableSync::rowsRemoved( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (ResaModule::getOnlineReservationRules().contains(id))
				{
					ResaModule::getOnlineReservationRules().remove(id);
				}
			}
		}
*/
		vector<shared_ptr<OnlineReservationRule> > OnlineReservationRuleTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			SQLite* sqlite = DBModule::GetSQLite();
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
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<OnlineReservationRule> > objects;
				while (rows->next ())
				{
					shared_ptr<OnlineReservationRule> object(new OnlineReservationRule());
					load(object.get(), rows);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}

		OnlineReservationRuleTableSync::~OnlineReservationRuleTableSync()
		{

		}
	}
}
