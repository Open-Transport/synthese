
/** AdvancedSelectTableSync class implementation.
	@file 54_departure_boards/AdvancedSelectTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "DBModule.h"
#include "CalendarTemplate.h"
#include "CityTableSync.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "Conversion.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplayScreenTableSync.h"
#include "Env.h"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ReservationContact.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TreeFolderUpNode.hpp"

#include <sstream>
#include <boost/optional/optional_io.hpp>

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
		std::vector<boost::shared_ptr<ConnectionPlaceWithBroadcastPoint> > searchConnectionPlacesWithBroadcastPoints(
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
					<< ",(SELECT COUNT(b." << TABLE_COL_ID << ") FROM " << DisplayScreenTableSync::TABLE.NAME << " AS b WHERE b." << BroadCastPoint::FIELD.name << "=p." << TABLE_COL_ID << ") AS bc"
					<< ",(SELECT COUNT(s." << TABLE_COL_ID << ") FROM " << DisplayScreenCPUTableSync::TABLE.NAME << " AS s WHERE s." << DisplayScreenCPUTableSync::COL_PLACE_ID << "=p." << TABLE_COL_ID << ") AS cc"
				<< " FROM " // Tables
					<< StopAreaTableSync::TABLE.NAME << " AS p"
					<< " INNER JOIN " << CityTableSync::TABLE.NAME << " AS c ON c." << TABLE_COL_ID << "=p." << pt::CityId::FIELD.name
					<< " INNER JOIN " << StopPointTableSync::TABLE.NAME << " AS ps ON " 	<< " ps." << ConnectionPlace::FIELD.name << "=p." << TABLE_COL_ID
					<< " INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS ls ON ps." << TABLE_COL_ID << "= ls." << LineNode::FIELD.name
					<< " INNER JOIN " << JourneyPatternCommercialLine::FIELD.name << " as l ON l." << TABLE_COL_ID << "=ls." << Line::FIELD.name;
			// Where
			query << " WHERE 1 ";
			if (neededLevel > FORBIDDEN)
				query << " AND l." << JourneyPatternCommercialLine::FIELD.name << " IN (" << CommercialLineTableSync::getSQLLinesList(rights, totalControl, neededLevel, false) << ")";
			if (lineId)
				query << " AND l." << JourneyPatternCommercialLine::FIELD.name << "=" << *lineId;
			if (!cityName.empty())
				query << " AND c." << CityTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToDBString(cityName, false) << "%'";
			if (!placeName.empty())
				query << " AND p." << CityTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToDBString(placeName, false) << "%'";

			if (bpPresence != WITH_OR_WITHOUT_ANY_BROADCASTPOINT && DBModule::GetDB()->isBackend(DB::MYSQL_BACKEND))
			{
				// In mysql, HAVING clause should be after GROUP clause
				// Grouping
				query << " GROUP BY p." << TABLE_COL_ID;
				query << " HAVING bc+cc ";
				if (bpPresence == AT_LEAST_ONE_BROADCASTPOINT)
					query << ">0";
				if (bpPresence == NO_BROADCASTPOINT)
					query << "=0";
			}
			else if (bpPresence != WITH_OR_WITHOUT_ANY_BROADCASTPOINT)
			{
				// In sqlite, WHERE clause works on result of SELECT COUNT AS
				query << " AND bc+cc ";
				if (bpPresence == AT_LEAST_ONE_BROADCASTPOINT)
					query << ">0";
				if (bpPresence == NO_BROADCASTPOINT)
					query << "=0";
				// Grouping
				query << " GROUP BY p." << TABLE_COL_ID;
			}
			// Order
			if (orderByCity)
			{
				query << " ORDER BY c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC") << ",p."  << pt::CityId::FIELD.name << (raisingOrder ? " ASC" : " DESC");
			}
			else if (orderByName)
			{
				query << " ORDER BY p." << SimpleObjectFieldDefinition<Name>::FIELD.name << (raisingOrder ? " ASC" : " DESC");
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
				DBResultSPtr rows = DBModule::GetDB()->execQuery(query.str());
				vector<boost::shared_ptr<ConnectionPlaceWithBroadcastPoint> > objects;
				while (rows->next ())
				{
					boost::shared_ptr<ConnectionPlaceWithBroadcastPoint> object(new ConnectionPlaceWithBroadcastPoint);
					object->broadCastPointsNumber = rows->getInt("bc");
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
			catch(DBException& e)
			{
				throw Exception(e.getMessage());
			}
		}



		std::vector<boost::shared_ptr<const CommercialLine> > getCommercialLineWithBroadcastPoints(
			Env& env,
			boost::optional<std::size_t> number,
			int first/*=0*/
		){
			stringstream query;
			query << " SELECT "
				<< "c." << TABLE_COL_ID << " AS " << TABLE_COL_ID
				<< " FROM " << CommercialLineTableSync::TABLE.NAME << " AS c "
				<< " INNER JOIN " << JourneyPatternTableSync::TABLE.NAME << " AS l ON l." << JourneyPatternCommercialLine::FIELD.name << "=c." << TABLE_COL_ID
				<< " INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS s ON s." << Line::FIELD.name << "=l." << TABLE_COL_ID
				<< " INNER JOIN " << StopPointTableSync::TABLE.NAME << " AS p ON p." << TABLE_COL_ID << "=s." << LineNode::FIELD.name
				<< " INNER JOIN " << DisplayScreenTableSync::TABLE.NAME << " AS b ON b." << BroadCastPoint::FIELD.name << "=p." << ConnectionPlace::FIELD.name
				<< " GROUP BY c." << TABLE_COL_ID
				<< " ORDER BY c." << ShortName::FIELD.name;
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			try
			{
				DB* db = DBModule::GetDB();
				DBResultSPtr rows = db->execQuery(query.str());
				vector<boost::shared_ptr<const CommercialLine> > objects;
				while (rows->next ())
				{
					objects.push_back(CommercialLineTableSync::Get(
							rows->getLongLong (TABLE_COL_ID),
							env
					)	);
				}
				return objects;
			}
			catch(DBException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
