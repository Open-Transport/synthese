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

#include "LineTableSync.h"
#include "PhysicalStopTableSync.h"

#include <sstream>

#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "SQLiteTransaction.h"
#include "Point2D.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace geometry;
	using namespace pt;

	template<> const string util::FactorableTemplate<SQLiteTableSync,LineStopTableSync>::FACTORY_KEY("15.57.01 JourneyPattern stops");

	namespace pt
	{
		const std::string LineStopTableSync::COL_PHYSICALSTOPID ("physical_stop_id");
		const std::string LineStopTableSync::COL_LINEID ("line_id");
		const std::string LineStopTableSync::COL_RANKINPATH ("rank_in_path");
		const std::string LineStopTableSync::COL_ISDEPARTURE ("is_departure");
		const std::string LineStopTableSync::COL_ISARRIVAL ("is_arrival");
		const std::string LineStopTableSync::COL_METRICOFFSET ("metric_offset");
		const std::string LineStopTableSync::COL_SCHEDULEINPUT ("schedule_input");
		const std::string LineStopTableSync::COL_VIAPOINTS ("via_points");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<LineStopTableSync>::TABLE(
			"t010_line_stops"
		);



		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<LineStopTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(LineStopTableSync::COL_PHYSICALSTOPID, SQL_INTEGER, false),
			SQLiteTableSync::Field(LineStopTableSync::COL_LINEID, SQL_INTEGER, false),
			SQLiteTableSync::Field(LineStopTableSync::COL_RANKINPATH, SQL_INTEGER),
			SQLiteTableSync::Field(LineStopTableSync::COL_ISDEPARTURE, SQL_BOOLEAN),
			SQLiteTableSync::Field(LineStopTableSync::COL_ISARRIVAL, SQL_BOOLEAN),
			SQLiteTableSync::Field(LineStopTableSync::COL_METRICOFFSET, SQL_DOUBLE),
			SQLiteTableSync::Field(LineStopTableSync::COL_VIAPOINTS, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<LineStopTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(
				LineStopTableSync::COL_LINEID.c_str(),
				LineStopTableSync::COL_RANKINPATH.c_str(),
				""
			),
			SQLiteTableSync::Index(LineStopTableSync::COL_PHYSICALSTOPID.c_str(), ""),
			SQLiteTableSync::Index()
		};

		
		template<> void SQLiteDirectTableSyncTemplate<LineStopTableSync,LineStop>::Load(
			LineStop* ls,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			int rankInPath(rows->getInt (LineStopTableSync::COL_RANKINPATH));
			bool isDeparture (rows->getBool (LineStopTableSync::COL_ISDEPARTURE));
			bool isArrival (rows->getBool (LineStopTableSync::COL_ISARRIVAL));
			double metricOffset (rows->getDouble (LineStopTableSync::COL_METRICOFFSET));
			std::string viaPointsStr (rows->getText (LineStopTableSync::COL_VIAPOINTS));

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			ls->clearViaPoints ();

			boost::char_separator<char> sep1 (",");
			boost::char_separator<char> sep2 (":");
			tokenizer viaPointsTokens (viaPointsStr, sep1);
			for (tokenizer::iterator viaPointIter = viaPointsTokens.begin();
				viaPointIter != viaPointsTokens.end (); ++viaPointIter)
			{
				tokenizer valueTokens (*viaPointIter, sep2);
				tokenizer::iterator valueIter = valueTokens.begin();

				// X:Y
				ls->addViaPoint (Point2D (Conversion::ToDouble (*valueIter), 
					Conversion::ToDouble (*(++valueIter))));
			}

			ls->setMetricOffset(metricOffset);
			ls->setIsArrival(isArrival);
			ls->setIsDeparture(isDeparture);
			ls->setRankInPath(rankInPath);
			ls->setLine(NULL);
			ls->setPhysicalStop(NULL);
			
			if (rows->getColumnIndex (LineStopTableSync::COL_SCHEDULEINPUT) != UNKNOWN_VALUE)
				ls->setScheduleInput(rows->getBool(LineStopTableSync::COL_SCHEDULEINPUT));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				util::RegistryKeyType fromPhysicalStopId (
					rows->getLongLong (LineStopTableSync::COL_PHYSICALSTOPID));

				util::RegistryKeyType lineId (rows->getLongLong (LineStopTableSync::COL_LINEID));
				JourneyPattern* line(LineTableSync::GetEditable (lineId, env, linkLevel).get());

				ls->setLine(line);
				ls->setPhysicalStop(PhysicalStopTableSync::GetEditable(fromPhysicalStopId, env, linkLevel).get());
				
				line->addEdge(*ls);
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<LineStopTableSync,LineStop>::Unlink(
			LineStop* obj
		){
			/// @todo line remove edge
		}

		template<> void SQLiteDirectTableSyncTemplate<LineStopTableSync,LineStop>::Save(
			LineStop* object,
			optional<SQLiteTransaction&> transaction
		){
			if(!object->getPhysicalStop()) throw Exception("Linestop save error. Missing physical stop");
			if(!object->getLine()) throw Exception("Linestop Save error. Missing line");

			stringstream viaPoints;
			bool first(true);
			BOOST_FOREACH(const geometry::Point2D* viaPoint, object->getViaPoints())
			{
				if(!first) viaPoints << ",";
				viaPoints << viaPoint->getX() << ":" << viaPoint->getY();
				first = false;
			}

			ReplaceQuery<LineStopTableSync> query(*object);
			query.addField(object->getPhysicalStop()->getKey());
			query.addField(object->getLine()->getKey());
			query.addField(object->getRankInPath());
			query.addField(object->isDepartureAllowed());
			query.addField(object->isArrivalAllowed());
			query.addField(object->getMetricOffset());
			query.addField(viaPoints.str());
			query.execute(transaction);
		}
	}

	namespace pt
	{
		LineStopTableSync::SearchResult LineStopTableSync::Search(
			Env& env,
			optional<RegistryKeyType> lineId,
			optional<RegistryKeyType> physicalStopId
			, int first /*= 0*/
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

			return LoadFromQuery(query, env, linkLevel);
		}



		void LineStopTableSync::InsertStop(
			LineStop& lineStop
		){
			stringstream query;
			query << "UPDATE " << TABLE.NAME << " SET " << COL_RANKINPATH << "=" << COL_RANKINPATH << "+1 WHERE " <<
				COL_LINEID << "=" << lineStop.getLine()->getKey() << " AND " << COL_RANKINPATH << ">=" << lineStop.getRankInPath();

			SQLiteTransaction transaction;
			transaction.add(query.str());

			Save(&lineStop, transaction);

			transaction.run();
		}



		void LineStopTableSync::RemoveStop(const LineStop& lineStop )
		{
			SQLiteTransaction transaction;

			Remove(lineStop.getKey(), transaction);

			stringstream query;
			query << "UPDATE " << TABLE.NAME << " SET " << COL_RANKINPATH << "=" << COL_RANKINPATH << "-1 WHERE " <<
				COL_LINEID << "=" << lineStop.getLine()->getKey() << " AND " << COL_RANKINPATH << ">" << lineStop.getRankInPath();
			transaction.add(query.str());

			transaction.run();
		}
	}
}
