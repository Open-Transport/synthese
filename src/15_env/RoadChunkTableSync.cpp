
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

#include "AddressTableSync.h"
#include "RoadTableSync.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "LinkException.h"

#include "Conversion.h"

#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace geometry;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, RoadChunkTableSync>::FACTORY_KEY("15.60.01 Road chunks");
	}

	namespace env
	{
		const string RoadChunkTableSync::COL_ADDRESSID ("address_id");
		const string RoadChunkTableSync::COL_RANKINPATH ("rank_in_path");
		const string RoadChunkTableSync::COL_ISDEPARTURE ("is_departure");
		const string RoadChunkTableSync::COL_ISARRIVAL ("is_arrival");
		const string RoadChunkTableSync::COL_VIAPOINTS ("via_points");  // list of ids
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<RoadChunkTableSync>::TABLE(
			"t014_road_chunks"
			);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<RoadChunkTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_ADDRESSID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_RANKINPATH, SQL_INTEGER),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_ISDEPARTURE, SQL_BOOLEAN),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_ISARRIVAL, SQL_BOOLEAN),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_VIAPOINTS, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<RoadChunkTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<RoadChunkTableSync,RoadChunk>::Load(
			RoadChunk* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    // Is departure
		    bool isDeparture (rows->getBool (RoadChunkTableSync::COL_ISDEPARTURE));
		    object->setIsDeparture(isDeparture);

		    // Is arrival
		    bool isArrival (rows->getBool (RoadChunkTableSync::COL_ISARRIVAL));
		    object->setIsArrival(isArrival);

		    // Rank in road
		    int rankInRoad (rows->getInt (RoadChunkTableSync::COL_RANKINPATH));
		    object->setRankInPath(rankInRoad);
		    
		    // Via points
		    object->clearViaPoints ();
		    string viaPointsStr (rows->getText (RoadChunkTableSync::COL_VIAPOINTS));
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
				object->addViaPoint (Point2D (Conversion::ToDouble (*valueIter), 
						      Conversion::ToDouble (*(++valueIter))));
		    }

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// From address
				uid fromAddressId (rows->getLongLong (RoadChunkTableSync::COL_ADDRESSID));

				try
				{
					Address* fromAddress(AddressTableSync::GetEditable (fromAddressId, env, linkLevel).get());
					object->setParentPath(fromAddress->getRoad());
					object->setFromAddress(fromAddress);

					if (!fromAddress->getRoad())
						throw LinkException<RoadChunkTableSync>(object->getKey(), RoadChunkTableSync::COL_ADDRESSID, Exception("Address without road"));
					const_cast<Road*>(fromAddress->getRoad())->addEdge (object);
					
				}
				catch (ObjectNotFoundException<Address>& e)
				{
					throw LinkException<RoadChunkTableSync>(object->getKey(), RoadChunkTableSync::COL_ADDRESSID, e);
				}
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<RoadChunkTableSync,RoadChunk>::Unlink(
			RoadChunk* obj
		){
		}

	    
	    template<> void SQLiteDirectTableSyncTemplate<RoadChunkTableSync,RoadChunk>::Save(RoadChunk* object)
	    {
		SQLite* sqlite = DBModule::GetSQLite();
		stringstream query;
		if (object->getKey() <= 0)
		    object->setKey(getId());	/// @todo Use grid ID
		
		query
		    << " REPLACE INTO " << TABLE.NAME << " VALUES("
		    << Conversion::ToString(object->getKey())
		    /// @todo fill other fields separated by ,
		    << ")";
		sqlite->execUpdate(query.str());
	    }

	}

	namespace env
	{
		RoadChunkTableSync::RoadChunkTableSync()
			: SQLiteRegistryTableSyncTemplate<RoadChunkTableSync,RoadChunk>()
		{
		}



	    void RoadChunkTableSync::Search(
			Env& env,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
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

			LoadFromQuery(query.str(), env, linkLevel);
	    }
	}
}
