
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
#include "GlobalRight.h"
#include "ImportableTableSync.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "User.h"

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

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DepotTableSync>::TABLE(
			"t073_depots"
		);



		template<> const Field DBTableSyncTemplate<DepotTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DepotTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					SimpleObjectFieldDefinition<Name>::FIELD.name.c_str(),
			"")	);
			return r;
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
			 	query.addWhereField(SimpleObjectFieldDefinition<Name>::FIELD.name, *name);
			}
			if(orderByName)
			{
				query.addOrderField(SimpleObjectFieldDefinition<Name>::FIELD.name, raisingOrder);
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

		bool DepotTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::READ);
		}
	}
}
