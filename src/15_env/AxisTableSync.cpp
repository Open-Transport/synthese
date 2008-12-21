
/** AxisTableSync class implementation.
	@file AxisTableSync.cpp

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

#include "Axis.h"
#include "AxisTableSync.h"
#include "EnvModule.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,AxisTableSync>::FACTORY_KEY("15.20.03 Axes");
	}

	namespace env
	{
		const std::string AxisTableSync::COL_NAME ("name");
		const std::string AxisTableSync::COL_FREE ("free");
		const std::string AxisTableSync::COL_ALLOWED ("allowed");
	}

	namespace db
	{
		template<> const SQLiteTableFormat SQLiteTableSyncTemplate<AxisTableSync>::TABLE(
			AxisTableSync::CreateFormat(
				"t004_axes",
				SQLiteTableFormat::CreateFields(
					SQLiteTableFormat::Field(AxisTableSync::COL_NAME, TEXT),
					SQLiteTableFormat::Field(AxisTableSync::COL_FREE, BOOLEAN),
					SQLiteTableFormat::Field(AxisTableSync::COL_ALLOWED, BOOLEAN),
					SQLiteTableFormat::Field()
				), SQLiteTableFormat::Indexes()
		)	);

		template<> void SQLiteDirectTableSyncTemplate<AxisTableSync,Axis>::Load(
			Axis* axis,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
		    axis->setName (rows->getText (AxisTableSync::COL_NAME));
		    axis->setFree (rows->getBool (AxisTableSync::COL_FREE));
		    axis->setAllowed (rows->getBool (AxisTableSync::COL_ALLOWED));
		}


		template<> void SQLiteDirectTableSyncTemplate<AxisTableSync,Axis>::Unlink(Axis* obj, Env* env)
		{
		}


		template<> void SQLiteDirectTableSyncTemplate<AxisTableSync,Axis>::Save(Axis* object)
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

	}

	namespace env
	{
		AxisTableSync::AxisTableSync()
			: SQLiteRegistryTableSyncTemplate<AxisTableSync,Axis>()
		{
		}

		void AxisTableSync::Search(
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

