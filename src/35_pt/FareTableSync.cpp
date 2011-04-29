
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
#include "DBResult.hpp"
#include "DBException.hpp"
#include "ReplaceQuery.h"
#include "FareTableSync.h"
#include "TransportNetworkRight.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync, FareTableSync>::FACTORY_KEY("35.10.02 Fares");

	namespace pt
	{
		const std::string FareTableSync::COL_NAME ("name");
		const std::string FareTableSync::COL_FARETYPE ("fare_type");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<FareTableSync>::TABLE(
			"t008_fares"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<FareTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(FareTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(FareTableSync::COL_FARETYPE, SQL_INTEGER),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<FareTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};

		template<> void DBDirectTableSyncTemplate<FareTableSync,Fare>::Load(
			Fare* fare,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			fare->setName (rows->getText (FareTableSync::COL_NAME));
			fare->setType (static_cast<Fare::FareType>(rows->getInt (FareTableSync::COL_FARETYPE)));
		}

		template<> void DBDirectTableSyncTemplate<FareTableSync,Fare>::Save(
			Fare* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<FareTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getType());
			query.execute(transaction);
		}


		template<> void DBDirectTableSyncTemplate<FareTableSync,Fare>::Unlink(Fare* obj)
		{
		}



		template<> bool DBTableSyncTemplate<FareTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<FareTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<FareTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<FareTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		FareTableSync::SearchResult FareTableSync::Search(
			Env& env,
			optional<string> name,
			bool orderByName,
			bool raisingOrder,
			int first /*= 0*/,
			boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if(name)
			{
				query << " AND " << COL_NAME << " LIKE " << Conversion::ToDBString(*name);
			}
			if(orderByName)
			{
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			}
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		FareTableSync::NamedList FareTableSync::GetList(
			util::Env& env
		){
			NamedList result;
			SearchResult fares(Search(env));
			result.push_back(make_pair(RegistryKeyType(0), "(inconnu)"));

			BOOST_FOREACH(shared_ptr<Fare> fare, fares)
			{
				result.push_back(make_pair(fare->getKey(), fare->getName()));
			}

			return result;
		}
	}
}
