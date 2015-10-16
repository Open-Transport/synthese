
//////////////////////////////////////////////////////////////////////////
///	JunctionTableSync class implementation.
///	@file JunctionTableSync.cpp
///	@author Hugues Romain
///	@date 2010
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

#include "JunctionTableSync.hpp"

#include "Profile.h"
#include "ReplaceQuery.h"
#include "Session.h"
#include "User.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "DataSourceTableSync.h"
#include "StopPointTableSync.hpp"
#include "TransportNetworkRight.h"
#include "StopAreaTableSync.hpp"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace impex;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,JunctionTableSync>::FACTORY_KEY("35.60 Junctions");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<JunctionTableSync>::TABLE(
			"t066_junctions"
		);



		template<> const Field DBTableSyncTemplate<JunctionTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<JunctionTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					StartPhysicalStop::FIELD.name.c_str(),
					EndPhysicalStop::FIELD.name.c_str(),
			"")	);
			r.push_back(
				DBTableSync::Index(
					EndPhysicalStop::FIELD.name.c_str(),
					StartPhysicalStop::FIELD.name.c_str(),
			"")	);
			return r;
		}



		template<> bool DBTableSyncTemplate<JunctionTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<JunctionTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<JunctionTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<JunctionTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace pt
	{
		JunctionTableSync::SearchResult JunctionTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> startStopFilter,
			boost::optional<util::RegistryKeyType> endStopFilter,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByStop,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<JunctionTableSync> query;
			if(startStopFilter)
			{
				if(decodeTableId(*startStopFilter) == StopAreaTableSync::TABLE.ID)
				{
					boost::shared_ptr<SQLExpression> expr(ValueExpression<int>::Get(0));
					StopPointTableSync::SearchResult stops(
						StopPointTableSync::Search(env, *startStopFilter)
					);
					if(!stops.empty())
					{
						BOOST_FOREACH(const boost::shared_ptr<StopPoint>& stop, stops)
						{
							expr = ComposedExpression::Get(
								expr,
								ComposedExpression::OP_OR,
								ComposedExpression::Get(
									FieldExpression::Get(TABLE.NAME, StartPhysicalStop::FIELD.name),
									ComposedExpression::OP_EQ,
									ValueExpression<RegistryKeyType>::Get(stop->getKey())
							)	);
						}
						query.addWhere(expr);
				}	}
				else
				{
					query.addWhereField(StartPhysicalStop::FIELD.name, *startStopFilter);
				}
			}
			if(endStopFilter)
			{
				if(decodeTableId(*endStopFilter) == StopAreaTableSync::TABLE.ID)
				{
					boost::shared_ptr<SQLExpression> expr(ValueExpression<int>::Get(0));
					StopPointTableSync::SearchResult stops(
						StopPointTableSync::Search(env, *endStopFilter)
					);
					if(!stops.empty())
					{
						BOOST_FOREACH(const boost::shared_ptr<StopPoint>& stop, stops)
						{
							expr = ComposedExpression::Get(
								expr,
								ComposedExpression::OP_OR,
								ComposedExpression::Get(
									FieldExpression::Get(TABLE.NAME, EndPhysicalStop::FIELD.name),
									ComposedExpression::OP_EQ,
									ValueExpression<RegistryKeyType>::Get(stop->getKey())
							)	);
						}
						query.addWhere(expr);
				}	}
				else
				{
					query.addWhereField(EndPhysicalStop::FIELD.name, *endStopFilter);
				}
			}
			if(orderByStop)
			{
				query.addOrderField(StartPhysicalStop::FIELD.name, raisingOrder);
				query.addOrderField(EndPhysicalStop::FIELD.name, raisingOrder);
			}
			if(number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
				{
					query.setFirst(first);
			}	}

			return LoadFromQuery(query, env, linkLevel);
		}

		bool JunctionTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::READ);
		}
	}
}
