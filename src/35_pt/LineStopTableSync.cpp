////////////////////////////////////////////////////////////////////////////////
/// LineStopTableSync class implementation.
///	@file LineStopTableSync.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "LineStopTableSync.h"
#include "JourneyPatternCopy.hpp"
#include "JourneyPatternTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "LineAreaInheritedTableSync.hpp"
#include "DesignatedLinePhysicalStopInheritedTableSync.hpp"
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
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<LineStopTableSync>::TABLE(
			"t010_line_stops"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<LineStopTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(LineStopTableSync::COL_PHYSICALSTOPID, SQL_INTEGER),
			DBTableSync::Field(LineStopTableSync::COL_LINEID, SQL_INTEGER),
			DBTableSync::Field(LineStopTableSync::COL_RANKINPATH, SQL_INTEGER),
			DBTableSync::Field(LineStopTableSync::COL_ISDEPARTURE, SQL_BOOLEAN),
			DBTableSync::Field(LineStopTableSync::COL_ISARRIVAL, SQL_BOOLEAN),
			DBTableSync::Field(LineStopTableSync::COL_METRICOFFSET, SQL_DOUBLE),
			DBTableSync::Field(LineStopTableSync::COL_SCHEDULEINPUT, SQL_BOOLEAN),
			DBTableSync::Field(LineStopTableSync::COL_INTERNAL_SERVICE, SQL_BOOLEAN),
			DBTableSync::Field(TABLE_COL_GEOMETRY, SQL_GEOM_LINESTRING),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<LineStopTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(
				LineStopTableSync::COL_LINEID.c_str(),
				LineStopTableSync::COL_RANKINPATH.c_str(),
				""
			),
			DBTableSync::Index(LineStopTableSync::COL_PHYSICALSTOPID.c_str(), ""),
			DBTableSync::Index()
		};



		template<>
		string DBInheritanceTableSyncTemplate<LineStopTableSync,LineStop>::_GetSubClassKey(const DBResultSPtr& row)
		{
			return
				(decodeTableId(row->getLongLong(LineStopTableSync::COL_PHYSICALSTOPID)) == StopPointTableSync::TABLE.ID) ?
				DesignatedLinePhysicalStopInheritedTableSync::FACTORY_KEY :
				LineAreaInheritedTableSync::FACTORY_KEY
			;
		}



		template<>
		string DBInheritanceTableSyncTemplate<LineStopTableSync,LineStop>::_GetSubClassKey(const LineStop* obj)
		{
			return
				(dynamic_cast<const DesignatedLinePhysicalStop*>(obj) != NULL) ?
				DesignatedLinePhysicalStopInheritedTableSync::FACTORY_KEY :
				LineAreaInheritedTableSync::FACTORY_KEY
			;
		}



		template<> void DBInheritanceTableSyncTemplate<LineStopTableSync,LineStop>::_CommonLoad(
			LineStop* ls,
			const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			int rankInPath(rows->getInt (LineStopTableSync::COL_RANKINPATH));
			bool isDeparture (rows->getBool (LineStopTableSync::COL_ISDEPARTURE));
			bool isArrival (rows->getBool (LineStopTableSync::COL_ISARRIVAL));
			double metricOffset (rows->getDouble (LineStopTableSync::COL_METRICOFFSET));

			// Geometry
			string viaPointsStr(rows->getText(TABLE_COL_GEOMETRY));
			if(viaPointsStr.empty())
			{
				ls->setGeometry(shared_ptr<LineString>());
			}
			else
			{
				ls->setGeometry(
					dynamic_pointer_cast<LineString,Geometry>(rows->getGeometryFromWKT(TABLE_COL_GEOMETRY))
				);
			}

			ls->setMetricOffset(metricOffset);
			ls->setIsArrival(isArrival);
			ls->setIsDeparture(isDeparture);
			ls->setRankInPath(rankInPath);
			ls->setLine(NULL);

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Line
				util::RegistryKeyType lineId (rows->getLongLong (LineStopTableSync::COL_LINEID));
				JourneyPattern* line(JourneyPatternTableSync::GetEditable (lineId, env, linkLevel).get());
				ls->setLine(line);
			}
		}



		template<> bool DBTableSyncTemplate<LineStopTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
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
			shared_ptr<const LineStop> lineStop(LineStopTableSync::Get(id, env));

			RankUpdateQuery<LineStopTableSync> query(LineStopTableSync::COL_RANKINPATH, -1, lineStop->getRankInPath(), false);
			query.addWhereField(LineStopTableSync::COL_LINEID, lineStop->getLine()->getKey());
			query.execute(transaction);
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



		void LineStopTableSync::InsertStop(
			LineStop& lineStop
		){
			DBTransaction transaction;
		
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
					updateQuery.execute(transaction);
			}	}

			Save(&lineStop, transaction);

			transaction.run();
		}
}	}
