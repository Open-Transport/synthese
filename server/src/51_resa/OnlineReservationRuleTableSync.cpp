
/** OnlineReservationRuleTableSync class implementation.
	@file OnlineReservationRuleTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "DBException.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "WebPageTableSync.h"
#include "ReplaceQuery.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"

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
	using namespace cms;

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
		const string OnlineReservationRuleTableSync::COL_CONFIRMATION_EMAIL_CMS_ID("confirmation_email_cms_id");
		const string OnlineReservationRuleTableSync::COL_CANCELLATION_EMAIL_CMS_ID("cancellation_email_cms_id");
		const string OnlineReservationRuleTableSync::COL_PASSWORD_EMAIL_CMS_ID("password_email_cms_id");
		const string OnlineReservationRuleTableSync::COL_MULTI_RESERVATIONS_EMAIL_CMS_ID("multi_reservations_email_cms_id");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<OnlineReservationRuleTableSync>::TABLE(
			"t047_online_reservation_rules"
		);

		template<> const Field DBTableSyncTemplate<OnlineReservationRuleTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(OnlineReservationRuleTableSync::COL_RESERVATION_CONTACT_ID, SQL_INTEGER),
			Field(OnlineReservationRuleTableSync::COL_EMAIL, SQL_TEXT),
			Field(OnlineReservationRuleTableSync::COL_COPY_EMAIL, SQL_TEXT),
			Field(OnlineReservationRuleTableSync::COL_NEEDS_SURNAME, SQL_INTEGER),
			Field(OnlineReservationRuleTableSync::COL_NEEDS_ADDRESS, SQL_INTEGER),
			Field(OnlineReservationRuleTableSync::COL_NEEDS_PHONE, SQL_INTEGER),
			Field(OnlineReservationRuleTableSync::COL_NEEDS_EMAIL, SQL_INTEGER),
			Field(OnlineReservationRuleTableSync::COL_NEEDS_CUSTOMER_NUMBER, SQL_INTEGER),
			Field(OnlineReservationRuleTableSync::COL_MAX_SEATS, SQL_INTEGER),
			Field(OnlineReservationRuleTableSync::COL_THRESHOLDS, SQL_TEXT),
			Field(OnlineReservationRuleTableSync::COL_SENDER_EMAIL, SQL_TEXT),
			Field(OnlineReservationRuleTableSync::COL_SENDER_NAME, SQL_TEXT),
			Field(OnlineReservationRuleTableSync::COL_CONFIRMATION_EMAIL_CMS_ID, SQL_TEXT),
			Field(OnlineReservationRuleTableSync::COL_CANCELLATION_EMAIL_CMS_ID, SQL_TEXT),
			Field(OnlineReservationRuleTableSync::COL_PASSWORD_EMAIL_CMS_ID, SQL_TEXT),
			Field(OnlineReservationRuleTableSync::COL_MULTI_RESERVATIONS_EMAIL_CMS_ID, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<OnlineReservationRuleTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(OnlineReservationRuleTableSync::COL_RESERVATION_CONTACT_ID.c_str(), ""));
			return r;
		}

		template<>
        void OldLoadSavePolicy<OnlineReservationRuleTableSync,OnlineReservationRule>::Load(
			OnlineReservationRule* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setEMail(rows->getText(OnlineReservationRuleTableSync::COL_EMAIL));
			object->setCopyEMail(rows->getText(OnlineReservationRuleTableSync::COL_COPY_EMAIL));
			object->setMaxSeats(rows->getOptionalUnsignedInt(OnlineReservationRuleTableSync::COL_MAX_SEATS));
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
					Log::GetInstance().warn("Reservation rule not found for online reservation rule "+ lexical_cast<string>(object->getKey()));
				}

				bool warningToDisplayForEmailCMS(true);
                try
                {
                    // CMS Confirmation EMail
					object->setConfirmationEMailCMS(
						WebPageTableSync::GetEditable(
							rows->getLongLong(OnlineReservationRuleTableSync::COL_CONFIRMATION_EMAIL_CMS_ID),
							env,
							linkLevel)
					);
					warningToDisplayForEmailCMS = false;
				}
				catch (...)
				{
                    Log::GetInstance().info("Confirmation E-Mail CMS webpage not found for online reservation rule "+ lexical_cast<string>(object->getKey()));
				}
                try
                {
                    // CMS Cancellation EMail
					object->setCancellationEMailCMS(
						WebPageTableSync::GetEditable(
							rows->getLongLong(OnlineReservationRuleTableSync::COL_CANCELLATION_EMAIL_CMS_ID),
							env,
							linkLevel)
					);
				}
				catch (...)
				{
                    Log::GetInstance().info("Cancellation E-Mail CMS webpage not found for online reservation rule "+ lexical_cast<string>(object->getKey()));
					warningToDisplayForEmailCMS = true;
				}
                try
                {
                    // CMS Password EMail
					object->setPasswordEMailCMS(
						WebPageTableSync::GetEditable(
							rows->getLongLong(OnlineReservationRuleTableSync::COL_PASSWORD_EMAIL_CMS_ID),
							env,
							linkLevel)
					);
				}
				catch (...)
				{
                    Log::GetInstance().info("Password E-Mail CMS webpage not found for online reservation rule "+ lexical_cast<string>(object->getKey()));
					warningToDisplayForEmailCMS = true;
				}
				try
				{
					// CMS Multi Reservations EMail
					object->setMultiReservationsEMailCMS(
						WebPageTableSync::GetEditable(
							rows->getLongLong(OnlineReservationRuleTableSync::COL_MULTI_RESERVATIONS_EMAIL_CMS_ID),
							env,
							linkLevel)
					);
				}
				catch (...)
				{
					Log::GetInstance().info("Multi Reservations E-Mail CMS webpage not found for online reservation rule "+ lexical_cast<string>(object->getKey()));
					warningToDisplayForEmailCMS = true;
				}

				if (warningToDisplayForEmailCMS)
				{
					Log::GetInstance().warn("E-Mail not found for online reservation rule "+ lexical_cast<string>(object->getKey()));
				}

			}
		}


		template<>
		void OldLoadSavePolicy<OnlineReservationRuleTableSync,OnlineReservationRule>::Unlink(
			OnlineReservationRule* object
		){
		}


		template<> void OldLoadSavePolicy<OnlineReservationRuleTableSync,OnlineReservationRule>::Save(
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
			query.addField(object->getMaxSeats() ? lexical_cast<string>(*object->getMaxSeats()) : string());
			// FIXME: set<int> serialization is not correct with MySQL.
			// thresholds is not used at the moment. This needs to be fixed once we start using it.
			//query.addField(object->getThresholds());
			query.addField(string(""));
			query.addField(object->getSenderEMail());
			query.addField(object->getSenderName());
			query.addField(object->getConfirmationEMailCMS() ? object->getConfirmationEMailCMS()->getKey() : RegistryKeyType(0));
			query.addField(object->getCancellationEMailCMS() ? object->getCancellationEMailCMS()->getKey() : RegistryKeyType(0));
			query.addField(object->getPasswordEMailCMS() ? object->getPasswordEMailCMS()->getKey() : RegistryKeyType(0));
			query.addField(object->getMultiReservationsEMailCMS() ? object->getMultiReservationsEMailCMS()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<OnlineReservationRuleTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<OnlineReservationRuleTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<OnlineReservationRuleTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<OnlineReservationRuleTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
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
