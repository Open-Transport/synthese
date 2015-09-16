
/** ObjectSiteLinkTableSync class implementation.
	@file ObjectSiteLinkTableSync.cpp
	@author Hugues Romain
	@date 2008

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

#include "ObjectSiteLinkTableSync.h"

#include "CityTableSync.h"
#include "Profile.h"
#include "PTServiceConfigTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "TransportWebsiteRight.h"
#include "User.h"
#include "UtilTypes.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt_website;
	using namespace geography;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,ObjectSiteLinkTableSync>::FACTORY_KEY("56.11 Object Website Links");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ObjectSiteLinkTableSync>::TABLE(
			"t001_object_site_links"
		);


		template<> const Field DBTableSyncTemplate<ObjectSiteLinkTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ObjectSiteLinkTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(Site::FIELD.name.c_str(), ""));
			r.push_back(DBTableSync::Index(ObjectId::FIELD.name.c_str(), ""));
			return r;
		}



		template<> bool DBTableSyncTemplate<ObjectSiteLinkTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<ObjectSiteLinkTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ObjectSiteLinkTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ObjectSiteLinkTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log the removal
		}
	}



	namespace pt_website
	{
		ObjectSiteLinkTableSync::SearchResult ObjectSiteLinkTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> siteId, // = boost::optional<util::RegistryKeyType>(),
			boost::optional<util::RegistryKeyType> objectId, // = boost::optional<util::RegistryKeyType>(),
			boost::optional<int> objectTableId, // = boost::optional<int>(),
			int first, /*= 0*/
			boost::optional<std::size_t> number, /*= 0*/
			LinkLevel linkLevel
		){
			SelectQuery<ObjectSiteLinkTableSync> query;
			if (siteId)
			{
				query.addWhereField(Site::FIELD.name, *siteId);
			}
			if(objectId)
			{
				query.addWhereField(ObjectId::FIELD.name, *objectId);
			}
			if(objectTableId)
			{
				query.addWhereField(ObjectId::FIELD.name + " & " + lexical_cast<string>(0xFFFF000000000000LL), util::encodeUId(*objectTableId, 0, 0));
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
