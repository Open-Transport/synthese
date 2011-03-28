
//////////////////////////////////////////////////////////////////////////
///	CityAliasTableSync class implementation.
///	@file CityAliasTableSync.cpp
///	@author Hugues
///	@date 2010
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

#include "CityAliasTableSync.hpp"
#include "ReplaceQuery.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLiteException.h"
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
		template<> const string FactorableTemplate<SQLiteTableSync,CityAliasTableSync>::FACTORY_KEY("32.40 City aliases");
	}

	namespace geography
	{
		const string CityAliasTableSync::COL_ALIASED_CITY_ID("aliased_city_id");
		const string CityAliasTableSync::COL_NAME ("name");
		const string CityAliasTableSync::COL_CODE ("code");
		const string CityAliasTableSync::COL_VISIBLE ("visible");
	}
	
	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<CityAliasTableSync>::TABLE(
			"t065_city_aliases"
		);



		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<CityAliasTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(CityAliasTableSync::COL_ALIASED_CITY_ID, SQL_INTEGER),
			SQLiteTableSync::Field(CityAliasTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(CityAliasTableSync::COL_CODE, SQL_TEXT),
			SQLiteTableSync::Field(CityAliasTableSync::COL_VISIBLE, SQL_INTEGER),
			SQLiteTableSync::Field()
		};



		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<CityAliasTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(
				CityAliasTableSync::COL_CODE.c_str(),
			""),
			SQLiteTableSync::Index(
				CityAliasTableSync::COL_ALIASED_CITY_ID.c_str(),
				CityAliasTableSync::COL_NAME.c_str(),
			""),
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<CityAliasTableSync,CityAlias>::Load(
			CityAlias* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(CityAliasTableSync::COL_NAME));
			object->setCode(rows->getText(CityAliasTableSync::COL_CODE));
			object->setVisible(rows->getBool(CityAliasTableSync::COL_VISIBLE));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType pid(rows->getLongLong(CityAliasTableSync::COL_ALIASED_CITY_ID));
				if(pid > 0)
				{
					try
					{
						object->setCity(CityTableSync::GetEditable(pid, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<City>& e)
					{
						Log::GetInstance().warn("No such city "+ lexical_cast<string>(pid) +" in CityAlias "+ lexical_cast<string>(object->getKey()));
					}
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<CityAliasTableSync,CityAlias>::Save(
			CityAlias* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<CityAliasTableSync> query(*object);
			query.addField(object->getCity() ? object->getCity()->getKey() : RegistryKeyType(0));
			query.addField(object->getName());
			query.addField(object->getCode());
			query.addField(object->getVisible());
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<CityAliasTableSync,CityAlias>::Unlink(
			CityAlias* obj
		){
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
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 "
				;
			if(aliasedCityFilter)
			{
			 	query << " AND " << COL_ALIASED_CITY_ID << "=" << *aliasedCityFilter;
			}
			if(codeFilter)
			{
				query << " AND " << COL_CODE << "=" << *codeFilter;
			}
			if(orderByName)
			{
			 	query << " ORDER BY " << COL_NAME << " " << (raisingOrder ? "ASC" : "DESC");
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
	}
}
