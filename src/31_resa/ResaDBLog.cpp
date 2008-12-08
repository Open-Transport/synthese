

/** ResaDBLog class implementation.
	@file ResaDBLog.cpp

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

#include "ResaDBLog.h"

#include "31_resa/Reservation.h"
#include "31_resa/ReservationTransaction.h"
#include "31_resa/ResaModule.h"
#include "31_resa/ReservationTransactionTableSync.h"
#include "31_resa/ReservationTransaction.h"
#include "31_resa/ReservationTableSync.h"

#include "04_time/DateTime.h"

#include "05_html/HTMLModule.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "01_util/Conversion.h"

#include "13_dblog/DBLogEntry.h"
#include "13_dblog/DBLogEntryTableSync.h"

#include "30_server/Session.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace dblog;
	using namespace util;
	using namespace resa;
	using namespace security;
	using namespace time;
	using namespace html;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBLog, ResaDBLog>::FACTORY_KEY("ResaDBLog");
	}

	namespace resa
	{
		const int ResaDBLog::COL_CALL(0);
		const int ResaDBLog::COL_TYPE(1);
		const int ResaDBLog::COL_DATE2(2);
		const int ResaDBLog::COL_TEXT(3);
		const int ResaDBLog::COL_RESA(4);


		std::string ResaDBLog::getObjectName( uid id ) const
		{
			int tableId = decodeTableId(id);

			if (tableId == UserTableSync::TABLE_ID)
			{
				shared_ptr<const User> user(UserTableSync::Get(id));
				return user->getFullName();
			}

			return Conversion::ToString(id);
		}



		std::string resa::ResaDBLog::getName() const
		{
			return "Réservation";
		}



		DBLog::ColumnsVector resa::ResaDBLog::getColumnNames() const
		{
			DBLog::ColumnsVector v;
			v.push_back("");
			v.push_back("Description");
			return v;
		}


		void ResaDBLog::AddBookReservationEntry(const server::Session* session, const ReservationTransaction& transaction)
		{
			shared_ptr<const Reservation> r1(*transaction.getReservations().begin());
			uid callId(ResaModule::GetCurrentCallId(session));

			DBLog::ColumnsVector content;
			content.push_back(Conversion::ToString(callId));
			content.push_back(Conversion::ToString(RESERVATION_ENTRY));
			content.push_back(r1->getDepartureTime().toSQLString(false));
			content.push_back("Réservation");
			content.push_back(Conversion::ToString(transaction.getKey()));

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, session->getUser().get(), transaction.getCustomerUserId());

			UpdateCallEntryCustomer(callId, transaction.getCustomerUserId());
		}

		uid ResaDBLog::AddCallEntry(const security::User* user)
		{
			DBLog::ColumnsVector content;
			content.push_back(Conversion::ToString(UNKNOWN_VALUE));
			content.push_back(Conversion::ToString(CALL_ENTRY));
			content.push_back(string());
			content.push_back("Réception d'appel");
			content.push_back(Conversion::ToString(UNKNOWN_VALUE));

			return _addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, UNKNOWN_VALUE);
		}

		void ResaDBLog::UpdateCallEntryDate( uid callId )
		{
			shared_ptr<DBLogEntry> entry(DBLogEntryTableSync::GetEditable(callId));
			DBLogEntry::Content content(entry->getContent());
			DateTime now(TIME_CURRENT);
			content[COL_DATE2] = now.toSQLString(false);
			entry->setContent(content);

			DBLogEntryTableSync::Save(entry.get());
		}

		void ResaDBLog::UpdateCallEntryCustomer( uid callId, uid customerId )
		{
			try
			{
				shared_ptr<DBLogEntry> entry(DBLogEntryTableSync::GetEditable(callId));
				entry->setObjectId(customerId);

				DBLogEntryTableSync::Save(entry.get());
			}
			catch(...)
			{

			}
		}



		void ResaDBLog::AddCancelReservationEntry( const server::Session* session, const ReservationTransaction& transaction, ReservationStatus oldStatus )
		{
			shared_ptr<const Reservation> r1(*transaction.getReservations().begin());
			uid callId(ResaModule::GetCurrentCallId(session));
			string description;
			DBLogEntry::Level level;
			DBLog::ColumnsVector content;
			_EntryType type(ResaDBLog::OTHER);

			switch (oldStatus)
			{
			case OPTION:
				type = ResaDBLog::CANCELLATION_ENTRY;
				description = "Annulation";
				level = DBLogEntry::DB_LOG_INFO;
				break;

			case TO_BE_DONE:
				type = ResaDBLog::DELAYED_CANCELLATION_ENTRY;
				description = "Annulation hors délai";
				level = DBLogEntry::DB_LOG_WARNING;
				break;

			case AT_WORK:
				type = ResaDBLog::NO_SHOW;
				description = "Absence";
				level = DBLogEntry::DB_LOG_ERROR;
				break;

			default:
				assert(false);
			}

			content.push_back(Conversion::ToString(callId));
			content.push_back(Conversion::ToString(type));
			content.push_back(r1->getDepartureTime().toSQLString(false));
			content.push_back(description);
			content.push_back(Conversion::ToString(transaction.getKey()));

			_addEntry(FACTORY_KEY, level, content, session->getUser().get(), transaction.getCustomerUserId());

			UpdateCallEntryCustomer(callId, transaction.getCustomerUserId());
		}



		DBLog::ColumnsVector ResaDBLog::parse( const dblog::DBLogEntry& entry) const
		{
			DBLog::ColumnsVector result;
			const DBLogEntry::Content& content(entry.getContent());

			ResaDBLog::_EntryType entryType(static_cast<ResaDBLog::_EntryType>(Conversion::ToInt(content[ResaDBLog::COL_TYPE])));
			shared_ptr<ReservationTransaction> tr;
			ReservationStatus status(NO_RESERVATION);
			
			if (Conversion::ToLongLong(content[ResaDBLog::COL_RESA]) > 0)
			{
				tr = ReservationTransactionTableSync::GetEditable(Conversion::ToLongLong(content[ResaDBLog::COL_RESA]));
				//ReservationTableSync::search(tr.get());
				status = tr->getStatus();
			}

			// Column Symbol
			{
				stringstream stream;
				switch (entryType)
				{
				case CALL_ENTRY:
					stream << HTMLModule::getHTMLImage("phone.png","Appel");
					break;

				case ResaDBLog::RESERVATION_ENTRY:
					stream << HTMLModule::getHTMLImage("resa_compulsory.png", "Réservation");
					stream << HTMLModule::getHTMLImage(ResaModule::GetStatusIcon(status), tr->getFullStatusText());
					break;

				case ResaDBLog::CANCELLATION_ENTRY:
					stream << HTMLModule::getHTMLImage("bullet_delete.png", "Annulation de réservation");
					break;

				case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
					stream << HTMLModule::getHTMLImage("error.png", "Annulation de réservation hors délai");
					break;

				case ResaDBLog::NO_SHOW:
					stream << HTMLModule::getHTMLImage("exclamation.png", "Absence");
					break;
				}
				result.push_back(stream.str());
			}

			// Column Description
			{
				stringstream stream;

				switch (entryType)
				{
				case CALL_ENTRY:
					{
						DateTime d(DateTime::FromSQLTimestamp(content[ResaDBLog::COL_DATE2]));
						stream << "APPEL";
						if (!d.isUnknown())
							stream << " jusqu'à " << d.toString() << " (" << (d.getSecondsDifference(entry.getDate())) << " s)";
					}
					break;

				case ResaDBLog::RESERVATION_ENTRY:
					ResaModule::DisplayReservations(stream, tr.get());
					break;

				case ResaDBLog::CANCELLATION_ENTRY:
					stream << "ANNULATION de : ";
					ResaModule::DisplayReservations(stream, tr.get());
					break;

				case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
					stream << "ANNULATION HORS DELAI de : ";
					ResaModule::DisplayReservations(stream, tr.get());
					break;

				case ResaDBLog::NO_SHOW:
					stream << "ABSENCE sur : ";
					ResaModule::DisplayReservations(stream, tr.get());
					break;
				}

				result.push_back(stream.str());
			}
			return result;
		}
	}
}
