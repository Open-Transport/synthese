/** ResaDBLog class implementation.
	@file ResaDBLog.cpp

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

#include "ResaDBLog.h"
#include "ResaRight.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "ResaLogAdmin.h"
#include "Reservation.h"
#include "ReservationTransaction.h"
#include "ResaModule.h"
#include "ReservationTransactionTableSync.h"
#include "ReservationTransaction.h"
#include "ReservationTableSync.h"
#include "HTMLModule.h"
#include "User.h"
#include "UserTableSync.h"
#include "DBLogEntry.h"
#include "DBLogEntryTableSync.h"
#include "Session.h"
#include "AdminInterfaceElement.h"
#include "ResaEditLogEntryAdmin.h"
#include "CancelReservationAction.h"
#include "ResaCustomerAdmin.h"
#include "Language.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace dblog;
	using namespace util;
	using namespace resa;
	using namespace security;
	using namespace html;
	using namespace server;
	using namespace admin;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBLog, ResaDBLog>::FACTORY_KEY("ResaDBLog");
	}

	namespace resa
	{
		const size_t ResaDBLog::COL_TYPE(0);
		const size_t ResaDBLog::COL_DATE2(1);
		const size_t ResaDBLog::COL_TEXT(2);
		const size_t ResaDBLog::COL_RESA(3);


		std::string ResaDBLog::getObjectName(
			RegistryKeyType id,
			const server::Request& searchRequest
		) const	{
			RegistryTableType tableId = decodeTableId(id);

			if (tableId == UserTableSync::TABLE.ID)
			{
				AdminFunctionRequest<ResaCustomerAdmin> openRequest(
					dynamic_cast<const Request& >(searchRequest)
				);

				boost::shared_ptr<const User> user(
					UserTableSync::Get(
						id,
						*dynamic_cast<const Request& >(
							searchRequest
						).getFunction()->getEnv(),
						FIELDS_ONLY_LOAD_LEVEL
				)	);

				openRequest.getPage()->setUser(user);
				return HTMLModule::getHTMLLink(openRequest.getURL(), user->getFullName());
			}

			return DBLog::getObjectName(id, searchRequest);
		}



		std::string resa::ResaDBLog::getName() const
		{
			return string();
		}



		DBLog::ColumnsVector resa::ResaDBLog::getColumnNames() const
		{
			DBLog::ColumnsVector v;
			v.push_back("Evénement");
			v.push_back("Evénement");
			v.push_back("Description");
			v.push_back("Action");
			return v;
		}


		void ResaDBLog::AddBookReservationEntry(const server::Session* session, const ReservationTransaction& transaction)
		{
			const Reservation* r1(*transaction.getReservations().begin());
			RegistryKeyType callId(ResaModule::GetCurrentCallId(session));

			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(RESERVATION_ENTRY));
			content.push_back(to_iso_extended_string(r1->get<DepartureTime>().date()) +" "+to_simple_string(r1->get<DepartureTime>().time_of_day()));
			content.push_back("Réservation");
			content.push_back(lexical_cast<string>(transaction.getKey()));

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, session->getUser().get(), transaction.get<Customer>()->getKey(), callId);

			UpdateCallEntryCustomer(callId, transaction.get<Customer>()->getKey());
		}



		RegistryKeyType ResaDBLog::AddCallEntry(const security::User* user)
		{
			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(CALL_ENTRY));
			content.push_back(string());
			content.push_back("Réception d'appel");
			content.push_back(string());

			return _addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, 0);
		}

		void ResaDBLog::UpdateCallEntryDate( RegistryKeyType callId )
		{
			Env env;
			boost::shared_ptr<DBLogEntry> entry(DBLogEntryTableSync::GetEditable(callId, env));
			DBLogEntry::Content content(entry->getContent());
			ptime now(second_clock::local_time());
			content[COL_DATE2] = to_iso_extended_string(now.date()) + " "+to_simple_string(now.time_of_day());
			entry->setContent(content);

			DBLogEntryTableSync::Save(entry.get());
		}

		void ResaDBLog::UpdateCallEntryCustomer(RegistryKeyType callId, RegistryKeyType customerId )
		{
			try
			{
				Env env;
				boost::shared_ptr<DBLogEntry> entry(DBLogEntryTableSync::GetEditable(callId, env));
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
			const Reservation* r1(*transaction.getReservations().begin());
			RegistryKeyType callId(ResaModule::GetCurrentCallId(session));
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
			case DONE:
			case SHOULD_BE_DONE:
			case SHOULD_BE_AT_WORK:
				type = ResaDBLog::NO_SHOW_ENTRY;
				description = "Absence";
				level = DBLogEntry::DB_LOG_ERROR;
				break;

			default:
				assert(false);
			}

			content.push_back(lexical_cast<string>(type));
			content.push_back(to_iso_extended_string(r1->get<DepartureTime>().date())+" "+to_simple_string(r1->get<DepartureTime>().time_of_day()));
			content.push_back(description);
			content.push_back(lexical_cast<string>(transaction.getKey()));

			_addEntry(FACTORY_KEY, level, content, session->getUser().get(), transaction.get<Customer>()->getKey(), callId);

			UpdateCallEntryCustomer(callId, transaction.get<Customer>()->getKey());
		}



		DBLog::ColumnsVector ResaDBLog::parse(
			const dblog::DBLogEntry& entry,
			const Request& searchRequest
		) const	{
			DBLog::ColumnsVector result;
			const DBLogEntry::Content& content(entry.getContent());

			if(content.size() <= COL_RESA || entry.getDate().is_not_a_date_time())
			{
				ColumnsVector cv;
				cv.push_back(GetIcon(OTHER));
				cv.push_back("Données incomplètes");
				stringstream s;
				BOOST_FOREACH(const string& col, content)
				{
					s << col << " ";
				}
				cv.push_back(s.str());
				cv.push_back(string());
				return ColumnsVector(cv);
			}

			// Rights
			bool writingRight(searchRequest.getUser()->getProfile()->isAuthorized<ResaRight>(WRITE,UNKNOWN_RIGHT_LEVEL));

			ResaDBLog::_EntryType entryType(static_cast<ResaDBLog::_EntryType>(lexical_cast<int>(content[ResaDBLog::COL_TYPE])));
			boost::shared_ptr<ReservationTransaction> tr;
			ReservationStatus status(NO_RESERVATION);
			Env env;

			try
			{
				if (lexical_cast<RegistryKeyType>(content[ResaDBLog::COL_RESA]) > 0)
				{
					tr = ReservationTransactionTableSync::GetEditable(lexical_cast<RegistryKeyType>(content[ResaDBLog::COL_RESA]), env);

					ReservationTableSync::SearchResult reservations(
						ReservationTableSync::Search(env, tr->getKey())
					);

					//ReservationTableSync::search(tr.get());
					status = tr->getStatus();
				}
			}
			catch(bad_lexical_cast&)
			{
			}

			// Column Symbol
			result.push_back(GetIcon(entryType));
			result.push_back(GetText(entryType));

			// Column Description
			{
				stringstream stream;

				switch (entryType)
				{
				case CALL_ENTRY:
				case OUTGOING_CALL:
				case RADIO_CALL:
					{
						try
						{
							ptime d(time_from_string(content[ResaDBLog::COL_DATE2]));
							stream << "Jusqu'à " << d << " (" << (d - entry.getDate()).total_seconds() << " s)";
						}
						catch(...)
						{
						}
					}
					break;

				case ResaDBLog::RESERVATION_ENTRY:
					if(tr.get())
					{
						ResaModule::DisplayReservations(
							stream,
							*tr,
							searchRequest.getUser()->getLanguage() ? *searchRequest.getUser()->getLanguage() : synthese::Language::GetLanguageFromIso639_1Code("fr")
						);
						stream << "<br />Statut actuel de la réservation : " << HTMLModule::getHTMLImage("/admin/img/" + ResaModule::GetStatusIcon(status), tr->getFullStatusText()) << " " << tr->getFullStatusText();
					}

					break;

				case ResaDBLog::CANCELLATION_ENTRY:
				case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
				case ResaDBLog::NO_SHOW_ENTRY:
					if(tr.get())
					{
						ResaModule::DisplayReservations(
							stream,
							*tr,
							searchRequest.getUser()->getLanguage() ? *searchRequest.getUser()->getLanguage() : synthese::Language::GetLanguageFromIso639_1Code("fr")
						);
					}
					break;

				default:
					stream << content[ResaDBLog::COL_TEXT];
				}

				result.push_back(stream.str());
			}


			if (writingRight)
			{
				stringstream stream;
				if(entryType == RESERVATION_ENTRY)
				{
					// Cancel request
					AdminActionFunctionRequest<CancelReservationAction, ResaLogAdmin> cancelRequest(searchRequest);
					cancelRequest.getAction()->setTransaction(tr);
					
					switch(status)
					{
					case OPTION:
						stream << HTMLModule::getLinkButton(
							cancelRequest.getURL(),
							"Annuler",
							"Etes-vous sûr de vouloir annuler la réservation ?",
							ResaModule::GetStatusIcon(CANCELLED)
						);
						break;

					case TO_BE_DONE:
						stream << HTMLModule::getLinkButton(
							cancelRequest.getURL(),
							"Annuler hors délai",
							"Etes-vous sûr de vouloir annuler la réservation (hors délai) ?",
							ResaModule::GetStatusIcon(CANCELLED_AFTER_DELAY)
						);
						break;

					case AT_WORK:
						stream << HTMLModule::getLinkButton(
							cancelRequest.getURL() + "&absence=1",
							"Noter absence",
							"Etes-vous sûr de noter l'absence du client à l'arrêt ?",
							ResaModule::GetStatusIcon(NO_SHOW)
						);
						break;
						
					default:
						break;
					}
					stream << " ";
				}

				if(	entryType == CALL_ENTRY ||
					entryType == FAKE_CALL ||
					entryType == RADIO_CALL ||
					entryType == OUTGOING_CALL ||
					entry.getObjectId2() > 0
				) try {
					AdminFunctionRequest<ResaEditLogEntryAdmin> openCallRequest(
						dynamic_cast<const Request& >(searchRequest)
					);
					openCallRequest.getPage()->setEntry(
						DBLogEntryTableSync::Get(
							entry.getObjectId2() > 0 ? entry.getObjectId2() : entry.getKey(),
							*dynamic_cast<const Request& >(
								searchRequest
							).getFunction()->getEnv()
					)	);
					stream << HTMLModule::getLinkButton(openCallRequest.getURL(), "Ouvrir", string(), "/admin/img/" + GetIconURL(CALL_ENTRY));
				} catch(...)
				{

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
			c.push_back(lexical_cast<string>(type));
			c.push_back(string());
			c.push_back(text);
			c.push_back(string());

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, c, &user, callEntry.getObjectId(), callEntry.getKey());
		}

		std::string ResaDBLog::GetIconURL(
			const ResaDBLog::_EntryType& type
		){
			switch (type)
			{
			case CALL_ENTRY:
				return "phone.png";

			case OUTGOING_CALL:
				return "phone_sound.png";

			case ResaDBLog::RESERVATION_ENTRY:
				return "resa_compulsory.png";

			case ResaDBLog::RESERVATION_UPDATE:
				return "resa_compulsory.png";

			case ResaDBLog::CANCELLATION_ENTRY:
				return ResaModule::GetStatusIcon(CANCELLED);

			case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
				return ResaModule::GetStatusIcon(CANCELLED_AFTER_DELAY);

			case ResaDBLog::NO_SHOW_ENTRY:
				return ResaModule::GetStatusIcon(NO_SHOW);

			case ResaDBLog::CUSTOMER_COMMENT_ENTRY:
				return "user_comment.png";

			case ResaDBLog::INFORMATION_ENTRY:
				return "information.png";

			case ResaDBLog::REDIRECTION_ENTRY:
				return "lorry.png";

			case ResaDBLog::RESERVATIONS_READ_ENTRY:
				return "table.png";

			case ResaDBLog::TECHNICAL_SUPPORT_ENTRY:
				return "wrench.png";

			case ResaDBLog::RADIO_CALL:
				return "transmit.png";

			case ResaDBLog::FAKE_CALL:
				return "keyboard.png";

			case ResaDBLog::EMAIL:
				return "email.png";

			case ResaDBLog::AUTORESA_ACTIVATION:
				return "application_form_add.png";

			case ResaDBLog::AUTORESA_DEACTIVATION:
				return "application_form_delete.png";

			case ResaDBLog::CUSTOMER_CREATION_ENTRY:
				return "user_add.png";

			case ResaDBLog::PASSWORD_UPDATE:
				return "key.png";

			case ResaDBLog::PERSONAL_DATA_UPDATE:
				return "user_edit.png";

			case ResaDBLog::OTHER:
			default:
				return "help.png";
			}
		}

		std::string ResaDBLog::GetIcon( const ResaDBLog::_EntryType& type )
		{
			return HTMLModule::getHTMLImage(
				"/admin/img/" + GetIconURL(type),
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

			case ResaDBLog::RESERVATION_UPDATE:
				return "Mise à jour de réservation";

			case ResaDBLog::CANCELLATION_ENTRY:
				return "Annulation de réservation";

			case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
				return "Annulation de réservation hors délai";

			case ResaDBLog::NO_SHOW_ENTRY:
				return "Absence";

			case ResaDBLog::CUSTOMER_COMMENT_ENTRY:
				return "Réclamation";

			case ResaDBLog::INFORMATION_ENTRY:
				return "Information";

			case ResaDBLog::REDIRECTION_ENTRY:
				return "Redirection vers ligne régulière";

			case ResaDBLog::RESERVATIONS_READ_ENTRY:
				return "Lecture liste de réservations";

			case ResaDBLog::TECHNICAL_SUPPORT_ENTRY:
				return "Support technique";

			case ResaDBLog::RADIO_CALL:
				return "Appel radio / service";

			case ResaDBLog::FAKE_CALL:
				return "Saisie";

			case ResaDBLog::EMAIL:
				return "E-mail";

			case ResaDBLog::OUTGOING_CALL:
				return "Appel sortant";

			case ResaDBLog::AUTORESA_ACTIVATION:
				return "Activation auto réservation";

			case ResaDBLog::AUTORESA_DEACTIVATION:
				return "Desactivation auto réservation";

			case ResaDBLog::CUSTOMER_CREATION_ENTRY:
				return "Création du client";

			case ResaDBLog::PASSWORD_UPDATE:
				return "Changement de mot de passe";

			case ResaDBLog::PERSONAL_DATA_UPDATE:
				return "Modification des informations de contact";

			case ResaDBLog::OTHER:
			default:
				return "Autre";
			}
		}

		std::string ResaDBLog::getObjectColumnName() const
		{
			return "Client";
		}



		void ResaDBLog::AddUserAdminEntry(
			const server::Session& session,
			const User& subject,
			const std::string& text
		){
			RegistryKeyType callId(ResaModule::GetCurrentCallId(&session));

			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(PERSONAL_DATA_UPDATE));
			content.push_back(string());
			content.push_back(text);
			content.push_back(string());

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, session.getUser().get(), subject.getKey(), callId);

			UpdateCallEntryCustomer(callId, subject.getKey());
		}



		void ResaDBLog::AddUserChangeAutoResaActivationEntry( const server::Session& session, const User& subject )
		{
			RegistryKeyType callId(ResaModule::GetCurrentCallId(&session));

			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(
					(subject.getProfile()->getKey() == ResaModule::GetBasicResaCustomerProfile()->getKey()) ?
					AUTORESA_DEACTIVATION :
					AUTORESA_ACTIVATION
			)	);
			content.push_back(string());
			content.push_back(string());
			content.push_back(string());

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, session.getUser().get(), subject.getKey(), callId);

			UpdateCallEntryCustomer(callId, subject.getKey());
		}



		void ResaDBLog::AddCustomerCreationEntry( const server::Session& session, const User& subject )
		{
			RegistryKeyType callId(ResaModule::GetCurrentCallId(&session));

			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(CUSTOMER_CREATION_ENTRY));
			content.push_back(string());
			content.push_back(subject.getFullName());
			content.push_back(string());

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, session.getUser().get(), subject.getKey(), callId);

			UpdateCallEntryCustomer(callId, subject.getKey());
		}



		void ResaDBLog::AddEMailEntry( const server::Session& session, const User& subject, const std::string& text )
		{
			RegistryKeyType callId(ResaModule::GetCurrentCallId(&session));

			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(EMAIL));
			content.push_back(string());
			content.push_back(text);
			content.push_back(string());

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, session.getUser().get(), subject.getKey(), callId);

			UpdateCallEntryCustomer(callId, subject.getKey());
		}



		void ResaDBLog::AddPasswordInitEntry( const server::Session& session, const security::User& subject )
		{
			RegistryKeyType callId(ResaModule::GetCurrentCallId(&session));

			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(PASSWORD_UPDATE));
			content.push_back(string());
			content.push_back("Réinitialisation aléatoire");
			content.push_back(string());

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, session.getUser().get(), subject.getKey(), callId);

			UpdateCallEntryCustomer(callId, subject.getKey());
		}



		void ResaDBLog::AddReservationUpdateEntry(
			const server::Session& session,
			const Reservation& reservation,
			const std::string& detail
		){
			RegistryKeyType callId(ResaModule::GetCurrentCallId(&session));

			DBLog::ColumnsVector content;
			content.push_back(lexical_cast<string>(RESERVATION_UPDATE));
			content.push_back(string());
			content.push_back(detail);
			content.push_back(lexical_cast<string>(reservation.get<Transaction>()->getKey()));

			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, session.getUser().get(), reservation.getKey(), callId);
		}
	}
}
