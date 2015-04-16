
//////////////////////////////////////////////////////////////////////////////////////////
/// ServiceQuotaTableSync class implementation.
///	@file ServiceQuotaTableSync.cpp
///	@author Gael Sauvanet
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ServiceQuotaTableSync.hpp"

#include "SelectQuery.hpp"
#include "ObjectSiteLinkTableSync.h"

#include <sstream>
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,ServiceQuotaTableSync>::FACTORY_KEY("35.81.01 Service Quota");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ServiceQuotaTableSync>::TABLE(
			"t089_service_quotas"
		);

		template<> const Field DBTableSyncTemplate<ServiceQuotaTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ServiceQuotaTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<ServiceQuotaTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<ServiceQuotaTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ServiceQuotaTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ServiceQuotaTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}
	}

	namespace pt
	{
		ServiceQuotaTableSync::SearchResult ServiceQuotaTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> serviceId /*= boost::optional<util::RegistryKeyType>()*/,
			size_t first /*= 0*/,
			boost::optional<std::size_t> number /*= boost::optional<std::size_t>()*/,
			bool raisingOrder /*= true*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
			){
				SelectQuery<ServiceQuotaTableSync> query;
				if (serviceId)
				{
					query.addWhereField(SimpleObjectFieldDefinition<Service>::FIELD.name, *serviceId);
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
