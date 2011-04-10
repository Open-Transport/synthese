
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
#include "UserTableSync.h"
#include "ReplaceQuery.h"
#include "TransportWebsiteRight.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt_website;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,UserFavoriteJourneyTableSync>::FACTORY_KEY("53.1 User favorite journey");
	}

	namespace pt_website
	{
		const std::string UserFavoriteJourneyTableSync::COL_USER_ID("user_id");
		const std::string UserFavoriteJourneyTableSync::COL_RANK("rank");
		const std::string UserFavoriteJourneyTableSync::COL_ORIGIN_CITY_NAME("origin_city_name");
		const std::string UserFavoriteJourneyTableSync::COL_ORIGIN_PLACE_NAME("origin_place_name");
		const std::string UserFavoriteJourneyTableSync::COL_DESTINATION_CITY_NAME("destination_city_name");
		const std::string UserFavoriteJourneyTableSync::COL_DESTINATION_PLACE_NAME("destination_place_name");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<UserFavoriteJourneyTableSync>::TABLE(
				"t048_user_favorite_journey"
				);

		template<> const DBTableSync::Field DBTableSyncTemplate<UserFavoriteJourneyTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(UserFavoriteJourneyTableSync::COL_USER_ID, SQL_INTEGER),
			DBTableSync::Field(UserFavoriteJourneyTableSync::COL_RANK, SQL_INTEGER),
			DBTableSync::Field(UserFavoriteJourneyTableSync::COL_ORIGIN_CITY_NAME, SQL_TEXT),
			DBTableSync::Field(UserFavoriteJourneyTableSync::COL_ORIGIN_PLACE_NAME, SQL_TEXT),
			DBTableSync::Field(UserFavoriteJourneyTableSync::COL_DESTINATION_CITY_NAME, SQL_TEXT),
			DBTableSync::Field(UserFavoriteJourneyTableSync::COL_DESTINATION_PLACE_NAME, SQL_TEXT),
			DBTableSync::Field()
		};
		
		template<> const DBTableSync::Index DBTableSyncTemplate<UserFavoriteJourneyTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<UserFavoriteJourneyTableSync,UserFavoriteJourney>::Load(
			UserFavoriteJourney* object
			, const db::DBResultSPtr& rows,
			Env& env,
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
				const User* user(UserTableSync::Get(rows->getLongLong(UserFavoriteJourneyTableSync::COL_USER_ID), env, linkLevel).get());
				object->setUser(user);
			}
		}



		template<> void DBDirectTableSyncTemplate<UserFavoriteJourneyTableSync,UserFavoriteJourney>::Save(
			UserFavoriteJourney* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<UserFavoriteJourneyTableSync> query(*object);
			query.addField(object->getUser() ? object->getUser()->getKey() : RegistryKeyType(0));
			if (!object->getRank())
			{
				query.addFieldExpression(
					SubQueryExpression::Get(
						"SELECT MAX(" + UserFavoriteJourneyTableSync::COL_RANK + string(") +1 FROM ") + 
						UserFavoriteJourneyTableSync::TABLE.NAME + " WHERE " +
						UserFavoriteJourneyTableSync::COL_USER_ID + "=" + lexical_cast<string>(object->getUser()->getKey())
				)	);
			}
			else
			{
				query.addField(object->getRank());
			}
			query.addField(object->getOriginCityName());
			query.addField(object->getOriginPlaceName());
			query.addField(object->getDestinationCityName());
			query.addField(object->getDestinationPlaceName());
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<UserFavoriteJourneyTableSync,UserFavoriteJourney>::Unlink(
			UserFavoriteJourney* object
		){
			object->setUser(NULL);
		}



		template<> bool DBTableSyncTemplate<UserFavoriteJourneyTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<UserFavoriteJourneyTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<UserFavoriteJourneyTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		void DBTableSyncTemplate<UserFavoriteJourneyTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log the removal
		}
	}



	namespace pt_website
	{
		UserFavoriteJourneyTableSync::SearchResult UserFavoriteJourneyTableSync::Search(
			Env& env,
			const User* user
			, int first /*= 0*/
			, boost::optional<std::size_t> number /*= 0*/
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE "
				<< COL_USER_ID << "=" << user->getKey()
				<< " ORDER BY " << COL_RANK << (raisingOrder ? " ASC" : " DESC");
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << Conversion::ToString(first);
			}
			
			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
