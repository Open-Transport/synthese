////////////////////////////////////////////////////////////////////////////////
///	DataSource table synchronizer class implementation.
///	@file DataSourceTableSync.cpp
///	@author Hugues Romain
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DataSourceTableSync.h"
#include "SelectQuery.hpp"
#include "ReplaceQuery.h"
#include "CoordinatesSystem.hpp"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace impex;

	template<> const string util::FactorableTemplate<DBTableSync,DataSourceTableSync>::FACTORY_KEY(
		"16.01 DataSource"
	);

	namespace impex
	{
		const string DataSourceTableSync::COL_NAME("name");
		const string DataSourceTableSync::COL_FORMAT("format");
		const string DataSourceTableSync::COL_ICON("icon");
		const string DataSourceTableSync::COL_CHARSET("charset");
		const string DataSourceTableSync::COL_SRID("srid");
		const string DataSourceTableSync::COL_DEFAULT_IMPORT_REQUEST = "default_import_request";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DataSourceTableSync>::TABLE(
			"t059_data_sources"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<DataSourceTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(DataSourceTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(DataSourceTableSync::COL_FORMAT, SQL_TEXT),
			DBTableSync::Field(DataSourceTableSync::COL_ICON, SQL_TEXT),
			DBTableSync::Field(DataSourceTableSync::COL_CHARSET, SQL_TEXT),
			DBTableSync::Field(DataSourceTableSync::COL_SRID, SQL_INTEGER),
			DBTableSync::Field(DataSourceTableSync::COL_DEFAULT_IMPORT_REQUEST, SQL_TEXT),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<DataSourceTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<DataSourceTableSync,DataSource>::Load(
			DataSource* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Name
			string name (rows->getText(DataSourceTableSync::COL_NAME));
			object->setName(name);

			// Format
			string format (rows->getText(DataSourceTableSync::COL_FORMAT));
			object->setFormat(format);

			// Icon
			object->setIcon(rows->getText(DataSourceTableSync::COL_ICON));

			// Charset
			object->setCharset(rows->getText(DataSourceTableSync::COL_CHARSET));

			// CoordinatesSystem
			CoordinatesSystem::SRID srid(rows->getInt(DataSourceTableSync::COL_SRID));
			if(srid > 0) try
			{
				object->setCoordinatesSystem(&CoordinatesSystem::GetCoordinatesSystem(srid));
			}
			catch(CoordinatesSystem::CoordinatesSystemNotFoundException& e)
			{
				Log::GetInstance().error("Bad SRID in record"+ lexical_cast<string>(object->getKey()), e);
			}
			else
			{
				object->setCoordinatesSystem(NULL);
			}

			// Default import request
			object->setDefaultImportRequest(rows->getText(DataSourceTableSync::COL_DEFAULT_IMPORT_REQUEST));
		}



		template<> void DBDirectTableSyncTemplate<DataSourceTableSync,DataSource>::Save(
			DataSource* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<DataSourceTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getFormat());
			query.addField(object->getIcon());
			query.addField(object->getCharset());
			query.addField(object->getCoordinatesSystem() ? object->getCoordinatesSystem()->getSRID() : CoordinatesSystem::SRID(0));
			query.addField(object->getDefaultImportRequest());
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<DataSourceTableSync,DataSource>::Unlink(
			DataSource* object
		){
			object->setCoordinatesSystem(NULL);
		}



		template<> bool DBTableSyncTemplate<DataSourceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the user has sufficient right level
			return true;
		}



		template<> void DBTableSyncTemplate<DataSourceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DataSourceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DataSourceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace impex
	{
		DataSourceTableSync::DataSourceTableSync ()
		:	DBRegistryTableSyncTemplate<DataSourceTableSync,DataSource>()
		{
		}



		DataSourceTableSync::SearchResult DataSourceTableSync::Search(
			Env& env,
			string name,
			optional<string> format,
			int first /*= 0*/
			, optional<size_t> number /*= 0*/
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<DataSourceTableSync> query;
			if (!name.empty())
			{
				query.addWhereField(COL_NAME, "%"+ lexical_cast<string>(name) + "%", ComposedExpression::OP_LIKE);
			}
			if(format)
			{
				query.addWhereField(COL_FORMAT, *format);
			}
			if (orderByName)
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
	}
}
