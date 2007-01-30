
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
#include "15_env/EnvModule.h"

#include "34_departures_table/AdvancedSelectTableSync.h"
#include "34_departures_table/BroadcastPointTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace env;

	namespace departurestable
	{

		std::vector<ConnectionPlaceWithBroadcastPoint> searchConnectionPlacesWithBroadcastPoints( 
			std::string cityName /*= ""*/, std::string placeName /*= ""*/, int bpNumbers /*= UNKNOWN_VALUE*/
			, uid lineId /*= UNKNOWN_VALUE*/, int number/*=UNKNOWN_VALUE*/, int first/*=0*/ )
		{
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
			if (bpNumbers != UNKNOWN_VALUE)
			{
				query << " AND COUNT(b." << TABLE_COL_ID << ")";
				if (bpNumbers == 1)
					query << ">0";
				if (bpNumbers == 0)
					query << "=0";
			}
			query << " GROUP BY p." << TABLE_COL_ID;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<ConnectionPlaceWithBroadcastPoint> objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					ConnectionPlaceWithBroadcastPoint object;
					object.broadCastPointsNumber = Conversion::ToInt(result.getColumn(i, "bc"));
					object.place = EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(result.getColumn(i, "p." + TABLE_COL_ID)));
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
