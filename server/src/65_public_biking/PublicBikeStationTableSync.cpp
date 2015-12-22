/** PublicBikeStationTableSync class implementation.
	@file PublicBikeStationTableSync.cpp

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

#include "PublicBikeStationTableSync.hpp"

#include "Profile.h"
#include "Session.h"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	template<> const string util::FactorableTemplate<db::DBTableSync, public_biking::PublicBikeStationTableSync>::FACTORY_KEY("65.10.01 Public bike stations");
	template<> const string util::FactorableTemplate<db::Fetcher<graph::Vertex>, public_biking::PublicBikeStationTableSync>::FACTORY_KEY("124");
	template<> const string util::FactorableTemplate<db::Fetcher<geography::NamedPlace>, public_biking::PublicBikeStationTableSync>::FACTORY_KEY("124");

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<public_biking::PublicBikeStationTableSync>::TABLE(
			"t124_public_bike_stations"
		);


		template<> const Field DBTableSyncTemplate<public_biking::PublicBikeStationTableSync>::_FIELDS[]=
		{
			Field()
		};


		template<>
		DBTableSync::Indexes DBTableSyncTemplate<public_biking::PublicBikeStationTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}


		template<> bool DBTableSyncTemplate<public_biking::PublicBikeStationTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<public_biking::PublicBikeStationTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<public_biking::PublicBikeStationTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<public_biking::PublicBikeStationTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace public_biking
	{

		bool PublicBikeStationTableSync::allowList(const server::Session* session) const
		{
			return true;
		}

		PublicBikeStationTableSync::SearchResult PublicBikeStationTableSync::Search(
			util::Env& env,
			optional<util::RegistryKeyType> networkId,
			util::LinkLevel linkLevel
		){
			db::SelectQuery<PublicBikeStationTableSync> query;
			if(networkId)
			{
				query.addWhereField(PublicBikeNetwork::FIELD.name, *networkId);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
}	}
