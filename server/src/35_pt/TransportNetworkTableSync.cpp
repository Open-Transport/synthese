
/** TransportNetworkTableSync class implementation.
	@file TransportNetworkTableSync.cpp

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

#include "TransportNetworkTableSync.h"

#include "CalendarTemplateTableSync.h"
#include "CommercialLineTableSync.h"
#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "TransportNetwork.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "ReservationContact.h"

#include <boost/logic/tribool.hpp>
#include <assert.h>

using boost::logic::tribool;
using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace impex;
	using namespace security;
	using namespace calendar;

	template<> const string util::FactorableTemplate<DBTableSync,TransportNetworkTableSync>::FACTORY_KEY(
		"35.20.02 Network transport"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TransportNetworkTableSync>::TABLE(
			"t022_transport_networks"
		);



		template<> const Field DBTableSyncTemplate<TransportNetworkTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TransportNetworkTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(DataSourceLinksWithoutUnderscore::FIELD.name.c_str(), ""));
			r.push_back(DBTableSync::Index(SimpleObjectFieldDefinition<Name>::FIELD.name.c_str(), ""));
			return r;
		};



		template<> bool DBTableSyncTemplate<TransportNetworkTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<TransportNetworkTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			CommercialLineTableSync::SearchResult lines(CommercialLineTableSync::Search(env, id));
			BOOST_FOREACH(const CommercialLineTableSync::SearchResult::value_type& line, lines)
			{
				CommercialLineTableSync::Remove(NULL, line->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<TransportNetworkTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<TransportNetworkTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		TransportNetworkTableSync::SearchResult TransportNetworkTableSync::Search(
			Env& env,
			string name,
			string creatorId,
			int first, /*= 0*/
			boost::optional<std::size_t> number,
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<TransportNetworkTableSync> query;
			if(!name.empty())
			{
				query.addWhereField(SimpleObjectFieldDefinition<Name>::FIELD.name, name, ComposedExpression::OP_LIKE);
			}
			if (!creatorId.empty())
			{
				query.addWhereField(DataSourceLinksWithoutUnderscore::FIELD.name, creatorId, ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
				query.addOrderField(SimpleObjectFieldDefinition<Name>::FIELD.name, raisingOrder);
			}
			if(number)
			{
				query.setNumber(*number + 1);
			}
			if(first > 0)
			{
				query.setFirst(first);
			}
			return LoadFromQuery(query, env, linkLevel);
		}



		db::RowsList TransportNetworkTableSync::SearchForAutoComplete(
				const boost::optional<std::string> prefix,
				const boost::optional<std::size_t> limit,
				const boost::optional<std::string> optionalParameter
		) const {
			RowsList result;

			SelectQuery<TransportNetworkTableSync> query;
			Env env;
			if(prefix) query.addWhereField(SimpleObjectFieldDefinition<Name>::FIELD.name, "%"+ *prefix +"%", ComposedExpression::OP_LIKE);
			if(limit) query.setNumber(*limit);
			TransportNetworkTableSync::SearchResult networks(TransportNetworkTableSync::LoadFromQuery(query, env, UP_LINKS_LOAD_LEVEL));
			BOOST_FOREACH(const boost::shared_ptr<TransportNetwork>& network, networks)
			{
				result.push_back(std::make_pair(network->getKey(), network->getName()));
			}
			return result;
		}
}	}
