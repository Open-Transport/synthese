
/** AlarmObjectLinkTableSync class header.
	@file AlarmObjectLinkTableSync.h

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

#ifndef SYNTHESE_AlarmObjectLinkTableSync_H__
#define SYNTHESE_AlarmObjectLinkTableSync_H__


#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/optional/optional_io.hpp>

#include "Conversion.h"
#include "Registrable.h"

#include "DBModule.h"
#include "DBDirectTableSyncTemplate.hpp"

#include "AlarmObjectLink.h"
#include "MessagesModule.h"

namespace synthese
{
	namespace messages
	{
		/** Alarm object links table synchronizer.
			@ingroup m17LS refLS

			Only the links concerning the sent alarms are loaded in ram.

			Load method behavior per load level :
			 - less than FIELDS_ONLY_LOAD_LEVEL : not usable (assertion)
			 - FIELDS_ONLY_LOAD_LEVEL and above : links the object to its alarm
			 - RECURSIVE_LINKS_LOAD_LEVEL and above : links its alarm to the corresponding object according to the rules defined in the recipient class
			   only if the alarm is a Alarm object

			Unlink method behavior :
			 - removes the link between the alarm and the object if the alarm is a Alarm object (no exception is thrown if it does not exists)
		*/
		class AlarmObjectLinkTableSync:
			public db::DBDirectTableSyncTemplate<
				AlarmObjectLinkTableSync,
				AlarmObjectLink,
				db::FullSynchronizationPolicy
			>
		{
		public:
			/** Search of alarm object links for a specified alarm in a specified recipient type.
				@param alarm Alarm to the object must be liked with
				@param recipientKey Key of the recipient to search (REMOVE THIS PARAMETER WHEN THE FACTORY KEY WILL BE STATIC)
				@param first First	object to answer
				@param number Number of	 objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<*> Founded  objects.
				@author Hugues Romain
				@date 2006
			*/
			template<class K, class T>
			static std::vector<boost::shared_ptr<T> > search(
				util::Env& env,
				util::RegistryKeyType alarmId,
				const std::string& recipientKey,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

			/** Search of alarm object links for a specified alarm in all the recipient types.
				@param env Environment to populate
				@param alarm Alarm to the object must be liked with
				@param first First	object to answer
				@param number Number of	 objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				util::RegistryKeyType alarmId,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			/** Remove a link between an alarm and an object specified by their id.
				@param alarmId ID of the alarm
				@param objectId ID of the object. Undefined = Removes all links of the alarm
				@author Hugues Romain
				@date 2007
			*/
			static void RemoveByMessage(
				util::RegistryKeyType alarmId,
				boost::optional<util::RegistryKeyType> objectId = boost::optional<util::RegistryKeyType>(),
				boost::optional<db::DBTransaction&> transaction = boost::optional<db::DBTransaction&>()
			);



			static void RemoveByTarget(
				util::RegistryKeyType objectId,
				boost::optional<db::DBTransaction&> transaction
			);


			//////////////////////////////////////////////////////////////////////////
			/// Performs a copy of each recipient of a message into an other one.
			/// @param sourceId the id of the message to copy
			/// @param destAlarm the new message message
			/// @param transaction the transaction
			static void CopyRecipients(
				util::RegistryKeyType sourceId,
				Alarm& destAlarm,
				boost::optional<db::DBTransaction&> transaction
			);

			virtual bool allowList( const server::Session* session ) const;
		};



		template<class K, class T>
		std::vector< boost::shared_ptr<T> > AlarmObjectLinkTableSync::search(
			util::Env& env,
			util::RegistryKeyType alarmId,
			const std::string& recipientKey,
			int first /*= 0*/,
			boost::optional<std::size_t> number	 /*= 0*/,
			util::LinkLevel linkLevel
		){
			std::stringstream query;
			query
				<< " SELECT "
					<< ObjectId::FIELD.name
				<< " FROM " << TABLE.NAME
				<< " WHERE "
					<< LinkedAlarm::FIELD.name << "=" << alarmId
					<< " AND " << RecipientKey::FIELD.name << "=" << util::Conversion::ToDBString(recipientKey);
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			db::DBResultSPtr rows = db::DBModule::GetDB()->execQuery(query.str());
			std::vector< boost::shared_ptr<T> > objects;
			while (rows->next ())
			{
				try
				{
					objects.push_back(
						K::GetEditable(
							rows->getLongLong (ObjectId::FIELD.name),
							env,
							linkLevel
					)	);
				}
				catch(Exception& e)
				{
					util::Log::GetInstance().warn(e.getMessage(), e);
				}
			}
			return objects;
		}

	}
}

#endif // SYNTHESE_AlarmObjectLinkTableSync_H__
