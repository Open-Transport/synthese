
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

#include "AdvancedSelectTableSync.h"

#include "15_env/LineStopTableSync.h"
#include "15_env/LineTableSync.h"
#include "15_env/PhysicalStopTableSync.h"
#include "15_env/CommercialLineTableSync.h"

#include "02_db/DBModule.h"

#include <sstream>

using namespace std;

namespace synthese
{
	using namespace db;

	namespace env
	{


		bool isPlaceServedByCommercialLine(uid lineId, uid placeId)
		{
			stringstream query;
			query
				<< "SELECT ls." << TABLE_COL_ID << " FROM "
				<< LineStopTableSync::TABLE_NAME << " AS ls "
				<< " INNER JOIN " << PhysicalStopTableSync::TABLE_NAME << " AS p ON p." << TABLE_COL_ID << "=ls." << LineStopTableSync::COL_PHYSICALSTOPID
				<< " INNER JOIN " << LineTableSync::TABLE_NAME << " AS l ON l." << TABLE_COL_ID << "=ls." << LineStopTableSync::COL_LINEID
				<< " WHERE "
				<< "p." << PhysicalStopTableSync::COL_PLACEID << "=" << placeId
				<< " AND l." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << lineId
				<< " LIMIT 1";
			SQLiteResultSPtr rows(DBModule::GetSQLite()->execQuery(query.str()));
			return rows->next();
		}

		bool isPlaceServedByNetwork( uid networkId, uid placeId )
		{
			stringstream query;
			query
				<< "SELECT ls." << TABLE_COL_ID << " FROM "
				<< LineStopTableSync::TABLE_NAME << " AS ls "
				<< " INNER JOIN " << PhysicalStopTableSync::TABLE_NAME << " AS p ON p." << TABLE_COL_ID << "=ls." << LineStopTableSync::COL_PHYSICALSTOPID
				<< " INNER JOIN " << LineTableSync::TABLE_NAME << " AS l ON l." << TABLE_COL_ID << "=ls." << LineStopTableSync::COL_LINEID
				<< " INNER JOIN " << CommercialLineTableSync::TABLE_NAME << " AS c ON c." << TABLE_COL_ID << "=l." << LineTableSync::COL_COMMERCIAL_LINE_ID
				<< " WHERE "
				<< "p." << PhysicalStopTableSync::COL_PLACEID << "=" << placeId
				<< " AND c." << CommercialLineTableSync::COL_NETWORK_ID << "=" << networkId
				<< " LIMIT 1";
			SQLiteResultSPtr rows(DBModule::GetSQLite()->execQuery(query.str()));
			return rows->next();
		}
	}
}
