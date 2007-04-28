
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

#include "31_resa/OnlineReservationRuleTableSync.h"
#include "31_resa/OnlineReservationRule.h"
#include "31_resa/ModuleClass.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

#include <sstream>

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace resa;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<OnlineReservationRule>::TABLE_NAME = "t046_reservations";
		template<> const int SQLiteTableSyncTemplate<OnlineReservationRule>::TABLE_ID = 46;
		template<> const bool SQLiteTableSyncTemplate<OnlineReservationRule>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<OnlineReservationRule>::load(OnlineReservationRule* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			/// @todo Set all other attributes from the row
		}

		template<> void SQLiteTableSyncTemplate<OnlineReservationRule>::save(OnlineReservationRule* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId(1,1));	/// @todo Use grid ID
               
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
			: SQLiteTableSyncTemplate<OnlineReservationRule>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
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

		void OnlineReservationRuleTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			// 
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				if (.contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					load(ResaModule::getOnlineReservationRules().get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))), rows, i);
				}
				else
				{
					OnlineReservationRule* object = new OnlineReservationRule;
					load(object, rows, i);
					ResaModule::getOnlineReservationRules().add(object);
				}
			}
		}
		
		void OnlineReservationRuleTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (ResaModule::getOnlineReservationRules().contains(id))
				{
					OnlineReservationRule* object = ResaModule::getOnlineReservationRules().get(id);
					load(object, rows, i);
				}
			}
		}

		void OnlineReservationRuleTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (ResaModule::getOnlineReservationRules().contains(id))
				{
					ResaModule::getOnlineReservationRules().remove(id);
				}
			}
		}

		std::vector<OnlineReservationRule*> OnlineReservationRuleTableSync::search(int first /*= 0*/, int number /*= 0*/ )
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
				vector<OnlineReservationRule*> objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					OnlineReservationRule* object = new OnlineReservationRule();
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
