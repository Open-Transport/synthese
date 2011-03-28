
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
#include "DBResult.hpp"
#include "DBException.hpp"
#include "ReplaceQuery.h"

#include <sstream>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace resa;
	using namespace pt;
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,OnlineReservationRuleTableSync>::FACTORY_KEY(
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
		template<> const DBTableSync::Format DBTableSyncTemplate<OnlineReservationRuleTableSync>::TABLE(
				"t047_online_reservation_rules"
				);

		template<> const DBTableSync::Field DBTableSyncTemplate<OnlineReservationRuleTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_RESERVATION_CONTACT_ID, SQL_INTEGER),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_EMAIL, SQL_TEXT),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_COPY_EMAIL, SQL_TEXT),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_NEEDS_SURNAME, SQL_INTEGER),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_NEEDS_ADDRESS, SQL_INTEGER),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_NEEDS_PHONE, SQL_INTEGER),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_NEEDS_EMAIL, SQL_INTEGER),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_NEEDS_CUSTOMER_NUMBER, SQL_INTEGER),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_MAX_SEATS, SQL_INTEGER),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_THRESHOLDS, SQL_TEXT),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_SENDER_EMAIL, SQL_TEXT),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_SENDER_NAME, SQL_TEXT),
			DBTableSync::Field(OnlineReservationRuleTableSync::COL_EMAIL_INTERFACE_ID, SQL_TEXT),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<OnlineReservationRuleTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(OnlineReservationRuleTableSync::COL_RESERVATION_CONTACT_ID.c_str(), ""),
			DBTableSync::Index()
		};

		template<> void DBDirectTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::Load(
			OnlineReservationRule* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setEMail(rows->getText(OnlineReservationRuleTableSync::COL_EMAIL));
			object->setCopyEMail(rows->getText(OnlineReservationRuleTableSync::COL_COPY_EMAIL));
			object->setMaxSeats(rows->getInt(OnlineReservationRuleTableSync::COL_MAX_SEATS));
			object->setNeedsAddress(rows->getTribool(OnlineReservationRuleTableSync::COL_NEEDS_ADDRESS));
			object->setNeedsCustomerNumber(rows->getTribool(OnlineReservationRuleTableSync::COL_NEEDS_CUSTOMER_NUMBER));
			object->setNeedsEMail(rows->getTribool(OnlineReservationRuleTableSync::COL_NEEDS_EMAIL));
			object->setNeedsPhone(rows->getTribool(OnlineReservationRuleTableSync::COL_NEEDS_PHONE));
			object->setNeedsSurname(rows->getTribool(OnlineReservationRuleTableSync::COL_NEEDS_SURNAME));
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


		template<> void DBDirectTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::Unlink(
			OnlineReservationRule* object
		){
		}


		template<> void DBDirectTableSyncTemplate<OnlineReservationRuleTableSync,OnlineReservationRule>::Save(
			OnlineReservationRule* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<OnlineReservationRuleTableSync> query(*object);
			query.addField(object->getReservationContact() ? object->getReservationContact()->getKey() : RegistryKeyType(0));
			query.addField(object->getEMail());
			query.addField(object->getCopyEMail());
			query.addField(object->getNeedsSurname());
			query.addField(object->getNeedsAddress());
			query.addField(object->getNeedsPhone());
			query.addField(object->getNeedsEMail());
			query.addField(object->getNeedsCustomerNumber());
			query.addField(object->getMaxSeats());
			query.addField(object->getThresholds());
			query.addField(object->getSenderEMail());
			query.addField(object->getSenderName());
			query.addField(object->getEMailInterface() ? object->getEMailInterface()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
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
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToDBString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << (number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
