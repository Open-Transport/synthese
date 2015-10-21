////////////////////////////////////////////////////////////////////////////////
/// LineStopTableSync class implementation.
///	@file LineStopTableSync.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "LineStopTableSync.h"

#include "ContinuousServiceTableSync.h"
#include "DRTAreaTableSync.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LinkException.h"
#include "Profile.h"
#include "ScheduledServiceTableSync.h"
#include "Session.h"
#include "StopPointTableSync.hpp"
#include "User.h"
#include "TransportNetworkRight.h"
#include "RankUpdateQuery.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "DBTransaction.hpp"

#include <sstream>
#include <geos/geom/LineString.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace geography;
	using namespace graph;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync,LineStopTableSync>::FACTORY_KEY("35.57.01 JourneyPattern stops");

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<LineStopTableSync>::TABLE(
			"t010_line_stops"
		);



		template<> const Field DBTableSyncTemplate<LineStopTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<LineStopTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					Line::FIELD.name.c_str(),
					RankInPath::FIELD.name.c_str(),
					""
			)	);
			r.push_back(
				DBTableSync::Index(LineNode::FIELD.name.c_str(), "")
			);
			return r;
		}



		template<> bool DBTableSyncTemplate<LineStopTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the user has sufficient right level
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL, string());
		}



		template<> void DBTableSyncTemplate<LineStopTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<LineStopTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			boost::shared_ptr<LineStop> lineStop(LineStopTableSync::GetEditable(id, env));

			RankUpdateQuery<LineStopTableSync> query(RankInPath::FIELD.name, -1, lineStop->get<RankInPath>()+1);
			query.addWhereField(Line::FIELD.name, lineStop->get<Line>()->getKey());
			query.execute(transaction);

			if(lineStop->get<ScheduleInput>())
			{
				LineStopTableSync::Search(env, lineStop->get<Line>()->getKey());
				ScheduledServiceTableSync::Search(
					env,
					lineStop->get<Line>()->getKey()
				);
				ContinuousServiceTableSync::Search(
					env,
					lineStop->get<Line>()->getKey()
				);
//				lineStop->get<Line>()->removeEdge(*lineStop);
				size_t rank(lineStop->get<Line>()->getRankInDefinedSchedulesVector(lineStop->get<RankInPath>()));
				BOOST_FOREACH(const ScheduledService::Registry::value_type& it, env.getRegistry<ScheduledService>())
				{
					ScheduledService& service(*it.second);
					ScheduledService::Schedules dp(service.getDataDepartureSchedules());
					dp.erase(dp.begin() + rank);
					ScheduledService::Schedules ar(service.getDataArrivalSchedules());
					ar.erase(ar.begin() + rank);
					service.setDataSchedules(dp, ar);
					ScheduledServiceTableSync::Save(&service, transaction);
				}
				BOOST_FOREACH(const ContinuousService::Registry::value_type& it, env.getRegistry<ContinuousService>())
				{
					ContinuousService& service(*it.second);
					ContinuousService::Schedules dp(service.getDataDepartureSchedules());
					dp.erase(dp.begin() + rank);
					ContinuousService::Schedules ar(service.getDataDepartureSchedules());
					ar.erase(ar.begin() + rank);
					service.setDataSchedules(dp, ar);
					ContinuousServiceTableSync::Save(&service, transaction);
				}
			}
		}



		template<> void DBTableSyncTemplate<LineStopTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		LineStopTableSync::SearchResult LineStopTableSync::Search(
			Env& env,
			optional<RegistryKeyType> lineId,
			optional<RegistryKeyType> physicalStopId
			, int first
			, boost::optional<std::size_t> number
			, bool orderByRank
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<LineStopTableSync> query;
			if (lineId)
			{
				query.addWhereField(Line::FIELD.name, *lineId);
			}
			if (physicalStopId)
			{
				query.addWhereField(LineNode::FIELD.name, *physicalStopId);
			}
			if (orderByRank)
			{
				query.addOrderField(RankInPath::FIELD.name, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query.toString(), env, linkLevel);
		}



		LineStopTableSync::SearchResult LineStopTableSync::SearchByStops(
			util::Env& env,
			boost::optional<util::RegistryKeyType> startStop /*= boost::optional<util::RegistryKeyType>()*/,
			boost::optional<util::RegistryKeyType> endStop /*= boost::optional<util::RegistryKeyType>()*/,
			std::size_t first /*= 0*/,
			boost::optional<std::size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderById /*= false*/,
			bool raisingOrder /*= false*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			SelectQuery<LineStopTableSync> query;

			if(startStop)
			{
				query.addWhereField(LineNode::FIELD.name, *startStop);
			}

			if(endStop)
			{
				query.addWhere(
					ComposedExpression::Get(
						SubQueryExpression::Get(
							string("SELECT b."+ LineNode::FIELD.name +" FROM "+ TABLE.NAME +" AS b WHERE b."+ Line::FIELD.name +"="+ TABLE.NAME +"."+ Line::FIELD.name +" AND b."+ RankInPath::FIELD.name +"="+ TABLE.NAME +"."+ RankInPath::FIELD.name +"+1")
						), ComposedExpression::OP_EQ,
						ValueExpression<RegistryKeyType>::Get(*endStop)
					)
				);
			}

			// Ordering
			if(orderById)
			{
				query.addOrderField(TABLE_COL_ID, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
				{
					query.setFirst(first);
				}
			}

			return LoadFromQuery(query.toString(), env, linkLevel);
		}



		void LineStopTableSync::InsertStop(
			LineStop& lineStop,
			DBTransaction& transaction
		){
			if(lineStop.get<Line>() && !lineStop.get<Line>()->getLineStops().empty())
			{
				for(size_t rank((*lineStop.get<Line>()->getLineStops().rbegin())->get<RankInPath>()); rank >= lineStop.get<RankInPath>(); --rank)
				{
					UpdateQuery<LineStopTableSync> updateQuery;
					updateQuery.addUpdateField(RankInPath::FIELD.name, RawSQL(RankInPath::FIELD.name + " + " + boost::lexical_cast<string>(1)));
					updateQuery.addWhereField(
						RankInPath::FIELD.name,
						rank
					);
					updateQuery.addWhereField(
						Line::FIELD.name,
						lineStop.get<Line>()->getKey()
					);
					updateQuery.execute(transaction);
					if(rank == 0)
					{
						break;
					}
			}	}

			Save(&lineStop, transaction);
		}



		void LineStopTableSync::ChangeLength(
			const LineStop& lineStop,
			MetricOffset newLength,
			boost::optional<db::DBTransaction&> transaction
		){
			// Current length
			Env env2;
			SearchResult lineStops(
				Search(env2, lineStop.get<Line>()->getKey())
			);
			const JourneyPattern& path(
				*(*lineStops.begin())->get<Line>()
			);


			if(lineStop.get<RankInPath>() + 1 == path.getLineStops().size())
			{
				return;
			}

			MetricOffset oldLength(
				path.getLineStop(lineStop.get<RankInPath>()+1)->get<MetricOffsetField>() -
				path.getLineStop(lineStop.get<RankInPath>())->get<MetricOffsetField>()
			);

			if(oldLength == newLength)
			{
				return;
			}

			int difference(static_cast<int>(newLength) - static_cast<int>(oldLength));

			for(size_t rank((*path.getLineStops().rbegin())->get<RankInPath>()); rank > lineStop.get<RankInPath>(); --rank)
			{
				UpdateQuery<LineStopTableSync> updateQuery;
				updateQuery.addUpdateField(MetricOffsetField::FIELD.name, RawSQL(MetricOffsetField::FIELD.name+ "+(" + boost::lexical_cast<string>(difference) +")"));
				updateQuery.addWhereField(
					RankInPath::FIELD.name,
					rank
				);
				updateQuery.addWhereField(
					Line::FIELD.name,
					path.getKey()
				);
				updateQuery.execute(transaction);
				if(rank == 0)
				{
					break;
				}
			}
		}



		boost::shared_ptr<LineStop> LineStopTableSync::SearchSimilarLineStop(
			const StopArea& departure,
			const StopArea& arrival,
			util::Env& env
		){
			stringstream query;
			query <<
				" SELECT t1.*" <<
				" FROM " <<
					LineStopTableSync::TABLE.NAME << " t1," <<
					LineStopTableSync::TABLE.NAME << " t2," <<
					StopPointTableSync::TABLE.NAME << " s1," <<
					StopPointTableSync::TABLE.NAME << " s2" <<
				" WHERE " <<
					"t1." << Line::FIELD.name << "=t2." << Line::FIELD.name << " AND " <<
					"t1." << RankInPath::FIELD.name << "+1=t2." << RankInPath::FIELD.name << " AND " <<
					"t1." << LineNode::FIELD.name << "=s1." << TABLE_COL_ID << " AND " <<
					"t2." << LineNode::FIELD.name << "=s2." << TABLE_COL_ID << " AND " <<
					"s1." << ConnectionPlace::FIELD.name << "=" << departure.getKey() << " AND " <<
					"s2." << ConnectionPlace::FIELD.name << "=" << arrival.getKey() <<
				" ORDER BY " <<
					"NumPoints(t1." << TABLE_COL_GEOMETRY << ") DESC," <<
					"t2." << MetricOffsetField::FIELD.name << "-t1." << MetricOffsetField::FIELD.name << " DESC" <<
				" LIMIT 1"
			;
			SearchResult result(
				LoadFromQuery(query.str(), env, UP_LINKS_LOAD_LEVEL)
			);
			return result.empty() ?
				boost::shared_ptr<LineStop>() :
				*result.begin();
		}



		boost::shared_ptr<LineStop> LineStopTableSync::SearchSimilarLineStop(
			const StopPoint& departure,
			const StopPoint& arrival,
			util::Env& env
		){
			SelectQuery<LineStopTableSync> query;
			query.addTableJoin<LineStopTableSync>(
				ComposedExpression::Get(
					ComposedExpression::Get(
						FieldExpression::Get(LineStopTableSync::TABLE.NAME, Line::FIELD.name),
						ComposedExpression::OP_EQ,
						FieldExpression::Get("t2", Line::FIELD.name)
					),
					ComposedExpression::OP_AND,
					ComposedExpression::Get(
						ComposedExpression::Get(
							FieldExpression::Get(LineStopTableSync::TABLE.NAME, RankInPath::FIELD.name),
							ComposedExpression::OP_ADD,
							ValueExpression<int>::Get(1)
						),
						ComposedExpression::OP_EQ,
						FieldExpression::Get("t2", RankInPath::FIELD.name)
				)	),
				"t2"
			);
			query.addWhereField(LineNode::FIELD.name, departure.getKey());
			query.addWhereFieldOtherAlias("t2", LineNode::FIELD.name, arrival.getKey());
			query.addOrder(
				ValueExpression<string>::Get("NumPoints("+ LineStopTableSync::TABLE.NAME +"."+ TABLE_COL_GEOMETRY +")"),
				false
			);
			query.addOrder(
				ComposedExpression::Get(
					FieldExpression::Get("t2", MetricOffsetField::FIELD.name),
					ComposedExpression::OP_SUB,
					FieldExpression::Get(LineStopTableSync::TABLE.NAME, MetricOffsetField::FIELD.name)
				),
				false
			);
			query.setNumber(1);
			SearchResult result(
				LoadFromQuery(query.toString(), env, UP_LINKS_LOAD_LEVEL)
			);
			return result.empty() ?
				boost::shared_ptr<LineStop>() :
				*result.begin()
			;
		}
}	}
