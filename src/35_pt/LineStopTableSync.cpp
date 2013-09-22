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
#include "DesignatedLinePhysicalStop.hpp"
#include "DRTAreaTableSync.hpp"
#include "JourneyPatternCopy.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineArea.hpp"
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

	namespace pt
	{
		const std::string LineStopTableSync::COL_PHYSICALSTOPID("physical_stop_id");
		const std::string LineStopTableSync::COL_LINEID("line_id");
		const std::string LineStopTableSync::COL_RANKINPATH("rank_in_path");
		const std::string LineStopTableSync::COL_ISDEPARTURE("is_departure");
		const std::string LineStopTableSync::COL_ISARRIVAL("is_arrival");
		const std::string LineStopTableSync::COL_METRICOFFSET("metric_offset");
		const std::string LineStopTableSync::COL_SCHEDULEINPUT("schedule_input");
		const std::string LineStopTableSync::COL_INTERNAL_SERVICE("internal_service");
		const std::string LineStopTableSync::COL_RESERVATION_NEEDED = "reservation_needed";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<LineStopTableSync>::TABLE(
			"t010_line_stops"
		);



		template<> const Field DBTableSyncTemplate<LineStopTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(LineStopTableSync::COL_PHYSICALSTOPID, SQL_INTEGER),
			Field(LineStopTableSync::COL_LINEID, SQL_INTEGER),
			Field(LineStopTableSync::COL_RANKINPATH, SQL_INTEGER),
			Field(LineStopTableSync::COL_ISDEPARTURE, SQL_BOOLEAN),
			Field(LineStopTableSync::COL_ISARRIVAL, SQL_BOOLEAN),
			Field(LineStopTableSync::COL_METRICOFFSET, SQL_DOUBLE),
			Field(LineStopTableSync::COL_SCHEDULEINPUT, SQL_BOOLEAN),
			Field(LineStopTableSync::COL_INTERNAL_SERVICE, SQL_BOOLEAN),
			Field(LineStopTableSync::COL_RESERVATION_NEEDED, SQL_BOOLEAN),
			Field(TABLE_COL_GEOMETRY, SQL_GEOM_LINESTRING),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<LineStopTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					LineStopTableSync::COL_LINEID.c_str(),
					LineStopTableSync::COL_RANKINPATH.c_str(),
					""
			)	);
			r.push_back(
				DBTableSync::Index(LineStopTableSync::COL_PHYSICALSTOPID.c_str(), "")
			);
			return r;
		}



		template<>
		boost::shared_ptr<LineStop> InheritanceLoadSavePolicy<LineStopTableSync,LineStop>::GetNewObject(
			const Record& row
		){
			return
				(	decodeTableId(row.getDefault<RegistryKeyType>(LineStopTableSync::COL_PHYSICALSTOPID, 0)) == StopPointTableSync::TABLE.ID) ?
					boost::shared_ptr<LineStop>(new DesignatedLinePhysicalStop(row.getDefault<RegistryKeyType>(TABLE_COL_ID, 0))) :
					boost::shared_ptr<LineStop>(new LineArea(row.getDefault<RegistryKeyType>(TABLE_COL_ID, 0))
				)
			;
		}



		template<>
		void InheritanceLoadSavePolicy<LineStopTableSync,LineStop>::Load(
			LineStop* ls,
			const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				DBModule::LoadObjects(ls->getLinkedObjectsIds(*rows), env, linkLevel);
			}
			ls->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				ls->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<>
		void InheritanceLoadSavePolicy<LineStopTableSync, LineStop>::Save(
			LineStop* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<LineStopTableSync> query(*object);
			if(dynamic_cast<DesignatedLinePhysicalStop*>(object))
			{
				DesignatedLinePhysicalStop& dls(static_cast<DesignatedLinePhysicalStop&>(*object));
				if(!dls.getPhysicalStop()) throw Exception("Linestop save error. Missing physical stop");
				if(!dls.getLine()) throw Exception("Linestop Save error. Missing line");

				query.addField(dls.getPhysicalStop()->getKey());
				query.addField(dls.getLine()->getKey());
				query.addField(dls.getRankInPath());
				query.addField(dls.isDepartureAllowed());
				query.addField(dls.isArrivalAllowed());
				query.addField(dls.getMetricOffset());
				query.addField(dls.getScheduleInput());
				query.addField(false);
				query.addField(dls.getReservationNeeded());
				query.addField(static_pointer_cast<Geometry,LineString>(dls.getGeometry()));
			}
			else if(dynamic_cast<LineArea*>(object))
			{
				LineArea& lineArea(static_cast<LineArea&>(*object));
				if(!lineArea.getArea()) throw Exception("LineArea save error. Missing physical stop");
				if(!lineArea.getLine()) throw Exception("LineArea Save error. Missing line");

				query.addField(lineArea.getArea()->getKey());
				query.addField(lineArea.getLine()->getKey());
				query.addField(lineArea.getRankInPath());
				query.addField(lineArea.isDepartureAllowed());
				query.addField(lineArea.isArrivalAllowed());
				query.addField(lineArea.getMetricOffset());
				query.addField(true);
				query.addField(lineArea.getInternalService());
				query.addField(static_pointer_cast<Geometry,LineString>(lineArea.getGeometry()));
				query.addField(true);
				query.execute(transaction);
			}
			query.execute(transaction);
		}



		template<>
		void InheritanceLoadSavePolicy<LineStopTableSync, LineStop>::Unlink(
			LineStop* obj
		){
			if(obj->getLine())
			{
				obj->getLine()->removeEdge(*obj);
			}

			if(dynamic_cast<DesignatedLinePhysicalStop*>(obj))
			{
				DesignatedLinePhysicalStop& object(
					dynamic_cast<DesignatedLinePhysicalStop&>(*obj)
				);

				// Useful transfer calculation
				if(object.getPhysicalStop())
				{
					object.getPhysicalStop()->getHub()->clearAndPropagateUsefulTransfer(PTModule::GRAPH_ID);
				}

				object.clearPhysicalStopLinks();
			}
			else if(dynamic_cast<LineArea*>(obj))
			{
				LineArea& object(
					dynamic_cast<LineArea&>(*obj)
				);

				// Useful transfer calculation
				BOOST_FOREACH(StopArea* stopArea, object.getArea()->get<Stops>())
				{
					stopArea->clearAndPropagateUsefulTransfer(PTModule::GRAPH_ID);
				}

				object.clearArea();
			}

			// Clear cache in case of non detected change in external objects (like path edges number)
			if(obj->getLine())
			{
				BOOST_FOREACH(const ServiceSet::value_type& service, obj->getLine()->getServices())
				{
					if(dynamic_cast<SchedulesBasedService*>(service))
					{
						static_cast<SchedulesBasedService*>(service)->_clearGeneratedSchedules();
					}
				}
			}
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

			RankUpdateQuery<LineStopTableSync> query(LineStopTableSync::COL_RANKINPATH, -1, lineStop->getRankInPath()+1);
			query.addWhereField(LineStopTableSync::COL_LINEID, lineStop->getLine()->getKey());
			query.execute(transaction);

			if(lineStop->getScheduleInput())
			{
				LineStopTableSync::Search(env, lineStop->getParentPath()->getKey());
				ScheduledServiceTableSync::Search(
					env,
					lineStop->getParentPath()->getKey()
				);
				ContinuousServiceTableSync::Search(
					env,
					lineStop->getParentPath()->getKey()
				);
				lineStop->getParentPath()->removeEdge(*lineStop);
				size_t rank(lineStop->getLine()->getRankInDefinedSchedulesVector(lineStop->getRankInPath()));
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
				query.addWhereField(COL_LINEID, *lineId);
			}
			if (physicalStopId)
			{
				query.addWhereField(COL_PHYSICALSTOPID, *physicalStopId);
			}
			if (orderByRank)
			{
				query.addOrderField(COL_RANKINPATH, raisingOrder);
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
				query.addWhereField(COL_PHYSICALSTOPID, *startStop);
			}

			if(endStop)
			{
				query.addWhere(
					ComposedExpression::Get(
						SubQueryExpression::Get(
							string("SELECT b."+ COL_PHYSICALSTOPID +" FROM "+ TABLE.NAME +" AS b WHERE b."+ COL_LINEID +"="+ TABLE.NAME +"."+ COL_LINEID +" AND b."+ COL_RANKINPATH +"="+ TABLE.NAME +"."+ COL_RANKINPATH +"+1")
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
			if(!lineStop.getParentPath()->getEdges().empty())
			{
				for(size_t rank((*lineStop.getParentPath()->getEdges().rbegin())->getRankInPath()); rank >= lineStop.getRankInPath(); --rank)
				{
					UpdateQuery<LineStopTableSync> updateQuery;
					updateQuery.addUpdateField(COL_RANKINPATH, RawSQL(COL_RANKINPATH + " + " + boost::lexical_cast<string>(1)));
					updateQuery.addWhereField(
						COL_RANKINPATH,
						rank
					);
					updateQuery.addWhereField(
						COL_LINEID,
						lineStop.getParentPath()->getKey()
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
			// Current lenght
			Env env2;
			SearchResult lineStops(
				Search(env2, lineStop.getParentPath()->getKey())
			);
			const Path& path(
				*(*lineStops.begin())->getParentPath()
			);


			if(lineStop.getRankInPath() + 1 == path.getEdges().size())
			{
				return;
			}

			MetricOffset oldLength(
				path.getEdge(lineStop.getRankInPath()+1)->getMetricOffset() -
				path.getEdge(lineStop.getRankInPath())->getMetricOffset()
			);

			if(oldLength == newLength)
			{
				return;
			}

			int difference(static_cast<int>(newLength) - static_cast<int>(oldLength));

			for(size_t rank((*path.getEdges().rbegin())->getRankInPath()); rank > lineStop.getRankInPath(); --rank)
			{
				UpdateQuery<LineStopTableSync> updateQuery;
				updateQuery.addUpdateField(COL_METRICOFFSET, RawSQL(COL_METRICOFFSET + "+(" + boost::lexical_cast<string>(difference) +")"));
				updateQuery.addWhereField(
					COL_RANKINPATH,
					rank
				);
				updateQuery.addWhereField(
					COL_LINEID,
					path.getKey()
				);
				updateQuery.execute(transaction);
				if(rank == 0)
				{
					break;
				}
			}
		}



		boost::shared_ptr<DesignatedLinePhysicalStop> LineStopTableSync::SearchSimilarLineStop(
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
					"t1." << LineStopTableSync::COL_LINEID << "=t2." << LineStopTableSync::COL_LINEID << " AND " <<
					"t1." << LineStopTableSync::COL_RANKINPATH << "+1=t2." << LineStopTableSync::COL_RANKINPATH << " AND " <<
					"t1." << LineStopTableSync::COL_PHYSICALSTOPID << "=s1." << TABLE_COL_ID << " AND " <<
					"t2." << LineStopTableSync::COL_PHYSICALSTOPID << "=s2." << TABLE_COL_ID << " AND " <<
					"s1." << StopPointTableSync::COL_PLACEID << "=" << departure.getKey() << " AND " <<
					"s2." << StopPointTableSync::COL_PLACEID << "=" << arrival.getKey() <<
				" ORDER BY " <<
					"NumPoints(t1." << TABLE_COL_GEOMETRY << ") DESC," <<
					"t2." << LineStopTableSync::COL_METRICOFFSET << "-t1." << LineStopTableSync::COL_METRICOFFSET << " DESC" <<
				" LIMIT 1"
			;
			SearchResult result(
				LoadFromQuery(query.str(), env, UP_LINKS_LOAD_LEVEL)
			);
			return result.empty() ?
				boost::shared_ptr<DesignatedLinePhysicalStop>() :
				static_pointer_cast<DesignatedLinePhysicalStop, LineStop>(*result.begin());
		}



		boost::shared_ptr<DesignatedLinePhysicalStop> LineStopTableSync::SearchSimilarLineStop(
			const StopPoint& departure,
			const StopPoint& arrival,
			util::Env& env
		){
			SelectQuery<LineStopTableSync> query;
			query.addTableJoin<LineStopTableSync>(
				ComposedExpression::Get(
					ComposedExpression::Get(
						FieldExpression::Get(LineStopTableSync::TABLE.NAME, LineStopTableSync::COL_LINEID),
						ComposedExpression::OP_EQ,
						FieldExpression::Get("t2", LineStopTableSync::COL_LINEID)
					),
					ComposedExpression::OP_AND,
					ComposedExpression::Get(
						ComposedExpression::Get(
							FieldExpression::Get(LineStopTableSync::TABLE.NAME, LineStopTableSync::COL_RANKINPATH),
							ComposedExpression::OP_ADD,
							ValueExpression<int>::Get(1)
						),
						ComposedExpression::OP_EQ,
						FieldExpression::Get("t2", LineStopTableSync::COL_RANKINPATH)
				)	),
				"t2"
			);
			query.addWhereField(LineStopTableSync::COL_PHYSICALSTOPID, departure.getKey());
			query.addWhereFieldOtherAlias("t2", LineStopTableSync::COL_PHYSICALSTOPID, arrival.getKey());
			query.addOrder(
				ValueExpression<string>::Get("NumPoints("+ LineStopTableSync::TABLE.NAME +"."+ TABLE_COL_GEOMETRY +")"),
				false
			);
			query.addOrder(
				ComposedExpression::Get(
					FieldExpression::Get("t2", LineStopTableSync::COL_METRICOFFSET),
					ComposedExpression::OP_SUB,
					FieldExpression::Get(LineStopTableSync::TABLE.NAME, LineStopTableSync::COL_METRICOFFSET)
				),
				false
			);
			query.setNumber(1);
			SearchResult result(
				LoadFromQuery(query.toString(), env, UP_LINKS_LOAD_LEVEL)
			);
			return result.empty() ?
				boost::shared_ptr<DesignatedLinePhysicalStop>() :
				static_pointer_cast<DesignatedLinePhysicalStop, LineStop>(*result.begin())
			;
		}
}	}
