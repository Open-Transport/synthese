
/** DisplayScreenTableSync class implementation.
	@file DisplayScreenTableSync.cpp

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

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "15_env/EnvModule.h"

#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/BroadcastPointTableSync.h"
#include "34_departures_table/DeparturesTableModule.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace departurestable;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<DisplayScreen>::TABLE_NAME = "t041_display_screens";
		template<> const int SQLiteTableSyncTemplate<DisplayScreen>::TABLE_ID = 41;
		template<> const bool SQLiteTableSyncTemplate<DisplayScreen>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<DisplayScreen>::load(DisplayScreen* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			object->setLocalization(DeparturesTableModule::getBroadcastPoints().get(Conversion::ToLongLong(rows.getColumn(rowId, DisplayScreenTableSync::COL_BROADCAST_POINT_ID))));
			object->setLocalizationComment(rows.getColumn(rowId, DisplayScreenTableSync::COL_BROADCAST_POINT_COMMENT));
			object->setType(DeparturesTableModule::getDisplayTypes().get(Conversion::ToLongLong(rows.getColumn(rowId, DisplayScreenTableSync::COL_TYPE_ID))).get());
			object->setWiringCode(Conversion::ToInt(rows.getColumn(rowId, DisplayScreenTableSync::COL_WIRING_CODE)));
			object->setTitle(rows.getColumn(rowId, DisplayScreenTableSync::COL_TITLE));
			object->setBlinkingDelay(Conversion::ToInt(rows.getColumn(rowId, DisplayScreenTableSync::COL_BLINKING_DELAY)));
			object->setTrackNumberDisplay(Conversion::ToBool(rows.getColumn(rowId, DisplayScreenTableSync::COL_TRACK_NUMBER_DISPLAY)));
			object->setServiceNumberDisplay(Conversion::ToBool(rows.getColumn(rowId, DisplayScreenTableSync::COL_SERVICE_NUMBER_DISPLAY)));

			// Physical stops
			vector<string> stops = Conversion::ToStringVector(rows.getColumn(rowId, DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS));
			object->clearPhysicalStops();
			for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
				try
				{
					object->addPhysicalStop(EnvModule::getPhysicalStops().get(Conversion::ToLongLong(*it)).get());
				}
				catch (PhysicalStop::RegistryKeyException e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE_NAME + "/" + DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS, e);
				}
			object->setAllPhysicalStopsDisplayed(Conversion::ToBool(rows.getColumn(rowId, DisplayScreenTableSync::COL_ALL_PHYSICAL_DISPLAYED)));

			// Forbidden places
			object->clearForbiddenPlaces();
			stops = Conversion::ToStringVector(rows.getColumn(rowId, DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS));
			for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
				try
				{
					object->addForbiddenPlace(EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(*it)).get());
				}
				catch (ConnectionPlace::RegistryKeyException e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE_NAME + "/" + DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS, e);
				}

			// DisplayScreenTableSync::COL_FORBIDDEN_LINES_IDS // List of forbidden lines uids, separated by comas

			object->setDirection((DeparturesTableDirection) Conversion::ToInt(rows.getColumn(rowId, DisplayScreenTableSync::COL_DIRECTION)));
			object->setOriginsOnly((EndFilter) Conversion::ToInt(rows.getColumn(rowId, DisplayScreenTableSync::COL_ORIGINS_ONLY)));

			// Displayed places
			stops = Conversion::ToStringVector(rows.getColumn(rowId, DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS));
			object->clearDisplayedPlaces();
			for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
				try
				{
					object->addDisplayedPlace(EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(*it)).get());
				}
				catch (ConnectionPlace::RegistryKeyException e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE_NAME + "/" + DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS, e);
				}

			object->setMaxDelay(Conversion::ToInt(rows.getColumn(rowId, DisplayScreenTableSync::COL_MAX_DELAY)));
			object->setClearingDelay(Conversion::ToInt(rows.getColumn(rowId, DisplayScreenTableSync::COL_CLEARING_DELAY)));
			object->setFirstRow(Conversion::ToInt(rows.getColumn(rowId, DisplayScreenTableSync::COL_FIRST_ROW)));
			object->setGenerationMethod((DisplayScreen::GenerationMethod) Conversion::ToInt(rows.getColumn(rowId, DisplayScreenTableSync::COL_GENERATION_METHOD)));

			// Forced destinations
			stops = Conversion::ToStringVector(rows.getColumn(rowId, DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS));
			object->clearForcedDestinations();
			for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
				try
				{
					object->addForcedDestination(EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(*it)).get());
				}
				catch (ConnectionPlace::RegistryKeyException e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE_NAME + "/" + DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS, e);
				}

			object->setDestinationForceDelay(Conversion::ToInt(rows.getColumn(rowId, DisplayScreenTableSync::COL_DESTINATION_FORCE_DELAY)));
			object->setMaintenanceChecksPerDay(Conversion::ToInt(rows.getColumn(rowId, DisplayScreenTableSync::COL_MAINTENANCE_CHECKS_PER_DAY)));
			object->setMaintenanceIsOnline(Conversion::ToBool(rows.getColumn(rowId, DisplayScreenTableSync::COL_MAINTENANCE_IS_ONLINE)));
			object->setMaintenanceMessage(rows.getColumn(rowId, DisplayScreenTableSync::COL_MAINTENANCE_MESSAGE));
		}

		template<> void SQLiteTableSyncTemplate<DisplayScreen>::save(DisplayScreen* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() == 0)
				object->setKey(getId(1,1));

            query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << (object->getLocalization() ? Conversion::ToString(object->getLocalization()->getKey()) : "0")
				<< "," << Conversion::ToSQLiteString(object->getLocalizationComment())
				<< "," << (object->getType() ? Conversion::ToString(object->getType()->getKey()) : "0")
				<< "," << Conversion::ToString(object->getWiringCode())
				<< "," << Conversion::ToSQLiteString(object->getTitle())
				<< "," << Conversion::ToString(object->getBlinkingDelay())
				<< "," << Conversion::ToString(object->getTrackNumberDisplay())
				<< "," << Conversion::ToString(object->getServiceNumberDisplay())
				<< ",'";

			int count=0;
			const PhysicalStopsSet& pss = object->getPhysicalStops(false);
			for (PhysicalStopsSet::const_iterator itp = pss.begin(); itp != pss.end(); ++itp)
			{
				if (!(*itp)->getKey())
					continue;
				if (count++)
					query << ",";
				query << Conversion::ToString((*itp)->getKey());
			}

			query
				<< "'," << Conversion::ToString(object->getAllPhysicalStopsDisplayed())
				<< ",'";

			count = 0;
			for (ForbiddenPlacesList::const_iterator itf = object->getForbiddenPlaces().begin(); itf != object->getForbiddenPlaces().end(); ++itf)
			{
				if (!(*itf)->getKey())
					continue;
				if (count++)
					query << ",";
				query << Conversion::ToString((*itf)->getKey());
			}

			query << "','";

			count = 0;
			for (LineFilter::const_iterator itl = object->getForbiddenLines().begin(); itl != object->getForbiddenLines().end(); ++itl)
			{
				if (!(*itl)->getKey())
					continue;
				if (count++)
					query << ",";
				query << Conversion::ToString((*itl)->getKey());
			}

			query
				<< "'," << Conversion::ToString((int) object->getDirection())
				<< "," << Conversion::ToString((int) object->getEndFilter())
				<< ",'";

			count = 0;
			for (DisplayedPlacesList::const_iterator itd = object->getDisplayedPlaces().begin(); itd != object->getDisplayedPlaces().end(); ++itd)
			{
				if (!(*itd)->getKey())
					continue;
				if (count++)
					query << ",";
				query << Conversion::ToString((*itd)->getKey());
			}

			query
				<< "'," << Conversion::ToString(object->getMaxDelay())
				<< "," << Conversion::ToString(object->getClearingDelay())
				<< "," << Conversion::ToString(object->getFirstRow())
				<< "," << Conversion::ToString((int) object->getGenerationMethod())
				<< ",'";

			count = 0;
			for (DisplayedPlacesList::const_iterator itd = object->getForcedDestinations().begin(); itd != object->getForcedDestinations().end(); ++itd)
			{
				if (!(*itd)->getKey())
					continue;
				if (count++)
					query << ",";
				query << Conversion::ToString((*itd)->getKey());
			}

			query
				<< "'," << Conversion::ToString(object->getForceDestinationDelay())
				<< "," << Conversion::ToString(object->getMaintenananceChecksPerDay())
				<< "," << Conversion::ToString(object->getIsOnline())
				<< "," << Conversion::ToSQLiteString(object->getMaintenanceMessage())
				<< ")";
			
			sqlite->execUpdate(query.str());
		}

	}

	namespace departurestable
	{
		const std::string DisplayScreenTableSync::COL_BROADCAST_POINT_ID = "broadcast_point_id";
		const std::string DisplayScreenTableSync::COL_BROADCAST_POINT_COMMENT = "broadcast_point_comment";
		const std::string DisplayScreenTableSync::COL_TYPE_ID = "type_id";
		const std::string DisplayScreenTableSync::COL_WIRING_CODE = "wiring_code";
		const std::string DisplayScreenTableSync::COL_TITLE = "title";
		const std::string DisplayScreenTableSync::COL_BLINKING_DELAY = "blinking_delay";
		const std::string DisplayScreenTableSync::COL_TRACK_NUMBER_DISPLAY = "track_number_display";
		const std::string DisplayScreenTableSync::COL_SERVICE_NUMBER_DISPLAY = "service_number_display";
		const std::string DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS = "physical_stops_ids";	// List of physical stops uids, separated by comas
		const std::string DisplayScreenTableSync::COL_ALL_PHYSICAL_DISPLAYED = "all_physicals";
		const std::string DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS = "forbidden_arrival_places_ids";	// List of forbidden connection places uids, separated by comas
		const std::string DisplayScreenTableSync::COL_FORBIDDEN_LINES_IDS = "forbidden_lines_ids";	// List of forbidden lines uids, separated by comas
		const std::string DisplayScreenTableSync::COL_DIRECTION = "direction";
		const std::string DisplayScreenTableSync::COL_ORIGINS_ONLY = "origins_only";
		const std::string DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS = "displayed_places_ids";	// List of displayed places uids, separated by comas
		const std::string DisplayScreenTableSync::COL_MAX_DELAY = "max_delay";
		const std::string DisplayScreenTableSync::COL_CLEARING_DELAY = "clearing_delay";
		const std::string DisplayScreenTableSync::COL_FIRST_ROW = "first_row";
		const std::string DisplayScreenTableSync::COL_GENERATION_METHOD = "generation_method";
		const std::string DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS = "forced_destinations_ids";	// List of forced destination uids in preselection, separated by comas
		const std::string DisplayScreenTableSync::COL_DESTINATION_FORCE_DELAY = "destination_force_delay";
		const std::string DisplayScreenTableSync::COL_MAINTENANCE_CHECKS_PER_DAY = "maintenance_checks";
		const std::string DisplayScreenTableSync::COL_MAINTENANCE_IS_ONLINE = "is_online";
		const std::string DisplayScreenTableSync::COL_MAINTENANCE_MESSAGE = "maintenance_message";

		DisplayScreenTableSync::DisplayScreenTableSync()
			: SQLiteTableSyncTemplate<DisplayScreen>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_BROADCAST_POINT_ID, "INTEGER");
			addTableColumn(COL_BROADCAST_POINT_COMMENT, "TEXT");
			addTableColumn(COL_TYPE_ID, "INTEGER");
			addTableColumn(COL_WIRING_CODE, "INTEGER");
			addTableColumn(COL_TITLE, "TEXT");
			addTableColumn(COL_BLINKING_DELAY, "INTEGER");
			addTableColumn(COL_TRACK_NUMBER_DISPLAY, "INTEGER");
			addTableColumn(COL_SERVICE_NUMBER_DISPLAY, "INTEGER");
			addTableColumn(COL_PHYSICAL_STOPS_IDS, "TEXT");
			addTableColumn(COL_ALL_PHYSICAL_DISPLAYED, "INTEGER");
			addTableColumn(COL_FORBIDDEN_ARRIVAL_PLACES_IDS, "TEXT");
			addTableColumn(COL_FORBIDDEN_LINES_IDS, "TEXT");
			addTableColumn(COL_DIRECTION, "INTEGER");
			addTableColumn(COL_ORIGINS_ONLY, "INTEGER");
			addTableColumn(COL_DISPLAYED_PLACES_IDS, "TEXT");
			addTableColumn(COL_MAX_DELAY, "INTEGER");
			addTableColumn(COL_CLEARING_DELAY, "INTEGER");
			addTableColumn(COL_FIRST_ROW, "INTEGER");
			addTableColumn(COL_GENERATION_METHOD, "INTEGER");
			addTableColumn(COL_FORCED_DESTINATIONS_IDS, "TEXT");
			addTableColumn(COL_DESTINATION_FORCE_DELAY, "INTEGER");
			addTableColumn(COL_MAINTENANCE_CHECKS_PER_DAY, "INTEGER");
			addTableColumn(COL_MAINTENANCE_IS_ONLINE, "INTEGER");
			addTableColumn(COL_MAINTENANCE_MESSAGE, "TEXT");
		}

		void DisplayScreenTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				if (DeparturesTableModule::getDisplayScreens().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					load(DeparturesTableModule::getDisplayScreens().getUpdateable(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))).get(), rows, i);
				}
				else
				{
					shared_ptr<DisplayScreen> object(new DisplayScreen);
					load(object.get(), rows, i);
					DeparturesTableModule::getDisplayScreens().add(object);
				}
			}
		}

		void DisplayScreenTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				shared_ptr<DisplayScreen> object = DeparturesTableModule::getDisplayScreens().getUpdateable(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
				load(object.get(), rows, i);
			}
		}

		void DisplayScreenTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				DeparturesTableModule::getDisplayScreens().remove(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
			}
		}

		std::vector<shared_ptr<DisplayScreen> > DisplayScreenTableSync::search(
			uid duid
			, uid localizationid
			, uid lineid
			, uid typeuid
			, int state 
			, int message 
			, int first /*= 0*/, int number /*= 0*/ 
			, bool orderByUid
			, bool orderByLocalization
			, bool orderByType
			, bool orderByStatus
			, bool orderByMessage
			, bool raisingOrder
		){
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME << " AS d"
				<< " LEFT JOIN " << BroadcastPointTableSync::TABLE_NAME << " AS b ON b." << TABLE_COL_ID << "=d." << COL_BROADCAST_POINT_ID
				<< " WHERE " 
				<< "d." << TABLE_COL_ID << " LIKE '%" << (duid ? Conversion::ToString(duid) : "") << "%'";
			if (localizationid > 0)
				query << " AND b." << BroadcastPointTableSync::TABLE_COL_PLACE_ID << "=" << localizationid;
			if (typeuid > 0)
				query << " AND d." << COL_TYPE_ID << "=" << typeuid;
			if (orderByUid)
				query << " ORDER BY d." << TABLE_COL_ID << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<DisplayScreen> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<DisplayScreen> object(new DisplayScreen());
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
