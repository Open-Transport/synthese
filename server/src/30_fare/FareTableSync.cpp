
/** FareTableSync class implementation.
	@file FareTableSync.cpp

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

#include "FareTableSync.hpp"

#include "Conversion.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "Session.h"
#include "User.h"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace fare;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync, FareTableSync>::FACTORY_KEY("30.10.02 Fares");

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<FareTableSync>::TABLE(
			"t008_fares"
		);

		template<> const Field DBTableSyncTemplate<FareTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<FareTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}

		template<> bool DBTableSyncTemplate<FareTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true; // TODO create fare right
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
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

	namespace fare
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
			SelectQuery<FareTableSync> query;
			if(name)
			{
				query.addWhereField(SimpleObjectFieldDefinition<Name>::FIELD.name, *name, ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
				query.addOrderField(SimpleObjectFieldDefinition<Name>::FIELD.name, raisingOrder);
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



		FareTableSync::NamedList FareTableSync::GetList(
			util::Env& env
		){
			NamedList result;
			SearchResult fares(Search(env));
			result.push_back(make_pair(RegistryKeyType(0), "(inconnu)"));

			BOOST_FOREACH(const boost::shared_ptr<Fare>& fare, fares)
			{
				result.push_back(make_pair(fare->getKey(), fare->getName()));
			}

			return result;
		}
	}
}
