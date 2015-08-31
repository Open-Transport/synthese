/** NotificationLog class header.
	@file NotificationLog.hpp
	@author Yves Martin
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_messages_NotificationLog_hpp__
#define SYNTHESE_messages_NotificationLog_hpp__

#include <DBLogTemplate.h>

#include <string>

namespace synthese
{
	namespace messages
	{
		// Forward declaration
		class NotificationEvent;

		/**
			Log for notification events.

			Entries are:
			- NotificationEvent, success or failure after each attempt

			Colums:
			- Free text log "Message"
			- Free text "Action"
			- Link to source NotificationProvider
			- Link to Alarm message

			@ingroup m17Logs refLogs
			@author Yves Martin
			@date 2015
		 */
		class NotificationLog:
			public dblog::DBLogTemplate<NotificationLog>
		{
		public:
			/// Log name
			std::string getName() const;

			/// Terminus log columns
			DBLog::ColumnsVector getColumnNames() const;

			/// Column name for object_id
			virtual std::string getObjectColumnName() const;

			/// Converts registry key (object_id) into object name
			virtual std::string getObjectName(
				util::RegistryKeyType id,
				const server::Request& searchRequest
			) const;

			/// Column name for object2_id
			virtual std::string getObject2ColumnName() const;

			/// Converts registry key (object2_id) into object name
			virtual std::string getObject2Name(
				util::RegistryKeyType id,
				const server::Request& searchRequest
			) const;

			/**
				Add an entry for a notification event. Current status and type
				are used, references to alarm and provider are available.

				@param event shared pointer to processed NotificationEvent to log
			*/
			static void AddNotificationEventEntry(
				const boost::shared_ptr<NotificationEvent>& event
			);

			/**
				Add an entry for a technical notification provider failure
			*/
			static void AddNotificationProviderFailure(
				const NotificationProvider* provider,
				const std::string details,
				const Alarm* alarm = NULL
			);

		private:

		};

	} /* namespace messages */
} /* namespace synthese */

#endif /* SYNTHESE_messages_NotificationLog_hpp__ */
