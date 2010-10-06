
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

#include "DBModule.h"
#include "SQLite.h"
#include "StopAreaTableSync.hpp"
#include "CityTableSync.h"
#include "StopPointTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "LineStopTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "StopArea.hpp"
#include "CommercialLine.h"
#include "AdvancedSelectTableSync.h"
#include "DisplayScreenTableSync.h"
#include "Env.h"
#include "Conversion.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace pt;
	using namespace security;
	using namespace util;
	using namespace geography;

	namespace departure_boards
	{
		std::vector<shared_ptr<ConnectionPlaceWithBroadcastPoint> > searchConnectionPlacesWithBroadcastPoints(
			Env& env,
			const security::RightsOfSameClassMap& rights 
			, bool totalControl 
			, RightLevel neededLevel
			, std::string cityName /*= ""*/
			, std::string placeName /*= ""*/
			, BroadcastPointsPresence bpPresence 
			, optional<RegistryKeyType> lineId 
			, boost::optional<std::size_t> number
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
					<< ",(SELECT COUNT(b." << TABLE_COL_ID << ") FROM " << DisplayScreenTableSync::TABLE.NAME << " AS b WHERE b." << DisplayScreenTableSync::COL_PLACE_ID << "=p." << TABLE_COL_ID << ") AS bc"
					<< ",(SELECT COUNT(s." << TABLE_COL_ID << ") FROM " << DisplayScreenCPUTableSync::TABLE.NAME << " AS s WHERE s." << DisplayScreenCPUTableSync::COL_PLACE_ID << "=p." << TABLE_COL_ID << ") AS cc"
				<< " FROM " // Tables
					<< StopAreaTableSync::TABLE.NAME << " AS p"
					<< " INNER JOIN " << CityTableSync::TABLE.NAME << " AS c ON c." << TABLE_COL_ID << "=p." << StopAreaTableSync::TABLE_COL_CITYID
					<< " INNER JOIN " << StopPointTableSync::TABLE.NAME << " AS ps ON " 	<< " ps." << StopPointTableSync::COL_PLACEID << "=p." << TABLE_COL_ID
					<< " INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS ls ON ps." << TABLE_COL_ID << "= ls." << LineStopTableSync::COL_PHYSICALSTOPID 
					<< " INNER JOIN " << JourneyPatternTableSync::TABLE.NAME << " as l ON l." << TABLE_COL_ID << "=ls." << LineStopTableSync::COL_LINEID;
			// Where	
			query << " WHERE 1 ";
			if (neededLevel > FORBIDDEN)
				query << " AND l." << JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID << " IN (" << CommercialLineTableSync::getSQLLinesList(rights, totalControl, neededLevel, false) << ")";
			if (lineId)
				query << " AND l." << JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID << "=" << *lineId;
			if (!cityName.empty())
				query << " AND c." << CityTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(cityName, false) << "%'";
			if (!placeName.empty())
				query << " AND p." << CityTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(placeName, false) << "%'";
			if (bpPresence != WITH_OR_WITHOUT_ANY_BROADCASTPOINT)
			{
				query << " AND bc+cc ";
				if (bpPresence == AT_LEAST_ONE_BROADCASTPOINT)
					query << ">0";
				if (bpPresence == NO_BROADCASTPOINT)
					query << "=0";
			}
			// Grouping
			query << " GROUP BY p." << TABLE_COL_ID;
			// Order
			if (orderByCity)
			{
				query << " ORDER BY c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC") << ",p."  << StopAreaTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			}
			else if (orderByName)
			{
				query << " ORDER BY p." << StopAreaTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			}
			else if (orderByNumber)
			{
				query << " ORDER BY bc" << (raisingOrder ? " ASC" : " DESC");
			}
			
			// Limits
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			try
			{
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				vector<shared_ptr<ConnectionPlaceWithBroadcastPoint> > objects;
				while (rows->next ())
				{
					shared_ptr<ConnectionPlaceWithBroadcastPoint> object(new ConnectionPlaceWithBroadcastPoint);
					object->broadCastPointsNumber = rows->getInt ("bc");
					object->cpuNumber = rows->getInt("cc");
					if(env.getRegistry<StopArea>().contains(rows->getKey()))
					{
						object->place = static_pointer_cast<NamedPlace, StopArea>(env.getEditable<StopArea>(rows->getKey()));
					}
					else
					{
						StopArea* stopArea(new StopArea(rows->getKey()));
						object->place.reset(stopArea);
						env.getEditableRegistry<StopArea>().add(static_pointer_cast<StopArea,NamedPlace>(object->place));
						StopAreaTableSync::Load(stopArea, rows, env);
					}
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

		
		
		std::vector<shared_ptr<const CommercialLine> > getCommercialLineWithBroadcastPoints(
			Env& env,
			boost::optional<std::size_t> number,
			int first/*=0*/
		){
			stringstream query;
			query << " SELECT "
				<< "c." << TABLE_COL_ID << " AS " << TABLE_COL_ID
				<< " FROM " << CommercialLineTableSync::TABLE.NAME << " AS c "
				<< " INNER JOIN " << JourneyPatternTableSync::TABLE.NAME << " AS l ON l." << JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID << "=c." << TABLE_COL_ID
				<< " INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS s ON s." << LineStopTableSync::COL_LINEID << "=l." << TABLE_COL_ID
				<< " INNER JOIN " << StopPointTableSync::TABLE.NAME << " AS p ON p." << TABLE_COL_ID << "=s." << LineStopTableSync::COL_PHYSICALSTOPID
				<< " INNER JOIN " << DisplayScreenTableSync::TABLE.NAME << " AS b ON b." << DisplayScreenTableSync::COL_PLACE_ID << "=p." << StopPointTableSync::COL_PLACEID
				<< " GROUP BY c." << TABLE_COL_ID
				<< " ORDER BY c." << CommercialLineTableSync::COL_SHORT_NAME;
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			try
			{
				SQLite* sqlite = DBModule::GetSQLite();
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<const CommercialLine> > objects;
				while (rows->next ())
				{
					objects.push_back(CommercialLineTableSync::Get(
							rows->getLongLong (TABLE_COL_ID),
							env
					)	);
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
