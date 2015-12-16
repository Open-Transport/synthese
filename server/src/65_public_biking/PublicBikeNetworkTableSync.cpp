
/** PublicBikeNetworkTableSync class implementation.
	@file PublicBikeNetworkTableSync.cpp

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

#include "PublicBikeNetworkTableSync.hpp"

#include "Profile.h"
#include "Session.h"
#include "User.h"

using namespace boost;
using namespace std;

namespace synthese
{
	template<> const string util::FactorableTemplate<db::DBTableSync, public_biking::PublicBikeNetworkTableSync>::FACTORY_KEY(
		"65.10.01 Public bike network"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<public_biking::PublicBikeNetworkTableSync>::TABLE(
			"t125_public_bike_networks"
		);



		template<> const Field DBTableSyncTemplate<public_biking::PublicBikeNetworkTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<public_biking::PublicBikeNetworkTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(SimpleObjectFieldDefinition<Name>::FIELD.name.c_str(), ""));
			return r;
		}



		template<> bool DBTableSyncTemplate<public_biking::PublicBikeNetworkTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<public_biking::PublicBikeNetworkTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<public_biking::PublicBikeNetworkTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<public_biking::PublicBikeNetworkTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace public_biking
	{
		bool PublicBikeNetworkTableSync::allowList(const server::Session* session) const
		{
			return true;
		}

		PublicBikeNetworkTableSync::SearchResult PublicBikeNetworkTableSync::Search(
			util::Env& env,
			util::LinkLevel linkLevel
		){
			db::SelectQuery<PublicBikeNetworkTableSync> query;

			return LoadFromQuery(query, env, linkLevel);
        }
}	}
