
/** DBLogEntryTableSync class header.
	@file DBLogEntryTableSync.h

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

#ifndef SYNTHESE_DBLogEntryTableSync_H__
#define SYNTHESE_DBLogEntryTableSync_H__

#include "DBLogEntry.h"
#include "DBDirectTableSyncTemplate.hpp"
#include "NoSynchronizationPolicy.hpp"

namespace synthese
{
	namespace dblog
	{
		class DBLog;

		//////////////////////////////////////////////////////////////////////////
		/// 13.10 Table : Database stored log entries.
		///	@ingroup m13LS refLS
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Table name : t045_log_entries
		class DBLogEntryTableSync:
			public db::DBDirectTableSyncTemplate<
				DBLogEntryTableSync,
				DBLogEntry,
				db::NoSynchronizationPolicy
			>
		{
		public:
			/** DBLog search.
				@param logKey key of the log (LIKE format)
				@param startDate start date
				@param endDate end date
				@param userId ID of user
				@param level level of the entry
				@param id ID of the object
				@param id2 ID of the second object
				@param text text in the content (LIKE format)
				@param first First DBLog object to answer
				@param number Number of DBLog objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Vector of shared pointers to found DBLog linked-objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				const std::string& logKey,
				const boost::posix_time::ptime& startDate,
				const boost::posix_time::ptime& endDate,
				boost::optional<util::RegistryKeyType> userId,
				Level level,
				boost::optional<util::RegistryKeyType> id,
				boost::optional<util::RegistryKeyType> id2,
				const std::string& text,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByDate = true,
				bool orderByUser = false,
				bool orderByLevel = false,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			/** DBLog search by user in object_id column
			 * @param objectId ID of user
			 * @return Vector of shared pointers to found DBLog linked-objects.
			 * @date 2013
			 */
			static SearchResult SearchByUser(
				util::Env& env,
				util::RegistryKeyType objectId,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			static void Purge(
				const std::string& logKey,
				const boost::posix_time::ptime& endDate
			);

			virtual bool allowList( const server::Session* session ) const;
		};
	}
}

#endif // SYNTHESE_DBLogEntryTableSync_H__
