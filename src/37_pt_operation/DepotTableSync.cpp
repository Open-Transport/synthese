
//////////////////////////////////////////////////////////////////////////
///	DepotTableSync class implementation.
///	@file DepotTableSync.cpp
///	@author RCSobility
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include <sstream>

#include "DepotTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace pt_operation;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DepotTableSync>::FACTORY_KEY("37.15 Depot");
	}

	namespace pt_operation
	{
		const string DepotTableSync::COL_NAME ("name");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DepotTableSync>::TABLE(
			"t073_depots"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<DepotTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(DepotTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(TABLE_COL_GEOMETRY, SQL_GEOM_POINT),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<DepotTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(
				DepotTableSync::COL_NAME.c_str(),
			""),
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<DepotTableSync,Depot>::Load(
			Depot* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(DepotTableSync::COL_NAME));
			string pointsStr(rows->getText(TABLE_COL_GEOMETRY));
			if(pointsStr.empty())
			{
				object->setGeometry(shared_ptr<Point>());
			}
			else
			{
				object->setGeometry(
					dynamic_pointer_cast<Point,Geometry>(rows->getGeometryFromWKT(TABLE_COL_GEOMETRY))
				);
			}
		}



		template<> void DBDirectTableSyncTemplate<DepotTableSync,Depot>::Save(
			Depot* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<DepotTableSync> query(*object);
			query.addField(object->getName());
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



		template<> void DBDirectTableSyncTemplate<DepotTableSync,Depot>::Unlink(
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
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
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
			BOOST_FOREACH(shared_ptr<Depot> depot, depots)
			{
				result.push_back(make_pair(optional<RegistryKeyType>(depot->getKey()), depot->getName()));
			}
			return result;
		}
	}
}
