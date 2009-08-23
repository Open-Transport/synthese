
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

#include "ReservationContactTableSync.h"
#include "InterfaceTableSync.h"
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
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,OnlineReservationRuleTableSync>::FACTORY_KEY(
			"31.5 Online Reservation Rule Table Sync"
		);
	}

	namespace resa
	{
		const string OnlineReservationRuleTableSync::COL_RESERVATION_CONTACT_ID = "reservation_rule_id";
		const string OnlineReservationRuleTableSync::COL_EMAIL = "email";
		const string OnlineReservationRuleTableSync::COL_COPY_EMAIL = "copy_email";
		const string OnlineReservationRuleTableSync::COL_NEEDS_SURNAME = "needs_surname";
		const string OnlineReservationRuleTableSync::COL_NEEDS_ADDRESS = "needs_address";
		const string OnlineReservationRuleTableSync::COL_NEEDS_PHONE = "needs_phone";
		const string OnlineReservationRuleTableSync::COL_NEEDS_EMAIL = "needs_email";
		const string OnlineReservationRuleTableSync::COL_NEEDS_CUSTOMER_NUMBER = "needs_customer_number";
		const string OnlineReservationRuleTableSync::COL_MAX_SEATS = "max_seat";
		const string OnlineReservationRuleTableSync::COL_THRESHOLDS = "thresholds";
		const string OnlineReservationRuleTableSync::COL_SENDER_EMAIL("sender_email");
		const string OnlineReservationRuleTableSync::COL_SENDER_NAME("sender_name");
		const string OnlineReservationRuleTableSync::COL_EMAIL_INTERFACE_ID("email_interface_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<OnlineReservationRuleTableSync>::TABLE(
				"t047_online_reservation_rules"
				);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<OnlineReservationRuleTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_RESERVATION_CONTACT_ID, SQL_INTEGER),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_EMAIL, SQL_TEXT),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_COPY_EMAIL, SQL_TEXT),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_NEEDS_SURNAME, SQL_INTEGER),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_NEEDS_ADDRESS, SQL_INTEGER),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_NEEDS_PHONE, SQL_INTEGER),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_NEEDS_EMAIL, SQL_INTEGER),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_NEEDS_CUSTOMER_NUMBER, SQL_INTEGER),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_MAX_SEATS, SQL_INTEGER),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_THRESHOLDS, SQL_INTEGER),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_SENDER_EMAIL, SQL_TEXT),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_SENDER_NAME, SQL_TEXT),
			SQLiteTableSync::Field(OnlineReservationRuleTableSync::COL_EMAIL_INTERFACE_ID, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<OnlineReservationRuleTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(OnlineReservationRuleTableSync::COL_RESERVATION_CONTACT_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::Load(
			OnlineReservationRule* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setEMail(rows->getText(OnlineReservationRuleTableSync::COL_EMAIL));
			object->setCopyEMail(rows->getText(OnlineReservationRuleTableSync::COL_COPY_EMAIL));
			object->setMaxSeats(rows->getInt(OnlineReservationRuleTableSync::COL_MAX_SEATS));
			/// @todo Finish to implement the loader

			object->setSenderEMail(rows->getText(OnlineReservationRuleTableSync::COL_SENDER_EMAIL));
			object->setSenderName(rows->getText(OnlineReservationRuleTableSync::COL_SENDER_NAME));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				try
				{
					object->setReservationContact(
						ReservationContactTableSync::Get(
							rows->getLongLong(OnlineReservationRuleTableSync::COL_RESERVATION_CONTACT_ID),
							env,
							linkLevel
						).get()
					);
				}
				catch (...)
				{
					Log::GetInstance().warn("Reservation rule not found for online reservation rule "+ Conversion::ToString(object->getKey()));
				}

				try
				{
					object->setEMailInterface(
						InterfaceTableSync::GetEditable(
							rows->getLongLong(OnlineReservationRuleTableSync::COL_EMAIL_INTERFACE_ID),
							env,
							linkLevel
						).get()
					);
				}
				catch (...)
				{
					Log::GetInstance().warn("E-Mail interface not found for online reservation rule "+ Conversion::ToString(object->getKey()));
				}

			}
		}


		template<> void SQLiteDirectTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::Unlink(
			OnlineReservationRule* object
		){
		}


		template<> void SQLiteDirectTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::Save(OnlineReservationRule* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace resa
	{
		OnlineReservationRuleTableSync::SearchResult OnlineReservationRuleTableSync::Search(
			Env& env,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << (number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
