
/** CityTableSync class implementation.
	@file CityTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
#include "GeographyModule.h"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "User.h"

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
		template<> const string FactorableTemplate<DBTableSync,CityTableSync>::FACTORY_KEY("32.00.01 Cities");
	}

	namespace geography
	{
		const string CityTableSync::TABLE_COL_NAME("name");
		const string CityTableSync::TABLE_COL_CODE("code");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CityTableSync>::TABLE(
			"t006_cities"
		);

		template<> const Field DBTableSyncTemplate<CityTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(CityTableSync::TABLE_COL_NAME, SQL_TEXT),
			Field(CityTableSync::TABLE_COL_CODE, SQL_TEXT),
			Field()

		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<CityTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(CityTableSync::TABLE_COL_NAME.c_str(), ""));
			r.push_back(DBTableSync::Index(CityTableSync::TABLE_COL_CODE.c_str(), ""));
			return r;
		}



		template<> bool DBTableSyncTemplate<CityTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<CityTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CityTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CityTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace geography
	{
		CityTableSync::SearchResult CityTableSync::Search(
			util::Env& env,
			boost::optional<std::string> exactName /*= boost::optional<std::string>()*/,
			boost::optional<std::string> likeName /*= boost::optional<std::string>()*/,
			boost::optional<std::string> code /*= boost::optional<std::string>()*/,
			int first /*= 0*/,
			int number /*= 0*/,
			bool orderByName /*= true*/,
			bool raisingOrder /*= true*/,
			util::LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (exactName)
				query << " AND " << TABLE_COL_NAME << "=" << Conversion::ToDBString(*exactName);
			if (likeName)
				query << " AND " << TABLE_COL_NAME << " LIKE " << Conversion::ToDBString(*likeName);
			if (code)
				query << " AND " << TABLE_COL_CODE << "=" << Conversion::ToDBString(*code);
			if (orderByName)
				query << " ORDER BY " << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << (number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);

		}

		boost::shared_ptr<City> CityTableSync::GetEditableFromCode(
			const string& code,
			util::Env& environment,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			Env tenv;
			SearchResult cities(
				Search(
					tenv,
					optional<string>(),
					optional<string>(),
					code,
					0, 1, false, false,
					FIELDS_ONLY_LOAD_LEVEL
			)	);
			if(cities.empty()) return boost::shared_ptr<City>();
			boost::shared_ptr<const City> result(cities.front());
			return GetEditable(result->getKey(), environment, linkLevel);
		}

		bool CityTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::READ);
		}
	}
}
