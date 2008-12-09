
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

#include "ReservationRuleTableSync.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Conversion.h"

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
		template<> const std::string SQLiteTableSyncTemplate<OnlineReservationRuleTableSync>::TABLE_NAME = "t047_online_reservation_rules";
		template<> const int SQLiteTableSyncTemplate<OnlineReservationRuleTableSync>::TABLE_ID = 47;
		template<> const bool SQLiteTableSyncTemplate<OnlineReservationRuleTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::Load(
			OnlineReservationRule* object,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
			object->setEMail(rows->getText(OnlineReservationRuleTableSync::COL_EMAIL));
			object->setCopyEMail(rows->getText(OnlineReservationRuleTableSync::COL_COPY_EMAIL));
			object->setMaxSeats(rows->getInt(OnlineReservationRuleTableSync::COL_MAX_SEATS));
			/// @todo Finish to implement the loader

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				try
				{
					object->setReservationRule(ReservationRuleTableSync::Get(rows->getLongLong(OnlineReservationRuleTableSync::COL_RESERVATION_RULE_ID), env, linkLevel).get());
				}
				catch (...)
				{
					Log::GetInstance().warn("Reservation rule not found for online reservation rule "+ Conversion::ToString(object->getKey()));
				}
			}
		}


		template<> void SQLiteDirectTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::Unlink(
			OnlineReservationRule* object,
			Env* env
		){
		}


		template<> void SQLiteDirectTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::Save(OnlineReservationRule* object)
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



		void OnlineReservationRuleTableSync::Search(
			Env& env,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
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

			LoadFromQuery(query.str(), env, linkLevel);
		}

		OnlineReservationRuleTableSync::~OnlineReservationRuleTableSync()
		{

		}
	}
}
