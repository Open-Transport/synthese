

/** LineStopTableSync class implementation.
	@file LineStopTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "LineStopTableSync.h"

#include "15_env/LineTableSync.h"
#include "15_env/PhysicalStopTableSync.h"

#include <sstream>

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "06_geometry/Point2D.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace geometry;

	template<> const string util::FactorableTemplate<SQLiteTableSync,LineStopTableSync>::FACTORY_KEY("15.57.01 Line stops");

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<LineStopTableSync>::TABLE_NAME = "t010_line_stops";
		template<> const int SQLiteTableSyncTemplate<LineStopTableSync>::TABLE_ID = 10;
		template<> const bool SQLiteTableSyncTemplate<LineStopTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<LineStopTableSync,LineStop>::load(LineStop* ls, const db::SQLiteResultSPtr& rows )
		{
			uid id (rows->getLongLong (TABLE_COL_ID));

			int rankInPath (
			    rows->getInt (LineStopTableSync::COL_RANKINPATH));
			
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

			ls->setKey(id);
			ls->setMetricOffset(metricOffset);
			ls->setIsArrival(isArrival);
			ls->setIsDeparture(isDeparture);
			ls->setRankInPath(rankInPath);
			
			if (rows->getColumnIndex (LineStopTableSync::COL_SCHEDULEINPUT) != UNKNOWN_VALUE)
				ls->setScheduleInput(rows->getBool(LineStopTableSync::COL_SCHEDULEINPUT));
		}

		template<> void SQLiteDirectTableSyncTemplate<LineStopTableSync,LineStop>::_link(LineStop* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{

			uid fromPhysicalStopId (
				rows->getLongLong (LineStopTableSync::COL_PHYSICALSTOPID));

			uid lineId (rows->getLongLong (LineStopTableSync::COL_LINEID));
			Line* line(LineTableSync::GetUpdateable (lineId,obj,temporary));

			obj->setPhysicalStop(PhysicalStopTableSync::GetUpdateable(fromPhysicalStopId,obj,temporary));
			obj->setLine(line);

			if (temporary == GET_REGISTRY)
				line->addEdge(obj);
		}

		template<> void SQLiteDirectTableSyncTemplate<LineStopTableSync,LineStop>::_unlink(LineStop* obj)
		{
			/// @todo line remove edge

			obj->setLine(NULL);
			obj->setPhysicalStop(NULL);
		}

		template<> void SQLiteDirectTableSyncTemplate<LineStopTableSync,LineStop>::save(LineStop* object)
		{
/*			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());	/// @todo Use grid ID
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
*/		}

	}

	namespace env
	{
		const std::string LineStopTableSync::COL_PHYSICALSTOPID ("physical_stop_id");
		const std::string LineStopTableSync::COL_LINEID ("line_id");
		const std::string LineStopTableSync::COL_RANKINPATH ("rank_in_path");
		const std::string LineStopTableSync::COL_ISDEPARTURE ("is_departure");
		const std::string LineStopTableSync::COL_ISARRIVAL ("is_arrival");
		const std::string LineStopTableSync::COL_METRICOFFSET ("metric_offset");
		const std::string LineStopTableSync::COL_SCHEDULEINPUT ("schedule_input");
		const std::string LineStopTableSync::COL_VIAPOINTS ("via_points");

		LineStopTableSync::LineStopTableSync()
			: SQLiteRegistryTableSyncTemplate<LineStopTableSync,LineStop>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_PHYSICALSTOPID, "INTEGER", false);
			addTableColumn (COL_LINEID, "INTEGER", false);
			addTableColumn (COL_RANKINPATH, "INTEGER", false);
			addTableColumn (COL_ISDEPARTURE, "BOOLEAN", false);
			addTableColumn (COL_ISARRIVAL, "BOOLEAN", false);
			addTableColumn (COL_METRICOFFSET, "DOUBLE", true);
			addTableColumn (COL_VIAPOINTS, "TEXT", true);

			addTableIndex(COL_LINEID);
			addTableIndex(COL_PHYSICALSTOPID);
		}


		std::vector<shared_ptr<LineStop> > LineStopTableSync::Search(
			uid lineId
			, uid physicalStopId
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByRank
			, bool raisingOrder
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			if (lineId != UNKNOWN_VALUE)
				query << " AND " << COL_LINEID << "=" << lineId;
			if (physicalStopId != UNKNOWN_VALUE)
				query << " AND " << COL_PHYSICALSTOPID << "=" << physicalStopId;
			if (orderByRank)
				query << " ORDER BY " << COL_RANKINPATH << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<LineStop> > objects;
				while (rows->next ())
				{
					shared_ptr<LineStop> object(new LineStop());
					load(object.get(), rows);
					link(object.get(), rows, GET_TEMPORARY);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
