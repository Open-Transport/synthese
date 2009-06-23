
/** CityTableSync class implementation.
	@file CityTableSync.cpp

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

#include "CityTableSync.h"
#include "Conversion.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "GeographyModule.h"

#include <sqlite3.h>
#include <assert.h>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace geography;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,CityTableSync>::FACTORY_KEY("14.00.01 Cities");
	}

	namespace geography
	{
		const string CityTableSync::TABLE_COL_NAME("name");
		const string CityTableSync::TABLE_COL_CODE("code");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<CityTableSync>::TABLE(
			"t006_cities"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<CityTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(CityTableSync::TABLE_COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(CityTableSync::TABLE_COL_CODE, SQL_TEXT),
			SQLiteTableSync::Field()

		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<CityTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(CityTableSync::TABLE_COL_NAME.c_str(), ""),
			SQLiteTableSync::Index(CityTableSync::TABLE_COL_CODE.c_str(), ""),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<CityTableSync,City>::Load(
			City* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    object->setName(rows->getText ( CityTableSync::TABLE_COL_NAME));
		    object->setCode(rows->getText ( CityTableSync::TABLE_COL_CODE));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				GeographyModule::AddToCitiesMatchers(object);
			}
		}


		template<> void SQLiteDirectTableSyncTemplate<CityTableSync,City>::Unlink(City* obj)
		{
			GeographyModule::RemoveFromCitiesMatchers(obj);
		}

		template<> void SQLiteDirectTableSyncTemplate<CityTableSync,City>::Save(City* object)
		{
			/// @todo Implement it
		}
	}

	namespace geography
	{
		CityTableSync::CityTableSync ()
			: SQLiteAutoRegisterTableSyncTemplate<CityTableSync,City> ()
		{
		}


		CityTableSync::~CityTableSync ()
		{

		}

		void CityTableSync::Search(
			util::Env& env,
			boost::optional<std::string> exactName /*= boost::optional<std::string>()*/,
			boost::optional<std::string> likeName /*= boost::optional<std::string>()*/,
			boost::optional<std::string> code /*= boost::optional<std::string>()*/,
			int first /*= 0*/,
			int number /*= 0*/,
			bool orderByName /*= true*/,
			bool raisingOrder /*= true*/,
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (exactName)
				query << " AND " << TABLE_COL_NAME << "=" << Conversion::ToSQLiteString(*exactName);
			if (likeName)
				query << " AND " << TABLE_COL_NAME << " LIKE " << Conversion::ToSQLiteString(*exactName);
			if (code)
				query << " AND " << TABLE_COL_CODE << "=" << Conversion::ToSQLiteString(*code);
			if (orderByName)
				query << " ORDER BY " << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);

		}

		boost::shared_ptr<City> CityTableSync::GetEditableFromCode(
			const string& code,
			util::Env& environment,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			Env tenv;
			Search(tenv, optional<string>(), optional<string>(), code, 0, 1, false, false, FIELDS_ONLY_LOAD_LEVEL);
			if(tenv.getRegistry<City>().empty()) return shared_ptr<City>();
			shared_ptr<const City> result(tenv.getRegistry<City>().front());
			return GetEditable(result->getKey(), environment, linkLevel);
		}
	}
}
