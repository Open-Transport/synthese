
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

	namespace pt
	{
		const std::string JunctionTableSync::COL_START_PHYSICAL_STOP_ID("start_physical_stop");
		const std::string JunctionTableSync::COL_END_PHYSICAL_STOP_ID("end_physical_stop");
		const std::string JunctionTableSync::COL_LENGTH("length");
		const std::string JunctionTableSync::COL_DURATION("duration");
		const std::string JunctionTableSync::COL_BIDIRECTIONAL("bidirectional");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<JunctionTableSync>::TABLE(
			"t066_junctions"
		);



		template<> const Field DBTableSyncTemplate<JunctionTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(JunctionTableSync::COL_START_PHYSICAL_STOP_ID, SQL_INTEGER),
			Field(JunctionTableSync::COL_END_PHYSICAL_STOP_ID, SQL_INTEGER),
			Field(JunctionTableSync::COL_LENGTH, SQL_INTEGER),
			Field(JunctionTableSync::COL_DURATION, SQL_INTEGER),
			Field(JunctionTableSync::COL_BIDIRECTIONAL, SQL_INTEGER),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<JunctionTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					JunctionTableSync::COL_START_PHYSICAL_STOP_ID.c_str(),
					JunctionTableSync::COL_END_PHYSICAL_STOP_ID.c_str(),
			"")	);
			r.push_back(
				DBTableSync::Index(
					JunctionTableSync::COL_END_PHYSICAL_STOP_ID.c_str(),
					JunctionTableSync::COL_START_PHYSICAL_STOP_ID.c_str(),
			"")	);
			return r;
		}



		template<> void OldLoadSavePolicy<JunctionTableSync,Junction>::Load(
			Junction* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType sid(rows->getLongLong(JunctionTableSync::COL_START_PHYSICAL_STOP_ID));
				RegistryKeyType eid(rows->getLongLong(JunctionTableSync::COL_END_PHYSICAL_STOP_ID));
				double length(rows->getDouble(JunctionTableSync::COL_LENGTH));
				time_duration duration(minutes(rows->getInt(JunctionTableSync::COL_DURATION)));
				bool bidir(rows->getBool(JunctionTableSync::COL_BIDIRECTIONAL));

				if(sid > 0 && eid > 0)
				{
					try
					{
						object->setStops(
							StopPointTableSync::GetEditable(sid, env, linkLevel).get(),
							StopPointTableSync::GetEditable(eid, env, linkLevel).get(),
							length,
							duration,
							bidir
						);
					}
					catch(ObjectNotFoundException<StopPoint>&)
					{
						Log::GetInstance().warn("No such stop in Junction "+ lexical_cast<string>(object->getKey()));
					}
				}
			}
		}



		template<> void OldLoadSavePolicy<JunctionTableSync,Junction>::Save(
			Junction* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<JunctionTableSync> query(*object);
			query.addField(object->isValid() ? object->getStart()->getKey() : RegistryKeyType(0));
			query.addField(object->isValid() ? object->getEnd()->getKey() : RegistryKeyType(0));
			query.addField(object->isValid() ? object->getLength() : double(0));
			query.addField(object->isValid() ? object->getDuration().total_seconds() / 60 : 0);
			query.addField(object->getBack() != NULL);
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<JunctionTableSync,Junction>::Unlink(
			Junction* obj
		){
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
									FieldExpression::Get(TABLE.NAME, COL_START_PHYSICAL_STOP_ID),
									ComposedExpression::OP_EQ,
									ValueExpression<RegistryKeyType>::Get(stop->getKey())
							)	);
						}
						query.addWhere(expr);
				}	}
				else
				{
					query.addWhereField(COL_START_PHYSICAL_STOP_ID, *startStopFilter);
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
									FieldExpression::Get(TABLE.NAME, COL_END_PHYSICAL_STOP_ID),
									ComposedExpression::OP_EQ,
									ValueExpression<RegistryKeyType>::Get(stop->getKey())
							)	);
						}
						query.addWhere(expr);
				}	}
				else
				{
					query.addWhereField(COL_END_PHYSICAL_STOP_ID, *endStopFilter);
				}
			}
			if(orderByStop)
			{
				query.addOrderField(COL_START_PHYSICAL_STOP_ID, raisingOrder);
				query.addOrderField(COL_END_PHYSICAL_STOP_ID, raisingOrder);
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
	}
}
