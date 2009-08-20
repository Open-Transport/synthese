////////////////////////////////////////////////////////////////////////////////
///	DataSource table synchronizer class implementation.
///	@file DataSourceTableSync.cpp
///	@author Hugues Romain
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DataSourceTableSync.h"

#include "SQLiteResult.h"
#include "SQLite.h"

#include "Conversion.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace impex;

	template<> const string util::FactorableTemplate<SQLiteTableSync,DataSourceTableSync>::FACTORY_KEY(
		"16.01 DataSource"
	);

	namespace impex
	{
		const string DataSourceTableSync::COL_NAME("name");
		const string DataSourceTableSync::COL_FORMAT("format");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<DataSourceTableSync>::TABLE(
			"t059_data_sources"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<DataSourceTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(DataSourceTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(DataSourceTableSync::COL_FORMAT, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<DataSourceTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<DataSourceTableSync,DataSource>::Load(
			DataSource* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			string name (rows->getText(DataSourceTableSync::COL_NAME));
			string format (rows->getText(DataSourceTableSync::COL_FORMAT));

			object->setName(name);
			object->setFormat(format);
		}

		template<> void SQLiteDirectTableSyncTemplate<DataSourceTableSync,DataSource>::Save(
			DataSource* object
		){
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());

			query
				<< "REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getName())
				<< "," << Conversion::ToSQLiteString(object->getFormat())
				<< ")";
			
			DBModule::GetSQLite()->execUpdate(query.str());
		}

		template<> void SQLiteDirectTableSyncTemplate<DataSourceTableSync,DataSource>::Unlink(
			DataSource* object
		){
		}
	}

	namespace impex
	{
		DataSourceTableSync::DataSourceTableSync ()
		:	SQLiteNoSyncTableSyncTemplate<DataSourceTableSync,DataSource>()
		{
		}


	    
		DataSourceTableSync::SearchResult DataSourceTableSync::Search(
			Env& env,
			string name
			, int first /*= 0*/
			, optional<size_t> number /*= 0*/
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
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
 
