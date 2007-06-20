
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

#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DeparturesTableModule.h"

#include "15_env/EnvModule.h"
#include "15_env/LineStopTableSync.h"
#include "15_env/LineTableSync.h"
#include "15_env/PhysicalStopTableSync.h"
#include "15_env/ConnectionPlaceTableSync.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/CityTableSync.h"

#include "13_dblog/DBLogEntryTableSync.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

#include <sstream>

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace departurestable;
	using namespace env;
	using namespace dblog;
	using namespace time;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<DisplayScreen>::TABLE_NAME = "t041_display_screens";
		template<> const int SQLiteTableSyncTemplate<DisplayScreen>::TABLE_ID = 41;
		template<> const bool SQLiteTableSyncTemplate<DisplayScreen>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<DisplayScreen>::load(DisplayScreen* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			object->setLocalization(EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(rows.getColumn(rowId, DisplayScreenTableSync::COL_PLACE_ID))));
			object->setLocalizationComment(rows.getColumn(rowId, DisplayScreenTableSync::COL_NAME));
			if (Conversion::ToLongLong(rows.getColumn(rowId, DisplayScreenTableSync::COL_TYPE_ID)) > 0)
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
				catch (PhysicalStop::RegistryKeyException& e)
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
				catch (ConnectionPlace::RegistryKeyException& e)
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
				catch (ConnectionPlace::RegistryKeyException& e)
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
				catch (ConnectionPlace::RegistryKeyException& e)
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
			if (object->getKey() <= 0)
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
				<< "," << Conversion::ToString(object->getMaintenanceChecksPerDay())
				<< "," << Conversion::ToString(object->getIsOnline())
				<< "," << Conversion::ToSQLiteString(object->getMaintenanceMessage())
				<< ")";
			
			sqlite->execUpdate(query.str());
		}

	}

	namespace departurestable
	{
		const std::string DisplayScreenTableSync::_COL_LINE_EXISTS = "line_exists";
		const std::string DisplayScreenTableSync::_COL_LAST_MAINTENANCE_CONTROL = "last_maintenance_control";
		const std::string DisplayScreenTableSync::_COL_LAST_OK_MAINTENANCE_CONTROL = "last_ok_maintenance_control";
		const std::string DisplayScreenTableSync::_COL_CORRUPTED_DATA_START_DATE = "corrupted_data_start_date";
		const std::string DisplayScreenTableSync::_COL_TYPE_NAME = "type_name";

		const std::string DisplayScreenTableSync::COL_PLACE_ID = "broadcast_point_id";
		const std::string DisplayScreenTableSync::COL_NAME = "broadcast_point_comment";
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
			: SQLiteTableSyncTemplate<DisplayScreen>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_PLACE_ID, "INTEGER");
			addTableColumn(COL_NAME, "TEXT");
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
			const security::RightsOfSameClassMap& rights 
			, bool totalControl 
			, RightLevel neededLevel
			, uid duid
			, uid localizationid
			, uid lineid
			, uid typeuid
			, std::string cityName
			, std::string stopName
			, std::string name
			, int state 
			, int message 
			, int first /*= 0*/
			, int number /*= 0*/ 
			, bool orderByUid /*= false*/
			, bool orderByCity /*= true*/
			, bool orderByStopName /*= false*/
			, bool orderByName /*= false*/
			, bool orderByType /*= false*/
			, bool orderByStatus
			, bool orderByMessage
			, bool raisingOrder
		){
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT"
					<< " d.*"
					<< ", EXISTS(SELECT ls." << TABLE_COL_ID << " FROM " << LineStopTableSync::TABLE_NAME << " AS ls INNER JOIN " << PhysicalStopTableSync::TABLE_NAME << " AS p ON p." << TABLE_COL_ID << "= ls." << LineStopTableSync::COL_PHYSICALSTOPID << " WHERE p." << PhysicalStopTableSync::COL_PLACEID << "=d." << COL_PLACE_ID << ") AS " << _COL_LINE_EXISTS
					<< ", (SELECT l." << TABLE_COL_ID << " FROM " << DBLogEntryTableSync::TABLE_NAME << " AS l WHERE l." << DBLogEntryTableSync::COL_LOG_KEY << "='displaymaintenance' AND l." << DBLogEntryTableSync::COL_OBJECT_ID << "=d." << TABLE_COL_ID << " ORDER BY l." << DBLogEntryTableSync::COL_DATE << " DESC LIMIT 1) AS " << _COL_LAST_MAINTENANCE_CONTROL
					<< ", (SELECT MAX(l." << DBLogEntryTableSync::COL_DATE << ") FROM " << DBLogEntryTableSync::TABLE_NAME << " AS l WHERE l." << DBLogEntryTableSync::COL_LOG_KEY << "='displaymaintenance' AND l." << DBLogEntryTableSync::COL_OBJECT_ID << "=d." << TABLE_COL_ID << " AND l." << DBLogEntryTableSync::COL_LEVEL << "=" << static_cast<int>(DBLogEntry::DB_LOG_INFO) << ") AS " << _COL_LAST_OK_MAINTENANCE_CONTROL
					<< ", (SELECT l." << TABLE_COL_ID << " FROM " << DBLogEntryTableSync::TABLE_NAME << " AS l WHERE l." << DBLogEntryTableSync::COL_LOG_KEY << "='displaymaintenance' AND l." << DBLogEntryTableSync::COL_OBJECT_ID << "=d." << TABLE_COL_ID << " AND l." << DBLogEntryTableSync::COL_LEVEL << "=" << static_cast<int>(DBLogEntry::DB_LOG_ERROR) << " ORDER BY l." << DBLogEntryTableSync::COL_DATE << " DESC LIMIT 1) AS " << _COL_CORRUPTED_DATA_START_DATE
					<< ", (SELECT t." << DisplayTypeTableSync::TABLE_COL_NAME << " FROM " << DisplayTypeTableSync::TABLE_NAME << " AS t WHERE t." << TABLE_COL_ID << "=d." << COL_TYPE_ID << ") AS " << _COL_TYPE_NAME
				<< " FROM "
					<< TABLE_NAME << " AS d"
					<< " INNER JOIN " << ConnectionPlaceTableSync::TABLE_NAME << " AS p ON p." << TABLE_COL_ID << "=d." << COL_PLACE_ID
					<< " INNER JOIN " << CityTableSync::TABLE_NAME << " AS c ON c." << TABLE_COL_ID << "=p." << ConnectionPlaceTableSync::TABLE_COL_CITYID
					<< " INNER JOIN " << PhysicalStopTableSync::TABLE_NAME << " AS s ON s." << PhysicalStopTableSync::COL_PLACEID << "=p." << TABLE_COL_ID
					;
			if (lineid != UNKNOWN_VALUE || neededLevel > FORBIDDEN)
				query
					<< " INNER JOIN " << LineStopTableSync::TABLE_NAME << " AS ls " << " ON s." << TABLE_COL_ID << "= ls." << LineStopTableSync::COL_PHYSICALSTOPID 
					<< " INNER JOIN " << LineTableSync::TABLE_NAME << " AS ll ON ll." << TABLE_COL_ID << "= ls." << LineStopTableSync::COL_LINEID
					;

			// Filtering
			query << " WHERE 1 ";
			if (neededLevel > FORBIDDEN)
				query << " AND ll." << LineTableSync::COL_COMMERCIAL_LINE_ID << " IN (" << CommercialLineTableSync::getSQLLinesList(rights, totalControl, neededLevel) << ")";
			if (!cityName.empty())
				query << " AND c." << CityTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(cityName, false) << "%'";
			if (!stopName.empty())
				query << " AND p." << ConnectionPlaceTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(stopName, false) << "%'";
			if (!name.empty())
				query << " AND d." << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
			if (duid != UNKNOWN_VALUE)
				query << " AND d." << TABLE_COL_ID << "=" << Conversion::ToString(duid);
			if (localizationid != UNKNOWN_VALUE)
				query << " AND p." << TABLE_COL_ID << "=" << Conversion::ToString(localizationid);
			if (typeuid != UNKNOWN_VALUE)
				query << " AND d." << COL_TYPE_ID << "=" << typeuid;
			if (lineid != UNKNOWN_VALUE)
				query << " AND ll." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << lineid;
			
			// Grouping
			query << " GROUP BY d." << TABLE_COL_ID;

			// Ordering
			if (orderByUid)
				query << " ORDER BY d." << TABLE_COL_ID << (raisingOrder ? " ASC" : " DESC");
			else if (orderByCity)
				query
					<< " ORDER BY c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",s." << ConnectionPlaceTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",d." << COL_NAME << (raisingOrder ? " ASC" : " DESC")
					;
			else if (orderByStopName)
				query
					<< " ORDER BY s." << ConnectionPlaceTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",d." << COL_NAME << (raisingOrder ? " ASC" : " DESC")
					;
			else if (orderByName)
				query
					<< " ORDER BY d." << COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",s." << ConnectionPlaceTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					;
			else if (orderByType)
				query
					<< " ORDER BY " << _COL_TYPE_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",s." << ConnectionPlaceTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",d." << COL_NAME << (raisingOrder ? " ASC" : " DESC")
					;
			if (number > 0)
			{
				query << " LIMIT " << Conversion::ToString(number + 1);
				if (first > 0)
					query << " OFFSET " << Conversion::ToString(first);
			}

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<DisplayScreen> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<DisplayScreen> object(new DisplayScreen());
					load(object.get(), result, i);
					objects.push_back(object);

					DisplayScreen::Complements c;
					DateTime now;

					// No news test
					if (object->getMaintenanceChecksPerDay())
					{
						if (Conversion::ToLongLong(result.getColumn(i, _COL_LAST_MAINTENANCE_CONTROL)))
						{
							shared_ptr<DBLogEntry> le = DBLogEntryTableSync::get(Conversion::ToLongLong(result.getColumn(i, _COL_LAST_MAINTENANCE_CONTROL)));
							if ((now - le->getDate()) > ((1440 / object->getMaintenanceChecksPerDay()) * 2))
							{
								if ((now - le->getDate()) < ((1440 / object->getMaintenanceChecksPerDay()) * 5))
									c.status = DISPLAY_STATUS_NO_NEWS_WARNING;
								else
									c.status = DISPLAY_STATUS_NO_NEWS_ERROR;
								c.statusText = string("L'afficheur n'a pas envoyé de message de maintenance depuis ") + Conversion::ToString(now - le->getDate()) + string(" minutes.");
								c.lastControl = le->getDate();
							}
							else
							{
								c.status = DISPLAY_STATUS_OK;
								c.statusText = "OK";
							}
						}
						else
						{
							c.status = DISPLAY_STATUS_NO_NEWS_WARNING;
							c.statusText = "L'afficheur n'a jamais envoyé de message de maintenance.";
						}
					}

					// Bad news test
					if (Conversion::ToLongLong(result.getColumn(i, _COL_LAST_MAINTENANCE_CONTROL)))
					{
						shared_ptr<DBLogEntry> le = DBLogEntryTableSync::get(Conversion::ToLongLong(result.getColumn(i, _COL_LAST_MAINTENANCE_CONTROL)));
						if (le->getLevel() == DBLogEntry::DB_LOG_ERROR)
							c.status = DISPLAY_STATUS_HARDWARE_ERROR;
						else
							c.status = DISPLAY_STATUS_HARDWARE_WARNING;
						
						c.statusText = le->getStringContent();
					}

					// Last OK control
					c.lastOKStatus = DateTime::FromSQLTimestamp(result.getColumn(i, _COL_LAST_OK_MAINTENANCE_CONTROL));

					// Data control
//					if (!Conversion::ToLongLong(result.getColumn(i, _COL_BROADCAST_POINT_ID)))
//					{
//						c.dataControl = DISPLAY_DATA_CORRUPTED;
//						c.dataControlText = "L'afficheur est relié à un point de diffusion inexistant.";
						/// @todo Put here a dblog entry writing
						/// @todo Put a control of the link between broadcast point and its place
//					}
			/*		else*/ if (!Conversion::ToInt(result.getColumn(i, _COL_LINE_EXISTS)))
					{
						c.dataControl = DISPLAY_DATA_NO_LINES;
						c.dataControlText = "Aucune ligne ne dessert l'afficheur.";
					}
					else
					{
						c.dataControl = DISPLAY_DATA_OK;
						c.dataControlText = "OK";
						/// @todo Put here a dblog entry writing if the last entry was negative
					}

					c.lastOKDataControl = DateTime::FromSQLTimestamp(result.getColumn(i, _COL_LAST_OK_MAINTENANCE_CONTROL));

					object->setComplements(c);
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
