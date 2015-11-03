
/** UserFavoriteJourneyTableSync class implementation.
	@file UserFavoriteJourneyTableSync.cpp
	@author Hugues Romain
	@date 2007

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

#include "UserFavoriteJourneyTableSync.h"

#include "Profile.h"
#include "ReplaceQuery.h"
#include "Session.h"
#include "TransportWebsiteRight.h"
#include "UserFavoriteJourney.h"
#include "UserTableSync.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt_website;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,UserFavoriteJourneyTableSync>::FACTORY_KEY("53.1 User favorite journey");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<UserFavoriteJourneyTableSync>::TABLE(
			"t048_user_favorite_journey"
		);

		template<> const Field DBTableSyncTemplate<UserFavoriteJourneyTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<UserFavoriteJourneyTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<UserFavoriteJourneyTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<UserFavoriteJourneyTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<UserFavoriteJourneyTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<UserFavoriteJourneyTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log the removal
		}
	}



	namespace pt_website
	{
		UserFavoriteJourneyTableSync::SearchResult UserFavoriteJourneyTableSync::Search(
			Env& env,
			const User* user
			, int first /*= 0*/
			, boost::optional<std::size_t> number /*= 0*/
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE "
				<< FavoriteJourneyUser::FIELD.name << "=" << user->getKey()
				<< " ORDER BY " << Rank::FIELD.name << (raisingOrder ? " ASC" : " DESC");
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}

		bool UserFavoriteJourneyTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(security::READ);
		}
	}
}
