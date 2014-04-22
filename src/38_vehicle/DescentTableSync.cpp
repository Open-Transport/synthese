
//////////////////////////////////////////////////////////////////////////
///	DescentTableSync class implementation.
///	@file DescentTableSync.cpp
///	@author Camille Hue
///	@date 2014
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

#include "DescentTableSync.hpp"

#include "Descent.hpp"
#include "ImportableTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DescentTableSync>::FACTORY_KEY("38.01 Descents");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DescentTableSync>::TABLE(
			"t118_descents"
		);



		template<> const Field DBTableSyncTemplate<DescentTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DescentTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<DescentTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check tue user rights
			return true;
		}



		template<> void DBTableSyncTemplate<DescentTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DescentTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DescentTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace vehicle
	{
		DescentTableSync::SearchResult DescentTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> serviceId,
			const date& minDate,
			const date& maxDate,
			bool displayCancelled,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			util::LinkLevel linkLevel
		){
			SelectQuery<DescentTableSync> query;
			if(serviceId)
			{
				query.addWhereField(Service::FIELD.name, *serviceId);
			}
			
			query.addWhereField(Date::FIELD.name, to_iso_extended_string(minDate), ComposedExpression::OP_SUPEQ);
			query.addWhereField(Date::FIELD.name, to_iso_extended_string(maxDate), ComposedExpression::OP_INF);
			
			if (displayCancelled)
			{
				std::stringstream subQuery;
				subQuery << CancellationTime::FIELD.name << " IS NOT NULL";
				
				query.addWhere(
					SubQueryExpression::Get(subQuery.str())
				);
			}
			else
			{
				std::stringstream subQuery;
				subQuery << CancellationTime::FIELD.name << " IS NULL";
				
				query.addWhere(
					SubQueryExpression::Get(subQuery.str())
				);
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
}	}
