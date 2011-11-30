/** PublicPlaceTableSync class implementation.
	@file PublicPlaceTableSync.cpp

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

#include <sstream>

#include "PublicPlaceTableSync.h"
#include "PublicPlace.h"
#include "CityTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "ImportableTableSync.hpp"

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace geography;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, PublicPlaceTableSync>::FACTORY_KEY("34.40.03 Public places");
		template<> const string FactorableTemplate<Fetcher<NamedPlace>, PublicPlaceTableSync>::FACTORY_KEY("13");
	}

	namespace road
	{
		const string PublicPlaceTableSync::COL_NAME ("name");
		const string PublicPlaceTableSync::COL_CITYID ("city_id");
		const string PublicPlaceTableSync::COL_DATASOURCE_LINKS = "datasource_links";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<PublicPlaceTableSync>::TABLE(
			"t013_public_places"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<PublicPlaceTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(PublicPlaceTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(PublicPlaceTableSync::COL_CITYID, SQL_INTEGER),
			DBTableSync::Field(PublicPlaceTableSync::COL_DATASOURCE_LINKS, SQL_TEXT),
			DBTableSync::Field(TABLE_COL_GEOMETRY, SQL_GEOM_POINT),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<PublicPlaceTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<PublicPlaceTableSync,PublicPlace>::Load(
			PublicPlace* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Name
			string name (rows->getText (PublicPlaceTableSync::COL_NAME));
			object->setName(name);

			// Geometry
			shared_ptr<Point> point(
				static_pointer_cast<Point, Geometry>(
					rows->getGeometryFromWKT(TABLE_COL_GEOMETRY)
			)	);
			if(point.get())
			{
				object->setGeometry(point);
			}

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				City* city(CityTableSync::GetEditable(rows->getLongLong (PublicPlaceTableSync::COL_CITYID), env, linkLevel).get());
				object->setCity(city);

				city->addPlaceToMatcher<PublicPlace>(env.getEditableSPtr(object));

				// Datasource links
				object->setDataSourceLinks(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						rows->getText(PublicPlaceTableSync::COL_DATASOURCE_LINKS),
						env
				)	);
			}
		}

		template<> void DBDirectTableSyncTemplate<PublicPlaceTableSync,PublicPlace>::Unlink(
			PublicPlace* obj
		){
			City* city(const_cast<City*>(obj->getCity()));
			if (city != NULL)
			{
//				city->removePlaceFromMatcher<PublicPlace>(obj);
				obj->setCity(NULL);
			}
		}


		template<> void DBDirectTableSyncTemplate<PublicPlaceTableSync,PublicPlace>::Save(
			PublicPlace* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<PublicPlaceTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getCity() ? object->getCity()->getKey() : RegistryKeyType(0));
			query.addField(ImportableTableSync::SerializeDataSourceLinks(object->getDataSourceLinks()));
			query.addField(static_pointer_cast<Geometry,Point>(object->getGeometry())); // Must stay at last position
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<PublicPlaceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<PublicPlaceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PublicPlaceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PublicPlaceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace road
	{
		PublicPlaceTableSync::SearchResult PublicPlaceTableSync::Search(
			Env& env,
			int first /*= 0*/,
			boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<PublicPlaceTableSync> query;
			if (number)
				query.setNumber(*number + 1);
			if (first > 0)
				query.setFirst(first);

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
