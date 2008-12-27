
/** FareTableSync class implementation.
	@file FareTableSync.cpp

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

#include "Conversion.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "FareTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	template<> const string util::FactorableTemplate<SQLiteTableSync, env::FareTableSync>::FACTORY_KEY("15.10.02 Fares");

	namespace env
	{
		const std::string FareTableSync::COL_NAME ("name");
		const std::string FareTableSync::COL_FARETYPE ("fare_type");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<FareTableSync>::TABLE(
			FareTableSync::CreateFormat(
				"t008_fares",
				SQLiteTableFormat::CreateFields(
					SQLiteTableSync::Field(FareTableSync::COL_NAME, SQL_TEXT),
					SQLiteTableSync::Field(FareTableSync::COL_FARETYPE, SQL_INTEGER),
					SQLiteTableSync::Field()
				), SQLiteTableFormat::Indexes()
		)	);

		template<> void SQLiteDirectTableSyncTemplate<FareTableSync,Fare>::Load(
			Fare* fare,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel		
		){
			fare->setName (rows->getText (FareTableSync::COL_NAME));
			fare->setType (static_cast<Fare::FareType>(rows->getInt (FareTableSync::COL_FARETYPE)));
		}

		template<> void SQLiteDirectTableSyncTemplate<FareTableSync,Fare>::Save(Fare* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE.NAME << " SET "
					/// @todo fill fields [,]FIELD=VALUE
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId());
                query
					<< " INSERT INTO " << TABLE.NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					/// @todo fill other fields separated by ,
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}


		template<> void SQLiteDirectTableSyncTemplate<FareTableSync,Fare>::Unlink(Fare* obj)
		{

		}

	}

	namespace env
	{
		FareTableSync::FareTableSync()
			: SQLiteRegistryTableSyncTemplate<FareTableSync,Fare>()
		{
		}

		void FareTableSync::Search(
			Env& env,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
