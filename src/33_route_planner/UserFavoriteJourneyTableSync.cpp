

/** UserFavoriteJourneyTableSync class implementation.
	@file UserFavoriteJourneyTableSync.cpp
	@author Hugues Romain
	@date 2007

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

#include "UserFavoriteJourneyTableSync.h"
#include "UserFavoriteJourney.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace routeplanner;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,UserFavoriteJourneyTableSync>::FACTORY_KEY("53.1 User favorite journey");
	}
	
	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<UserFavoriteJourneyTableSync>::TABLE_NAME("t048_user_favorite_journey");
		template<> const int SQLiteTableSyncTemplate<UserFavoriteJourneyTableSync>::TABLE_ID(48);
		template<> const bool SQLiteTableSyncTemplate<UserFavoriteJourneyTableSync>::HAS_AUTO_INCREMENT(true);



		template<> void SQLiteDirectTableSyncTemplate<UserFavoriteJourneyTableSync,UserFavoriteJourney>::Load(
			UserFavoriteJourney* object
			, const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
			// Properties
			object->setRank(rows->getInt(UserFavoriteJourneyTableSync::COL_RANK));
			object->setDestinationCityName(rows->getText(UserFavoriteJourneyTableSync::COL_DESTINATION_CITY_NAME));
			object->setDestinationPlaceName(rows->getText(UserFavoriteJourneyTableSync::COL_DESTINATION_PLACE_NAME));
			object->setOriginCityName(rows->getText(UserFavoriteJourneyTableSync::COL_ORIGIN_CITY_NAME));
			object->setOriginPlaceName(rows->getText(UserFavoriteJourneyTableSync::COL_ORIGIN_PLACE_NAME));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				const User* user(UserTableSync::Get(rows->getLongLong(UserFavoriteJourneyTableSync::COL_USER_ID), env, linkLevel));
				object->setUser(user);
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<UserFavoriteJourneyTableSync,UserFavoriteJourney>::Save(
			UserFavoriteJourney* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());

			assert(object->getUser());
            
			query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToString(object->getUser()->getKey())
				<< ",";
			if (object->getRank() == UNKNOWN_VALUE)
				query << "(SELECT MAX(" << UserFavoriteJourneyTableSync::COL_RANK << ") +1 FROM " << UserFavoriteJourneyTableSync::TABLE_NAME << " WHERE " << UserFavoriteJourneyTableSync::COL_USER_ID << "=" << object->getUser()->getKey() << ")";
			else
				query << Conversion::ToString(object->getRank());
			query
				<< "," << Conversion::ToSQLiteString(object->getOriginCityName())
				<< "," << Conversion::ToSQLiteString(object->getOriginPlaceName())
				<< "," << Conversion::ToSQLiteString(object->getDestinationCityName())
				<< "," << Conversion::ToSQLiteString(object->getDestinationPlaceName())
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<UserFavoriteJourneyTableSync,UserFavoriteJourney>::Unlink(
			UserFavoriteJourney* object,
			Env* env
		){
			object->setUser(NULL);
		}
	}
	
	
	
	namespace routeplanner
	{
		const std::string UserFavoriteJourneyTableSync::COL_USER_ID("user_id");
		const std::string UserFavoriteJourneyTableSync::COL_RANK("rank");
		const std::string UserFavoriteJourneyTableSync::COL_ORIGIN_CITY_NAME("origin_city_name");
		const std::string UserFavoriteJourneyTableSync::COL_ORIGIN_PLACE_NAME("origin_place_name");
		const std::string UserFavoriteJourneyTableSync::COL_DESTINATION_CITY_NAME("destination_city_name");
		const std::string UserFavoriteJourneyTableSync::COL_DESTINATION_PLACE_NAME("destination_place_name");



		UserFavoriteJourneyTableSync::UserFavoriteJourneyTableSync()
			: SQLiteNoSyncTableSyncTemplate<UserFavoriteJourneyTableSync,UserFavoriteJourney>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_USER_ID, "INTEGER");
			addTableColumn(COL_RANK, "INTEGER");
			addTableColumn(COL_ORIGIN_CITY_NAME, "TEXT");
			addTableColumn(COL_ORIGIN_PLACE_NAME, "TEXT");
			addTableColumn(COL_DESTINATION_CITY_NAME, "TEXT");
			addTableColumn(COL_DESTINATION_PLACE_NAME, "TEXT");
		}



		void UserFavoriteJourneyTableSync::Search(
			Env& env,
			const User* user
			, int first /*= 0*/
			, int number /*= 0*/
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE "
				<< COL_USER_ID << "=" << user->getKey()
				<< " ORDER BY " << COL_RANK << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
