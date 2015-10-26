
//////////////////////////////////////////////////////////////////////////
///	DriverServiceTableSync class implementation.
///	@file DriverServiceTableSync.cpp
///	@author Hugues Romain
///	@date 2011
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

#include "DriverServiceTableSync.hpp"

#include "DataSourceLinksField.hpp"
#include "DeadRun.hpp"
#include "DeadRunTableSync.hpp"
#include "DriverActivityTableSync.hpp"
#include "ImportableTableSync.hpp"
#include "OperationUnitTableSync.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "ScheduledServiceTableSync.h"
#include "SelectQuery.hpp"
#include "VehicleServiceTableSync.hpp"
#include "User.h"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace pt_operation;
	using namespace impex;
	using namespace pt;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, DriverServiceTableSync>::FACTORY_KEY("37.40 Driver services");
		template<> const string FactorableTemplate<Fetcher<Calendar>, DriverServiceTableSync>::FACTORY_KEY("81");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DriverServiceTableSync>::TABLE(
			"t081_driver_services"
		);



		template<> const Field DBTableSyncTemplate<DriverServiceTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DriverServiceTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(DriverServiceOperationUnit::FIELD.name.c_str(), ""));
			return r;
		}



		template<> bool DBTableSyncTemplate<DriverServiceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<DriverServiceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DriverServiceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DriverServiceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}



	namespace pt_operation
	{
		DriverServiceTableSync::SearchResult DriverServiceTableSync::Search(
			util::Env& env,
			boost::optional<std::string> searchName,
			boost::optional<util::RegistryKeyType> searchUnit,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<DriverServiceTableSync> query;
			if(searchName)
			{
				query.addWhereField(SimpleObjectFieldDefinition<Name>::FIELD.name, *searchName, ComposedExpression::OP_LIKE);
			}
			if(searchUnit)
			{
				query.addWhereField(DriverServiceOperationUnit::FIELD.name, *searchUnit);
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



		bool DriverServiceTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::READ);
		}
}	}
