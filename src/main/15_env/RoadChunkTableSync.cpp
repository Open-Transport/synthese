
/** RoadChunkTableSync class implementation.
	@file RoadChunkTableSync.cpp

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

#include <sstream>

#include "RoadChunkTableSync.h"
#include "RoadChunk.h"
#include "15_env/EnvModule.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace util
	{
		// template<> const std::string FactorableTemplate<SQLiteTableSync, RoadChunkTableSync>::FACTORY_KEY("");
	}
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<RoadChunk>::TABLE_NAME = "t014_road_chunks";
		template<> const int SQLiteTableSyncTemplate<RoadChunk>::TABLE_ID = 14;
		template<> const bool SQLiteTableSyncTemplate<RoadChunk>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<RoadChunk>::load(RoadChunk* object, const db::SQLiteResult& rows, int rowIndex/*=0*/ )
		{
			// ID
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowIndex, TABLE_COL_ID)));

			// Is departure
			bool isDeparture (Conversion::ToBool (
				rows.getColumn (rowIndex, RoadChunkTableSync::COL_ISDEPARTURE)));
			object->setIsDeparture(isDeparture);

			// Is arrival
			bool isArrival (Conversion::ToBool (
				rows.getColumn (rowIndex, RoadChunkTableSync::COL_ISARRIVAL)));
			object->setIsArrival(isArrival);

			// Rank in road
			int rankInRoad (
				Conversion::ToInt (rows.getColumn (rowIndex, RoadChunkTableSync::COL_RANKINPATH)));
			object->setRankInPath(rankInRoad);

			// From address
			uid fromAddressId (Conversion::ToLongLong (rows.getColumn (rowIndex, RoadChunkTableSync::COL_ADDRESSID)));
			shared_ptr<const Address> fromAddress = EnvModule::getAddresses ().get (fromAddressId);
			object->setFromAddress(fromAddress.get());

			// Via points
			object->clearViaPoints ();
			std::string viaPointsStr (rows.getColumn (rowIndex, RoadChunkTableSync::COL_VIAPOINTS));
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep1 (",");
			boost::char_separator<char> sep2 (":");
			tokenizer viaPointsTokens (viaPointsStr, sep1);
			for (tokenizer::iterator viaPointIter = viaPointsTokens.begin();
				viaPointIter != viaPointsTokens.end (); ++viaPointIter)
			{
				tokenizer valueTokens (*viaPointIter, sep2);
				tokenizer::iterator valueIter = valueTokens.begin();

				// X:Y
				object->addViaPoint (synthese::env::Point (Conversion::ToDouble (*valueIter), 
					Conversion::ToDouble (*(++valueIter))));
			}
		}

		template<> void SQLiteTableSyncTemplate<RoadChunk>::save(RoadChunk* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId(1,1));	/// @todo Use grid ID
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		const std::string RoadChunkTableSync::COL_ADDRESSID ("address_id");
		const std::string RoadChunkTableSync::COL_RANKINPATH ("rank_in_path");
		const std::string RoadChunkTableSync::COL_ISDEPARTURE ("is_departure");
		const std::string RoadChunkTableSync::COL_ISARRIVAL ("is_arrival");
		const std::string RoadChunkTableSync::COL_VIAPOINTS ("via_points");  // list of ids

		RoadChunkTableSync::RoadChunkTableSync()
			: SQLiteTableSyncTemplate<RoadChunk>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_ADDRESSID, "INTEGER", false);
			addTableColumn (COL_RANKINPATH, "INTEGER", false);
			addTableColumn (COL_ISDEPARTURE, "BOOLEAN", false);
			addTableColumn (COL_ISARRIVAL, "BOOLEAN", false);
			addTableColumn (COL_VIAPOINTS, "TEXT", true);
		}

		void RoadChunkTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getRoadChunks().contains(id))
				{
					load(EnvModule::getRoadChunks().getUpdateable(id).get(), rows, i);
				}
				else
				{
					shared_ptr<RoadChunk> object(new RoadChunk);
					load(object.get(), rows, i);
					EnvModule::getRoadChunks().add(object);

					uid fromAddressId (Conversion::ToLongLong (rows.getColumn (i, RoadChunkTableSync::COL_ADDRESSID)));
					shared_ptr<const Address> fromAddress = EnvModule::getAddresses ().get (fromAddressId);
					EnvModule::getRoads ().getUpdateable(fromAddress->getRoad ()->getId ())->addEdge (object.get());
				}
			}
		}
		
		void RoadChunkTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getRoadChunks().contains(id))
				{
					load(EnvModule::getRoadChunks().getUpdateable(id).get(), rows, i);
				}
			}
		}

		void RoadChunkTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getRoadChunks().contains(id))
				{
					EnvModule::getRoadChunks().remove(id);
				}
			}
		}

		vector<shared_ptr<RoadChunk> > RoadChunkTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			/// @todo Fill Where criteria
			// if (!name.empty())
			// 	query << " AND " << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
				;
			//if (orderByName)
			//	query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = DBModule::GetSQLite()->execQuery(query.str());
				vector<shared_ptr<RoadChunk> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<RoadChunk> object(new RoadChunk);
					load(object.get(), result, i);
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
