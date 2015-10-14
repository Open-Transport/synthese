
//////////////////////////////////////////////////////////////////////////
///	CityAliasTableSync class implementation.
///	@file CityAliasTableSync.cpp
///	@author Hugues Romain
///	@date 2010
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

#include <sstream>

#include "CityAliasTableSync.hpp"
#include "ReplaceQuery.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "CityTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,CityAliasTableSync>::FACTORY_KEY("32.40 City aliases");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CityAliasTableSync>::TABLE(
			"t065_city_aliases"
		);

		template<> const Field DBTableSyncTemplate<CityAliasTableSync>::_FIELDS[]=
		{	Field()
		};


		template<>
		DBTableSync::Indexes DBTableSyncTemplate<CityAliasTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					Code::FIELD.name.c_str(),
			"")	);
			r.push_back(
				DBTableSync::Index(
					AliasedCity::FIELD.name.c_str(),
					Name::FIELD.name.c_str(),
			"")	);
			return r;
		}



		template<> bool DBTableSyncTemplate<CityAliasTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<CityAliasTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CityAliasTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CityAliasTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace geography
	{
		CityAliasTableSync::SearchResult CityAliasTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> aliasedCityFilter,
			boost::optional<std::string> codeFilter,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 "
				;
			if(aliasedCityFilter)
			{
				query << " AND " << AliasedCity::FIELD.name << "=" << *aliasedCityFilter;
			}
			if(codeFilter)
			{
				query << " AND " << Code::FIELD.name << "=" << *codeFilter;
			}
			if(orderByName)
			{
				query << " ORDER BY " << Name::FIELD.name << " " << (raisingOrder ? "ASC" : "DESC");
			}
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
				{
					query << " OFFSET " << first;
			}	}

			return LoadFromQuery(query.str(), env, linkLevel);
		}


		bool CityAliasTableSync::allowList(const server::Session* session) const
		{
			return true;
		}

	}
}
