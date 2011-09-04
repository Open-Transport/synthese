
//////////////////////////////////////////////////////////////////////////
///	DesignatedLinePhysicalStopInheritedTableSync class implementation.
///	@file DesignatedLinePhysicalStopInheritedTableSync.cpp
///	@author RCSobility
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include "DesignatedLinePhysicalStopInheritedTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "ReplaceQuery.h"
#include "JourneyPattern.hpp"
#include "JourneyPatternCopy.hpp"
#include "StopArea.hpp"

#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace pt;
	using namespace util;

	template<>
	const string util::FactorableTemplate<LineStopTableSync,DesignatedLinePhysicalStopInheritedTableSync>::FACTORY_KEY("DesignatedLinePhysicalStopInheritedTableSync");

	namespace db
	{
		template<>
		void DBInheritedTableSyncTemplate<LineStopTableSync,DesignatedLinePhysicalStopInheritedTableSync,DesignatedLinePhysicalStop>::Load(
			DesignatedLinePhysicalStop* obj,
			const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);

			// Schedule input
			if(!rows->getText(LineStopTableSync::COL_SCHEDULEINPUT).empty())
			{
				obj->setScheduleInput(rows->getBool(LineStopTableSync::COL_SCHEDULEINPUT));
			}
			else
			{
				obj->setScheduleInput(true);
			}

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Stop point
				util::RegistryKeyType fromPhysicalStopId (
					rows->getLongLong (LineStopTableSync::COL_PHYSICALSTOPID)
				);
				obj->setPhysicalStop(*StopPointTableSync::GetEditable(fromPhysicalStopId, env, linkLevel));

				// Line update
				obj->getLine()->addEdge(*obj);
			
				// Sublines update
				BOOST_FOREACH(JourneyPatternCopy* copy, static_cast<LineStop*>(obj)->getLine()->getSubLines())
				{
					DesignatedLinePhysicalStop* newEdge(
						new DesignatedLinePhysicalStop(
							0,
							copy,
							obj->getRankInPath(),
							obj->isDeparture(),
							obj->isArrival(),
							obj->getMetricOffset(),
							obj->getPhysicalStop(),
							obj->getScheduleInput()
					)	);
					copy->addEdge(*newEdge);
				}
			}
		}



		template<>
		void DBInheritedTableSyncTemplate<LineStopTableSync,DesignatedLinePhysicalStopInheritedTableSync,DesignatedLinePhysicalStop>::Unlink(
			DesignatedLinePhysicalStop* obj
		){
			obj->clearPhysicalStop();
		}



		template<>
		void DBInheritedTableSyncTemplate<LineStopTableSync,DesignatedLinePhysicalStopInheritedTableSync,DesignatedLinePhysicalStop>::Save(
			DesignatedLinePhysicalStop* object,
			optional<DBTransaction&> transaction
		){
			if(!object->getPhysicalStop()) throw Exception("Linestop save error. Missing physical stop");
			if(!object->getLine()) throw Exception("Linestop Save error. Missing line");

			ReplaceQuery<LineStopTableSync> query(*object);
			query.addField(object->getPhysicalStop()->getKey());
			query.addField(object->getLine()->getKey());
			query.addField(object->getRankInPath());
			query.addField(object->isDepartureAllowed());
			query.addField(object->isArrivalAllowed());
			query.addField(object->getMetricOffset());
			query.addField(object->getScheduleInput());
			query.addField(false);
			query.addField(static_pointer_cast<Geometry,LineString>(object->getGeometry()));
			query.execute(transaction);
		}
	}

	namespace pt
	{
		DesignatedLinePhysicalStopInheritedTableSync::SearchResult DesignatedLinePhysicalStopInheritedTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> startStop,
			boost::optional<util::RegistryKeyType> endStop,
			size_t first,
			boost::optional<std::size_t> number,
			bool orderById,
			bool raisingOrder,
			LinkLevel linkLevel
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



		boost::shared_ptr<DesignatedLinePhysicalStop> DesignatedLinePhysicalStopInheritedTableSync::SearchSimilarLineStop(
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
			return result.empty() ? shared_ptr<DesignatedLinePhysicalStop>() : *result.begin();
		}



		boost::shared_ptr<DesignatedLinePhysicalStop> DesignatedLinePhysicalStopInheritedTableSync::SearchSimilarLineStop(
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
			return result.empty() ? shared_ptr<DesignatedLinePhysicalStop>() : *result.begin();
		}

}	}
