
/** ObjectSiteLinkTableSync class implementation.
	@file ObjectSiteLinkTableSync.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include <sstream>

#include "ObjectSiteLink.h"
#include "ObjectSiteLinkTableSync.h"
#include "SiteTableSync.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "CityTableSync.h"
#include "ReplaceQuery.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace transportwebsite;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,ObjectSiteLinkTableSync>::FACTORY_KEY("36.11 Object Site Links");
	}

	namespace transportwebsite
	{
		const string ObjectSiteLinkTableSync::COL_OBJECT_ID("object_id");
		const string ObjectSiteLinkTableSync::COL_SITE_ID("site_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<ObjectSiteLinkTableSync>::TABLE(
			"t001_object_site_links"
		);


		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<ObjectSiteLinkTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ObjectSiteLinkTableSync::COL_OBJECT_ID, SQL_INTEGER),
			SQLiteTableSync::Field(ObjectSiteLinkTableSync::COL_SITE_ID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};



		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<ObjectSiteLinkTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(ObjectSiteLinkTableSync::COL_SITE_ID.c_str(), ""),
			SQLiteTableSync::Index(ObjectSiteLinkTableSync::COL_OBJECT_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<ObjectSiteLinkTableSync,ObjectSiteLink>::Load(
			ObjectSiteLink* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setObjectId(rows->getLongLong(ObjectSiteLinkTableSync::COL_OBJECT_ID));
			uid id(rows->getLongLong(ObjectSiteLinkTableSync::COL_SITE_ID));
			try
			{
				object->setSite(SiteTableSync::Get(id, env, linkLevel).get());
			}
			catch(Exception e)
			{
			}
			
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				if(decodeTableId(object->getObjectId()) == CityTableSync::TABLE.ID)
				{
					shared_ptr<Site> site(SiteTableSync::GetEditable(id, env, linkLevel));
					shared_ptr<City> city(CityTableSync::GetEditable(object->getObjectId(), env, linkLevel));
					site->addCity(city.get());
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<ObjectSiteLinkTableSync,ObjectSiteLink>::Save(
			ObjectSiteLink* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<ObjectSiteLinkTableSync> query(*object);
			query.addField(object->getObjectId());
			query.addField(object->getSite() ? object->getSite()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<ObjectSiteLinkTableSync,ObjectSiteLink>::Unlink(
			ObjectSiteLink* obj
		){
		}
	}
	
	
	
	namespace transportwebsite
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
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (siteId)
			{
				query << " AND " << COL_SITE_ID << "=" << *siteId;
			}
			if(objectId)
			{
				query << " AND " << COL_OBJECT_ID << "=" << *objectId;
			}
			if(objectTableId)
			{
				query << " AND " << COL_OBJECT_ID << " & " << 0xFFFF000000000000LL << " = " << util::encodeUId(*objectTableId, 0, 0, 0);
			}
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << (first);
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
