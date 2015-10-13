
/** UserTableSync class header.
	@file UserTableSync.h

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

#ifndef SYNTHESE_UserTableSync_H__
#define SYNTHESE_UserTableSync_H__

#include <boost/logic/tribool.hpp>
#include <boost/shared_ptr.hpp>

#include "User.h"
#include "DBDirectTableSyncTemplate.hpp"
#include "NoSynchronizationPolicy.hpp"

namespace synthese
{
	namespace security
	{
		/** User table synchronizer.
			@ingroup m12LS refLS
			@todo Update the opened session on user update
		*/

		class UserTableSync:
			public db::DBDirectTableSyncTemplate<
				UserTableSync,
				User,
				db::NoSynchronizationPolicy
			>
		{
		public:
			/** Gets a user in the database, founded by his login.
				@param login login to search
				@return boost::shared_ptr<User> Shared pointer to a new user linked-object.
				@author Hugues Romain
				@date 2007
			*/
			static boost::shared_ptr<User> getUserFromLogin(const std::string& login);



			/** Gets a user in the database, founded by his mail address.
				@param mail mail to search
				@return boost::shared_ptr<User> Shared pointer to a new user linked-object.
				@date 2013
			*/
			static boost::shared_ptr<User> getUserFromMail(const std::string& mail);



			static bool loginExists(const std::string& login);

			/** User search.
				@param login login to search : use LIKE syntax
				@param name name to search : use LIKE syntax
				@param surname name to search : use LIKE syntax
				@param phone phone to search (LIKE syntax)
				@param profileId Profile ID which user must belong
				@param emptyLogin User without login acceptation (true = user must have a login, false = user must not have a login, indeterminate = filter deactivated)
				@param first First user to answer
				@param number Number of users to answer (-1 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others users to show. Test it to know if the situation needs a "click for more" button.
				@param orderByLogin Order the results by login
				@param orderByName Order the results by name and surname
				@param orderByProfile Order the results by profile name
				@param raisingOrder True = Ascendant order, false = descendant order
				@return Found vector of shared pointers to User linked-objects.
				@throw UserTableSyncException If the query has failed (does not occurs in normal case)
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<std::string> login = boost::optional<std::string>(),
				boost::optional<std::string> name = boost::optional<std::string>(),
				boost::optional<std::string> surname = boost::optional<std::string>(),
				boost::optional<std::string> phone = boost::optional<std::string>(),
				boost::optional<util::RegistryKeyType> profileId = boost::optional<util::RegistryKeyType>(),
				boost::logic::tribool emptyLogin = boost::logic::indeterminate,
				boost::logic::tribool emptyPhone = boost::logic::indeterminate,
				boost::optional<util::RegistryKeyType> differentUserId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByLogin = true,
				bool orderByName = false,
				bool orderByProfileName = false,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};

	}
}
#endif // SYNTHESE_UserTableSync_H__

