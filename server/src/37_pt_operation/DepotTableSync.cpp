
//////////////////////////////////////////////////////////////////////////
///	DepotTableSync class implementation.
///	@file DepotTableSync.cpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "DepotTableSync.hpp"

#include "DataSourceLinksField.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "ImportableTableSync.hpp"

#include <sstream>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt_operation;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DepotTableSync>::FACTORY_KEY("37.15 Depot");
	}

	namespace pt_operation
	{
		const string DepotTableSync::COL_NAME = "name";
		const string DepotTableSync::COL_DATASOURCE_LINKS = "datasource_links";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DepotTableSync>::TABLE(
			"t073_depots"
		);



		template<> const Field DBTableSyncTemplate<DepotTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(DepotTableSync::COL_NAME, SQL_TEXT),
			Field(DepotTableSync::COL_DATASOURCE_LINKS, SQL_TEXT),
			Field(TABLE_COL_GEOMETRY, SQL_GEOM_POINT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DepotTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					DepotTableSync::COL_NAME.c_str(),
			"")	);
			return r;
		}



		template<> void OldLoadSavePolicy<DepotTableSync,Depot>::Load(
			Depot* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(DepotTableSync::COL_NAME));
			string pointsStr(rows->getText(TABLE_COL_GEOMETRY));
			if(pointsStr.empty())
			{
				object->setGeometry(boost::shared_ptr<Point>());
			}
			else
			{
				object->setGeometry(
					dynamic_pointer_cast<Point,Geometry>(rows->getGeometryFromWKT(TABLE_COL_GEOMETRY))
				);
			}

			// Datasource links
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				object->setDataSourceLinksWithRegistration(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						rows->getText(DepotTableSync::COL_DATASOURCE_LINKS),
						env
				)	);
			}
		}



		template<> void OldLoadSavePolicy<DepotTableSync,Depot>::Save(
			Depot* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<DepotTableSync> query(*object);
			query.addField(object->getName());
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);
			if(object->hasGeometry())
			{
				query.addField(static_pointer_cast<Geometry,Point>(object->getGeometry()));
			}
			else
			{
				query.addFieldNull();
			}
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<DepotTableSync,Depot>::Unlink(
			Depot* obj
		){
		}



		template<> bool DBTableSyncTemplate<DepotTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<DepotTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DepotTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DepotTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}



	namespace pt_operation
	{
		DepotTableSync::SearchResult DepotTableSync::Search(
			util::Env& env,
			boost::optional<string> name /*= boost::optional<util::RegistryKeyType>()*/,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<DepotTableSync> query;
			if(name)
			{
			 	query.addWhereField(COL_NAME, *name);
			}
			if(orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}



		DepotTableSync::DepotsList DepotTableSync::GetDepotsList( util::Env& env, boost::optional<std::string> noDepotLabel )
		{
			DepotsList result;
			if(noDepotLabel)
			{
				result.push_back(make_pair(optional<RegistryKeyType>(0), *noDepotLabel));
			}
			SearchResult depots(Search(env));
			BOOST_FOREACH(const boost::shared_ptr<Depot>& depot, depots)
			{
				result.push_back(make_pair(optional<RegistryKeyType>(depot->getKey()), depot->getName()));
			}
			return result;
		}
	}
}
