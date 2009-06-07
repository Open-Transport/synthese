

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
#include "ResaRight.h"
#include "Request.h"
#include "Reservation.h"
#include "ReservationTransaction.h"
#include "ResaModule.h"
#include "ReservationTransactionTableSync.h"
#include "ReservationTransaction.h"
#include "ReservationTableSync.h"
#include "DateTime.h"
#include "HTMLModule.h"
#include "User.h"
#include "UserTableSync.h"
#include "Conversion.h"
#include "DBLogEntry.h"
#include "DBLogEntryTableSync.h"
#include "Session.h"
#include "AdminRequest.h"
#include "ResaEditLogEntryAdmin.h"

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
	using namespace server;
	using namespace admin;

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

			if (tableId == UserTableSync::TABLE.ID)
			{
				Env env;
				shared_ptr<const User> user(UserTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
				return user->getFullName();
			}

			return Conversion::ToString(id);
		}



		std::string resa::ResaDBLog::getName() const
		{
			return "Réservation";
		}


		bool ResaDBLog::isAuthorized(
			const Request& request,
			const security::RightLevel& level
		) const {
			return request.isAuthorized<ResaRight>(level);
		}

		DBLog::ColumnsVector resa::ResaDBLog::getColumnNames() const
		{
			DBLog::ColumnsVector v;
			v.push_back("");
			v.push_back("Description");
			v.push_back("Action");
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
			Env env;
			shared_ptr<DBLogEntry> entry(DBLogEntryTableSync::GetEditable(callId, env));
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
				Env env;
				shared_ptr<DBLogEntry> entry(DBLogEntryTableSync::GetEditable(callId, env));
				entry->setObjectId(customerId);

				DBLogEntryTableSync::Save(entry.get());
			}
			catch(...)
			{

			}
		}



		void ResaDBLog::AddCancelReservationEntry(
			const server::Session* session,
			const ReservationTransaction& transaction,
			ReservationStatus oldStatus )
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



		DBLog::ColumnsVector ResaDBLog::parse(
			const dblog::DBLogEntry& entry,
			const Request& searchRequest
		) const	{
			DBLog::ColumnsVector result;
			const DBLogEntry::Content& content(entry.getContent());

			// Rights
			bool writingRight(searchRequest.isAuthorized<ResaRight>(WRITE,UNKNOWN_RIGHT_LEVEL));

			// Cancel request
			Request cancelRequest(searchRequest);
//			cancelRequest._setAction(shared_ptr<Action>(new CancelReservationAction));

			ResaDBLog::_EntryType entryType(static_cast<ResaDBLog::_EntryType>(Conversion::ToInt(content[ResaDBLog::COL_TYPE])));
			shared_ptr<ReservationTransaction> tr;
			ReservationStatus status(NO_RESERVATION);
			Env env;
			
			if (Conversion::ToLongLong(content[ResaDBLog::COL_RESA]) > 0)
			{
				tr = ReservationTransactionTableSync::GetEditable(Conversion::ToLongLong(content[ResaDBLog::COL_RESA]), env);
				//ReservationTableSync::search(tr.get());
				status = tr->getStatus();
			}

			// Column Symbol
			{
				stringstream stream;
				stream << GetIcon(entryType);

				if(entryType == ResaDBLog::RESERVATION_ENTRY)
				{
					stream << HTMLModule::getHTMLImage(ResaModule::GetStatusIcon(status), tr->getFullStatusText());
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

				default:
					stream << GetText(entryType) << "<br />" << content[ResaDBLog::COL_TEXT];
				}

				result.push_back(stream.str());
			}


			if (writingRight)
			{
				stringstream stream;
				switch (entryType)
				{
				case CALL_ENTRY:
				case FAKE_CALL:
				case RADIO_CALL:
					FunctionRequest<AdminRequest> openCallRequest(&searchRequest);
					openCallRequest.getFunction()->setPage<ResaEditLogEntryAdmin>();
					openCallRequest.setObjectId(entry.getKey());
					stream << HTMLModule::getLinkButton(openCallRequest.getURL(), "Ouvrir");
					break;
				}

				switch(status)
				{
				case OPTION:
					stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler", "Etes-vous sûr de vouloir annuler la réservation ?", "bullet_delete.png");
					break;

				case TO_BE_DONE:
					stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler hors délai", "Etes-vous sûr de vouloir annuler la réservation (hors délai) ?", "error.png");
					break;

				case AT_WORK:
					stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Noter absence", "Etes-vous sûr de noter l'absence du client à l'arrêt ?", "exclamation.png");
					break;
				}
				result.push_back(stream.str());
			}


			return result;
		}

		void ResaDBLog::AddCallInformationEntry(
			const DBLogEntry& callEntry,
			const ResaDBLog::_EntryType& type,
			const std::string& text,
			const security::User& user
		){
			DBLogEntry::Content c;
			c.push_back(lexical_cast<string>(callEntry.getKey()));
			c.push_back(lexical_cast<string>(type));
			c.push_back(string());
			c.push_back(text);
			c.push_back(string());

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, c, &user, callEntry.getObjectId());
		}

		std::string ResaDBLog::GetIconURL(
			const ResaDBLog::_EntryType& type
		){
			switch (type)
			{
			case CALL_ENTRY:
				return "phone.png";

			case ResaDBLog::RESERVATION_ENTRY:
				return "resa_compulsory.png";

			case ResaDBLog::CANCELLATION_ENTRY:
				return "bullet_delete.png";

			case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
				return "error.png";
				
			case ResaDBLog::NO_SHOW:
				return "exclamation.png";
				
			case ResaDBLog::CUSTOMER_COMMENT_ENTRY:
				return "user_comment.png";

			case ResaDBLog::INFORMATION_ENTRY:
				return "information.png";

			case ResaDBLog::REDIRECTION_ENTRY:
				return "lorry.png";

			case ResaDBLog::TECHNICAL_SUPPORT_ENTRY:
				return "wrench.png";

			case ResaDBLog::RADIO_CALL:
				return "transmit.png";

			case ResaDBLog::FAKE_CALL:
				return "keyboard.png";

			case ResaDBLog::EMAIL:
				return "email.png";

			case ResaDBLog::OTHER:
			default:
				return "help.png";
			}
		}

		std::string ResaDBLog::GetIcon( const ResaDBLog::_EntryType& type )
		{
			return HTMLModule::getHTMLImage(
				GetIconURL(type),
				GetText(type)
			);
		}

		std::string ResaDBLog::GetText( const ResaDBLog::_EntryType& type )
		{
			switch (type)
			{
			case CALL_ENTRY:
				return "Appel";

			case ResaDBLog::RESERVATION_ENTRY:
				return "Réservation";

			case ResaDBLog::CANCELLATION_ENTRY:
				return "Annulation de réservation";

			case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
				return "Annulation de réservation hors délai";

			case ResaDBLog::NO_SHOW:
				return "Absence";

			case ResaDBLog::CUSTOMER_COMMENT_ENTRY:
				return "Réclamation";

			case ResaDBLog::INFORMATION_ENTRY:
				return "Information";

			case ResaDBLog::REDIRECTION_ENTRY:
				return "Redirection vers ligne régulière";

			case ResaDBLog::TECHNICAL_SUPPORT_ENTRY:
				return "Support technique";

			case ResaDBLog::RADIO_CALL:
				return "Appel radio / service";

			case ResaDBLog::FAKE_CALL:
				return "Saisie";

			case ResaDBLog::EMAIL:
				return "E-mail";

			case ResaDBLog::OTHER:
			default:
				return "Autre";
			}
		}
	}
}
