
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
#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/CommercialLine.h"

#include "34_departures_table/AdvancedSelectTableSync.h"
#include "34_departures_table/DisplayScreenTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace env;
	using namespace security;

	namespace departurestable
	{

		std::vector<shared_ptr<ConnectionPlaceWithBroadcastPoint> > searchConnectionPlacesWithBroadcastPoints( 
			const security::RightsOfSameClassMap& rights 
			, bool totalControl 
			, RightLevel neededLevel
			, std::string cityName /*= ""*/
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
			// Columns
			query
				<< " SELECT "
					<< "p.*"
					<< ",c." << CityTableSync::TABLE_COL_NAME << " AS city_name"
					<< ",(SELECT COUNT(b." << TABLE_COL_ID << ") FROM " << DisplayScreenTableSync::TABLE_NAME << " AS b WHERE b." << DisplayScreenTableSync::COL_PLACE_ID << "=p." << TABLE_COL_ID << ") AS bc"
				<< " FROM " // Tables
					<< ConnectionPlaceTableSync::TABLE_NAME << " AS p"
					<< " INNER JOIN " << CityTableSync::TABLE_NAME << " AS c ON c." << TABLE_COL_ID << "=p." << ConnectionPlaceTableSync::TABLE_COL_CITYID
					<< " INNER JOIN " << PhysicalStopTableSync::TABLE_NAME << " AS ps ON " 	<< " ps." << PhysicalStopTableSync::COL_PLACEID << "=p." << TABLE_COL_ID
					<< " INNER JOIN " << LineStopTableSync::TABLE_NAME << " AS ls ON ps." << TABLE_COL_ID << "= ls." << LineStopTableSync::COL_PHYSICALSTOPID 
					<< " INNER JOIN " << LineTableSync::TABLE_NAME << " as l ON l." << TABLE_COL_ID << "=ls." << LineStopTableSync::COL_LINEID;
			// Where	
			query << " WHERE 1 ";
			if (neededLevel > FORBIDDEN)
				query << " AND l." << LineTableSync::COL_COMMERCIAL_LINE_ID << " IN (" << CommercialLineTableSync::getSQLLinesList(rights, totalControl, neededLevel, false) << ")";
			if (lineId != UNKNOWN_VALUE)
				query << " AND l." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << lineId;
			if (!cityName.empty())
				query << " AND c." << CityTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(cityName, false) << "%'";
			if (!placeName.empty())
				query << " AND p." << CityTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(placeName, false) << "%'";
			if (bpPresence != WITH_OR_WITHOUT_ANY_BROADCASTPOINT)
			{
				query << " AND bc ";
				if (bpPresence == AT_LEAST_ONE_BROADCASTPOINT)
					query << ">0";
				if (bpPresence == NO_BROADCASTPOINT)
					query << "=0";
			}
			// Grouping
			query << " GROUP BY p." << TABLE_COL_ID;
			// Order
			if (orderByCity)
				query << " ORDER BY c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC") << ",p."  << ConnectionPlaceTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByName)
				query << " ORDER BY p." << ConnectionPlaceTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByNumber)
				query << " ORDER BY bc" << (raisingOrder ? " ASC" : " DESC");
			// Limits
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				vector<shared_ptr<ConnectionPlaceWithBroadcastPoint> > objects;
				while (rows->next ())
				{
					shared_ptr<ConnectionPlaceWithBroadcastPoint> object(new ConnectionPlaceWithBroadcastPoint);
					object->broadCastPointsNumber = rows->getInt ("bc");
					object->place.reset(new PublicTransportStopZoneConnectionPlace);
					ConnectionPlaceTableSync::load(object->place.get(), rows);
					object->cityName = rows->getText ("city_name");
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
				<< " INNER JOIN " << DisplayScreenTableSync::TABLE_NAME << " AS b ON b." << DisplayScreenTableSync::COL_PLACE_ID << "=p." << PhysicalStopTableSync::COL_PLACEID
				<< " GROUP BY c." << TABLE_COL_ID
				<< " ORDER BY c." << CommercialLineTableSync::COL_SHORT_NAME;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteHandle* sqlite = DBModule::GetSQLite();
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<const CommercialLine> > objects;
				while (rows->next ())
				{
					objects.push_back(CommercialLine::Get(
							  rows->getLongLong (TABLE_COL_ID)));
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
