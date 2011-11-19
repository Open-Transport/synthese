
/** ResaDBLog class header.
	@file ResaDBLog.h

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

#ifndef SYNTHESE_RESA_DB_LOG
#define SYNTHESE_RESA_DB_LOG

#include <string>

#include "ResaTypes.h"
#include "DBLogTemplate.h"

namespace synthese
{
	namespace server
	{
		class Session;
	}

	namespace security
	{
		class User;
	}

	namespace resa
	{
		class ReservationTransaction;
		class Reservation;

		/** Reservation Log.
			@ingroup m51Logs refLogs

			Columns :
				- standard user_id : Person who made the action
				- standard object_id : Customer id
				- call id : ID of the log entry corresponding to the call
				- type :
					- call
					- reservation
					- cancellation
					- customer comment
					- information
					- redirection
					- technical support
					- no show
					- autoresa activation/deactivation
					- personal data update
					- password update
					- email
					- other
				- description : if type is reservation or cancellation, copy of main informations about the reservation, else manual typed text
				- second date, depends on the type :
					- call : date of the end of the call
					- reservation or cancellation : copy of the date of the departure
					- no show : date of the theorical departure
					- other : nothing
				- reservation id

		*/
		class ResaDBLog:
			public dblog::DBLogTemplate<ResaDBLog>
		{
		public:

			typedef enum
			{
				CALL_ENTRY = 10,
				RADIO_CALL = 12,
				AUTO_CALL_ENTRY = 14,
				FAKE_CALL = 15,
				OUTGOING_CALL = 16,
				RESERVATION_ENTRY = 20,
				RESERVATION_UPDATE = 25,
				CANCELLATION_ENTRY = 30,
				DELAYED_CANCELLATION_ENTRY = 35,
				CUSTOMER_COMMENT_ENTRY = 40,
				INFORMATION_ENTRY = 50,
				REDIRECTION_ENTRY = 60,
				RESERVATIONS_READ_ENTRY = 65,
				TECHNICAL_SUPPORT_ENTRY = 70,
				NO_SHOW_ENTRY = 75,
				AUTORESA_ACTIVATION = 80,
				AUTORESA_DEACTIVATION = 81,
				PERSONAL_DATA_UPDATE = 85,
				CUSTOMER_CREATION_ENTRY = 86,
				PASSWORD_UPDATE = 90,
				EMAIL = 95,
				OTHER = 100
			} _EntryType;

			static const std::size_t COL_TYPE;
			static const std::size_t COL_DATE2;
			static const std::size_t COL_TEXT;
			static const std::size_t COL_RESA;

			std::string getName() const;
			DBLog::ColumnsVector getColumnNames() const;

			virtual std::string getObjectName(
				util::RegistryKeyType id,
				const server::Request& searchRequest
			) const;

			//////////////////////////////////////////////////////////////////////////
			/// Object 1 column name getter.
			/// If empty result, then the column is not displayed (default behavior).
			virtual std::string getObjectColumnName() const;



			virtual DBLog::ColumnsVector parse(
				const dblog::DBLogEntry& entry,
				const server::Request& searchRequest
			) const;

			static util::RegistryKeyType AddCallEntry(const security::User* user);

			static void AddCallInformationEntry(
				const dblog::DBLogEntry& callEntry,
				const ResaDBLog::_EntryType& type,
				const std::string& text,
				const security::User& user
			);
			static void UpdateCallEntryDate(util::RegistryKeyType callId);
			static void UpdateCallEntryCustomer(util::RegistryKeyType callId, util::RegistryKeyType customerId);

			static void AddBookReservationEntry(
				const server::Session* user,
				const ReservationTransaction& transaction
			);
			static void AddCancelReservationEntry(
				const server::Session* user,
				const ReservationTransaction& transaction,
				ReservationStatus oldStatus
			);

			static void AddUserAdminEntry(
				const server::Session& session,
				const security::User& subject,
				const std::string& text
			);

			static void AddCustomerCreationEntry(
				const server::Session& session,
				const security::User& subject
			);

			static void AddUserChangeAutoResaActivationEntry(
				const server::Session& session,
				const security::User& subject
			);

			static void AddEMailEntry(
				const server::Session& session,
				const security::User& subject,
				const std::string& text
			);

			static void AddPasswordInitEntry(
				const server::Session& session,
				const security::User& subject
			);

			static void AddReservationUpdateEntry(
				const server::Session& session,
				const Reservation& reservation,
				const std::string& detail
			);


			//////////////////////////////////////////////////////////////////////////
			/// Builds an HTML image representing an entry type.
			/// @param type of entry
			/// @return URL of the icon
			static std::string GetIconURL(
				const ResaDBLog::_EntryType& type
			);

			static std::string GetText(
				const ResaDBLog::_EntryType& type
			);

			static std::string GetIcon(
				const ResaDBLog::_EntryType& type
			);
		};
	}
}

#endif

