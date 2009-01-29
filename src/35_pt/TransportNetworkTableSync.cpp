
/** TransportNetworkTableSync class implementation.
	@file TransportNetworkTableSync.cpp

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

#include "TransportNetworkTableSync.h"

#include "SQLiteResult.h"
#include "SQLite.h"

#include "Conversion.h"

#include <boost/logic/tribool.hpp>
#include <assert.h>

using boost::logic::tribool;
using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;

	template<> const string util::FactorableTemplate<SQLiteTableSync,TransportNetworkTableSync>::FACTORY_KEY(
		"15.20.02 Network transport"
	);

	namespace pt
	{
		const string TransportNetworkTableSync::COL_NAME("name");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<TransportNetworkTableSync>::TABLE(
			"t022_transport_networks"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<TransportNetworkTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(TransportNetworkTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<TransportNetworkTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::Load(
			TransportNetwork* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			std::string name (rows->getText (TransportNetworkTableSync::COL_NAME));

			object->setName(name);
		}

		template<> void SQLiteDirectTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::Save(TransportNetwork* object)
		{
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());

			query
				<< "REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getName())
				<< ")";
			
			DBModule::GetSQLite()->execUpdate(query.str());
		}

		template<> void SQLiteDirectTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::Unlink(
			TransportNetwork* object
		){
		}
	}

	namespace pt
	{
		TransportNetworkTableSync::TransportNetworkTableSync ()
		: SQLiteRegistryTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>()
		{
		}


	    
	    void TransportNetworkTableSync::Search(
			Env& env,
			string name
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (!name.empty())
				query << " AND " << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
