
/** AdvancedSelectTableSync class implementation.
	@file AdvancedSelectTableSync.cpp

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

#include "02_db/DBModule.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/ConnectionPlaceTableSync.h"
#include "15_env/CityTableSync.h"
#include "15_env/PhysicalStopTableSync.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/LineStopTableSync.h"
#include "15_env/LineTableSync.h"
#include "15_env/EnvModule.h"

#include "34_departures_table/AdvancedSelectTableSync.h"
#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/BroadcastPointTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace env;

	namespace departurestable
	{

		std::vector<shared_ptr<ConnectionPlaceWithBroadcastPoint> > searchConnectionPlacesWithBroadcastPoints( 
			std::string cityName /*= ""*/
			, std::string placeName /*= ""*/
			, BroadcastPointsPresence bpPresence /*= UNKNOWN_VALUE*/
			, uid lineId /*= UNKNOWN_VALUE*/
			, int number/*=UNKNOWN_VALUE*/
			, int first/*=0*/ 
			, bool orderByCity
			, bool orderByName
			, bool orderByNumber
			, bool raisingOrder
		){
			stringstream query;
			query << " SELECT "
					<< "p." << TABLE_COL_ID
					<< ",COUNT(b." << TABLE_COL_ID << ") AS bc"
				<< " FROM " << ConnectionPlaceTableSync::TABLE_NAME << " AS p"
					<< " INNER JOIN " << CityTableSync::TABLE_NAME << " AS c ON c." << TABLE_COL_ID << "=p." << ConnectionPlaceTableSync::TABLE_COL_CITYID
					<< " LEFT JOIN " << BroadcastPointTableSync::TABLE_NAME << " AS b ON b." << BroadcastPointTableSync::TABLE_COL_PLACE_ID << "=p." << TABLE_COL_ID
				<< " WHERE "
					<< "c." << CityTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(cityName, false) << "%'"
					<< " AND p." << CityTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(placeName, false) << "%'";
			query << " GROUP BY p." << TABLE_COL_ID;
			if (bpPresence != WITH_OR_WITHOU_ANY_BROADCASTPOINT)
			{
				query << " HAVING COUNT(b." << TABLE_COL_ID << ")";
				if (bpPresence == AT_LEAST_ONE_BROADCASTPOINT)
					query << ">0";
				if (bpPresence == NO_BROADCASTPOINT)
					query << "=0";
			}
			if (orderByCity)
				query << " ORDER BY c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC") << ",b."  << BroadcastPointTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByName)
				query << " ORDER BY b."  << BroadcastPointTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByNumber)
				query << " ORDER BY COUNT(b."  << TABLE_COL_ID << ")" << (raisingOrder ? " ASC" : " DESC");

			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<ConnectionPlaceWithBroadcastPoint> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<ConnectionPlaceWithBroadcastPoint> object(new ConnectionPlaceWithBroadcastPoint);
					object->broadCastPointsNumber = Conversion::ToInt(result.getColumn(i, "bc"));
					object->place = EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(result.getColumn(i, "p." + TABLE_COL_ID)));
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}

		vector<PhysicalStopAndBroadcastPoint> getConnectionPlacePhysicalStopsAndBroadcastPoints( uid placeId, int number/*=UNKNOWN_VALUE*/, int first/*=0*/ )
		{
			stringstream query;
			query << " SELECT "
				<< "p." << TABLE_COL_ID << " AS pid"
				<< ",b." << TABLE_COL_ID << " AS bid"
				<< " FROM " << PhysicalStopTableSync::TABLE_NAME << " AS p"
				<< " LEFT JOIN " << BroadcastPointTableSync::TABLE_NAME << " AS b ON b." << BroadcastPointTableSync::TABLE_COL_PHYSICAL_STOP_ID << "=p." << TABLE_COL_ID
				<< " WHERE "
				<< "p." << PhysicalStopTableSync::COL_PLACEID << "=" << Conversion::ToString(placeId);
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<PhysicalStopAndBroadcastPoint> objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					PhysicalStopAndBroadcastPoint object;
					object.stop = EnvModule::getPhysicalStops().get(Conversion::ToLongLong(result.getColumn(i, "pid")));
					object.bp = (result.getColumn(i, "bid") != "")
						? BroadcastPointTableSync::get(Conversion::ToLongLong(result.getColumn(i, "bid")))
						: shared_ptr<BroadcastPoint>();
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}

		}

		std::vector<PhysicalStopAndBroadcastPoint> getConnectionPlaceBroadcastPointsAndPhysicalStops( uid placeId, boost::logic::tribool withPhysical, int number/*=UNKNOWN_VALUE*/, int first/*=0*/ )
		{
			stringstream query;
			query << " SELECT "
				<< "p." << TABLE_COL_ID << " AS pid"
				<< ",b." << TABLE_COL_ID << " AS bid"
				<< " FROM " << BroadcastPointTableSync::TABLE_NAME << " AS b"
				<< " LEFT JOIN " << PhysicalStopTableSync::TABLE_NAME << " AS p ON b." << BroadcastPointTableSync::TABLE_COL_PHYSICAL_STOP_ID << "=p." << TABLE_COL_ID
				<< " WHERE "
				<< "b." << BroadcastPointTableSync::TABLE_COL_PLACE_ID << "=" << Conversion::ToString(placeId);
			if (withPhysical == true)
				query << " AND p." << TABLE_COL_ID << " IS NOT NULL";
			if (withPhysical == false)
				query << " AND p." << TABLE_COL_ID << " IS NULL";
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<PhysicalStopAndBroadcastPoint> objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					PhysicalStopAndBroadcastPoint object;
					object.stop = (result.getColumn(i, "pid") != "")
						? EnvModule::getPhysicalStops().get(Conversion::ToLongLong(result.getColumn(i, "pid")))
						: boost::shared_ptr<const PhysicalStop>();
					object.bp = BroadcastPointTableSync::get(Conversion::ToLongLong(result.getColumn(i, "bid")));
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}

		std::vector<shared_ptr<const CommercialLine> > getCommercialLineWithBroadcastPoints( int number/*=UNKNOWN_VALUE*/, int first/*=0*/ )
		{
			stringstream query;
			query << " SELECT "
				<< "c." << TABLE_COL_ID << " AS " << TABLE_COL_ID
				<< " FROM " << CommercialLineTableSync::TABLE_NAME << " AS c "
				<< " INNER JOIN " << LineTableSync::TABLE_NAME << " AS l ON l." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=c." << TABLE_COL_ID
				<< " INNER JOIN " << LineStopTableSync::TABLE_NAME << " AS s ON s." << LineStopTableSync::COL_LINEID << "=l." << TABLE_COL_ID
				<< " INNER JOIN " << PhysicalStopTableSync::TABLE_NAME << " AS p ON p." << TABLE_COL_ID << "=s." << LineStopTableSync::COL_PHYSICALSTOPID
				<< " INNER JOIN " << BroadcastPointTableSync::TABLE_NAME << " AS b ON b." << BroadcastPointTableSync::TABLE_COL_PLACE_ID << "=p." << PhysicalStopTableSync::COL_PLACEID
				<< " GROUP BY c." << TABLE_COL_ID
				<< " ORDER BY c." << CommercialLineTableSync::COL_SHORT_NAME;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<const CommercialLine> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					objects.push_back(EnvModule::getCommercialLines().get(Conversion::ToLongLong(result.getColumn(i, TABLE_COL_ID))));
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
