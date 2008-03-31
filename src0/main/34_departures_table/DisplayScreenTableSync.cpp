
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
#include "34_departures_table/DisplayType.h"

#include "15_env/LineStopTableSync.h"
#include "15_env/LineTableSync.h"
#include "15_env/PhysicalStopTableSync.h"
#include "15_env/ConnectionPlaceTableSync.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/CityTableSync.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/PhysicalStop.h"
#include "15_env/Line.h"

#include "13_dblog/DBLogEntryTableSync.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace departurestable;
	using namespace env;
	using namespace dblog;
	using namespace time;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, DisplayScreenTableSync>::FACTORY_KEY("34.50 Display Screens");
	}

	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<DisplayScreenTableSync>::TABLE_NAME = "t041_display_screens";
		template<> const int SQLiteTableSyncTemplate<DisplayScreenTableSync>::TABLE_ID = 41;
		template<> const bool SQLiteTableSyncTemplate<DisplayScreenTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::load(DisplayScreen* object, const db::SQLiteResultSPtr& rows )
		{
			object->setKey (rows->getLongLong (TABLE_COL_ID));
			object->setLocalizationComment (rows->getText ( DisplayScreenTableSync::COL_NAME));
			object->setWiringCode (rows->getInt ( DisplayScreenTableSync::COL_WIRING_CODE));
			object->setTitle (rows->getText ( DisplayScreenTableSync::COL_TITLE));
			object->setBlinkingDelay (rows->getInt ( DisplayScreenTableSync::COL_BLINKING_DELAY));
			object->setTrackNumberDisplay (rows->getBool ( DisplayScreenTableSync::COL_TRACK_NUMBER_DISPLAY));
			object->setServiceNumberDisplay (rows->getBool ( DisplayScreenTableSync::COL_SERVICE_NUMBER_DISPLAY));
			object->setAllPhysicalStopsDisplayed (rows->getBool (DisplayScreenTableSync::COL_ALL_PHYSICAL_DISPLAYED));
			object->setDirection(static_cast<DeparturesTableDirection>(rows->getInt(DisplayScreenTableSync::COL_DIRECTION)));
			object->setOriginsOnly(static_cast<EndFilter>(rows->getInt(DisplayScreenTableSync::COL_ORIGINS_ONLY)));
			object->setMaxDelay (rows->getInt ( DisplayScreenTableSync::COL_MAX_DELAY));
			object->setClearingDelay (rows->getInt ( DisplayScreenTableSync::COL_CLEARING_DELAY));
			object->setFirstRow (rows->getInt ( DisplayScreenTableSync::COL_FIRST_ROW));
			object->setGenerationMethod(static_cast<DisplayScreen::GenerationMethod>(rows->getInt(DisplayScreenTableSync::COL_GENERATION_METHOD)));
			object->setDestinationForceDelay (rows->getInt ( DisplayScreenTableSync::COL_DESTINATION_FORCE_DELAY));
			object->setMaintenanceChecksPerDay (rows->getInt ( DisplayScreenTableSync::COL_MAINTENANCE_CHECKS_PER_DAY));
			object->setMaintenanceIsOnline (rows->getBool ( DisplayScreenTableSync::COL_MAINTENANCE_IS_ONLINE));
			object->setMaintenanceMessage (rows->getText ( DisplayScreenTableSync::COL_MAINTENANCE_MESSAGE));
			object->setDisplayTeam(rows->getBool(DisplayScreenTableSync::COL_DISPLAY_TEAM));
		}


		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::_link(DisplayScreen* object, const db::SQLiteResultSPtr& rows, GetSource temporary)
		{
			// Column reading
			uid placeId(rows->getLongLong ( DisplayScreenTableSync::COL_PLACE_ID));
			uid typeId(rows->getLongLong ( DisplayScreenTableSync::COL_TYPE_ID));
			
			// Localization
			try
			{
				object->setLocalization(ConnectionPlaceTableSync::Get(placeId, object, true, temporary));
			}
			catch(PublicTransportStopZoneConnectionPlace::ObjectNotFoundException& e)
			{
				Log::GetInstance().warn("Data corrupted in "+ TABLE_NAME + " on display screen : localization "+ Conversion::ToString(placeId) + " not found");
			}

			// Type
			if (typeId > 0)
				object->setType(DisplayTypeTableSync::Get(typeId, object, true, temporary));

			// Physical stops
			vector<string> stops = Conversion::ToStringVector(rows->getText ( DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS));
			for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
			{
				try
				{
					uid id(Conversion::ToLongLong(*it));
					object->addPhysicalStop(PhysicalStopTableSync::Get(id, object, true, temporary));
				}
				catch (PhysicalStop::ObjectNotFoundException& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE_NAME + "/" + DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS);
				}
			}

			// Forbidden places
			stops = Conversion::ToStringVector (rows->getText (DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS));
			for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
			{
				try
				{
					object->addForbiddenPlace(ConnectionPlaceTableSync::Get(Conversion::ToLongLong(*it), object, false, temporary));
				}
				catch (PublicTransportStopZoneConnectionPlace::ObjectNotFoundException& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE_NAME + "/" + DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS, e);
				}
			}

			// Displayed places
			stops = Conversion::ToStringVector (rows->getText (DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS));
			for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
			{
				try
				{
					object->addDisplayedPlace(ConnectionPlaceTableSync::Get(Conversion::ToLongLong(*it), object, false, temporary));
				}
				catch (PublicTransportStopZoneConnectionPlace::ObjectNotFoundException& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE_NAME + "/" + DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS, e);
				}
			}

			// Forced destinations
			stops = Conversion::ToStringVector (rows->getText ( DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS));
			for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
			{
				try
				{
					object->addForcedDestination(ConnectionPlaceTableSync::Get (Conversion::ToLongLong(*it), object, false, temporary));
				}
				catch (PublicTransportStopZoneConnectionPlace::ObjectNotFoundException& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE_NAME + "/" + DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS, e);
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::_unlink(DisplayScreen* object)
		{
			object->setLocalization(NULL);
			object->setType(NULL);
			object->clearPhysicalStops();
			object->clearForbiddenPlaces();
			object->clearDisplayedPlaces();
			object->clearForcedDestinations();
		}



		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::save(DisplayScreen* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());

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
				<< "," << Conversion::ToString(object->getDisplayTeam())
				<< ",'";

			int count=0;
			const PhysicalStops& pss = object->getPhysicalStops(false);
			for (PhysicalStops::const_iterator itp = pss.begin(); itp != pss.end(); ++itp)
			{
				assert(itp->second->getKey() > 0);
				if (count++)
					query << ",";
				query << Conversion::ToString(itp->first);
			}

			query
				<< "'," << Conversion::ToString(object->getAllPhysicalStopsDisplayed())
				<< ",'";

			count = 0;
			for (ForbiddenPlacesList::const_iterator itf = object->getForbiddenPlaces().begin(); itf != object->getForbiddenPlaces().end(); ++itf)
			{
				assert(itf->second->getKey() > 0);
				if (count++)
					query << ",";
				query << Conversion::ToString(itf->first);
			}

			query << "','";

			count = 0;
			for (LineFilter::const_iterator itl = object->getForbiddenLines().begin(); itl != object->getForbiddenLines().end(); ++itl)
			{
				assert(itl->second->getKey() > 0);
				if (count++)
					query << ",";
				query << Conversion::ToString(itl->first);
			}

			query
				<< "'," << Conversion::ToString(static_cast<int>(object->getDirection()))
				<< "," << Conversion::ToString(static_cast<int>(object->getEndFilter()))
				<< ",'";

			count = 0;
			for (DisplayedPlacesList::const_iterator itd = object->getDisplayedPlaces().begin(); itd != object->getDisplayedPlaces().end(); ++itd)
			{
				assert(itd->second->getKey() > 0);
				if (count++)
					query << ",";
				query << Conversion::ToString(itd->first);
			}

			query
				<< "'," << Conversion::ToString(object->getMaxDelay())
				<< "," << Conversion::ToString(object->getClearingDelay())
				<< "," << Conversion::ToString(object->getFirstRow())
				<< "," << Conversion::ToString(static_cast<int>(object->getGenerationMethod()))
				<< ",'";

			count = 0;
			for (DisplayedPlacesList::const_iterator itd = object->getForcedDestinations().begin(); itd != object->getForcedDestinations().end(); ++itd)
			{
				assert(itd->second->getKey() > 0);
				if (count++)
					query << ",";
				query << Conversion::ToString(itd->first);
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
		const string DisplayScreenTableSync::_COL_LINE_EXISTS = "line_exists";
		const string DisplayScreenTableSync::_COL_LAST_MAINTENANCE_CONTROL = "last_maintenance_control";
		const string DisplayScreenTableSync::_COL_LAST_OK_MAINTENANCE_CONTROL = "last_ok_maintenance_control";
		const string DisplayScreenTableSync::_COL_CORRUPTED_DATA_START_DATE = "corrupted_data_start_date";
		const string DisplayScreenTableSync::_COL_TYPE_NAME = "type_name";

		const string DisplayScreenTableSync::COL_PLACE_ID = "broadcast_point_id";
		const string DisplayScreenTableSync::COL_NAME = "broadcast_point_comment";
		const string DisplayScreenTableSync::COL_TYPE_ID = "type_id";
		const string DisplayScreenTableSync::COL_WIRING_CODE = "wiring_code";
		const string DisplayScreenTableSync::COL_TITLE = "title";
		const string DisplayScreenTableSync::COL_BLINKING_DELAY = "blinking_delay";
		const string DisplayScreenTableSync::COL_TRACK_NUMBER_DISPLAY = "track_number_display";
		const string DisplayScreenTableSync::COL_SERVICE_NUMBER_DISPLAY = "service_number_display";
		const string DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS = "physical_stops_ids";	// List of physical stops uids, separated by comas
		const string DisplayScreenTableSync::COL_ALL_PHYSICAL_DISPLAYED = "all_physicals";
		const string DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS = "forbidden_arrival_places_ids";	// List of forbidden connection places uids, separated by comas
		const string DisplayScreenTableSync::COL_FORBIDDEN_LINES_IDS = "forbidden_lines_ids";	// List of forbidden lines uids, separated by comas
		const string DisplayScreenTableSync::COL_DIRECTION = "direction";
		const string DisplayScreenTableSync::COL_ORIGINS_ONLY = "origins_only";
		const string DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS = "displayed_places_ids";	// List of displayed places uids, separated by comas
		const string DisplayScreenTableSync::COL_MAX_DELAY = "max_delay";
		const string DisplayScreenTableSync::COL_CLEARING_DELAY = "clearing_delay";
		const string DisplayScreenTableSync::COL_FIRST_ROW = "first_row";
		const string DisplayScreenTableSync::COL_GENERATION_METHOD = "generation_method";
		const string DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS = "forced_destinations_ids";	// List of forced destination uids in preselection, separated by comas
		const string DisplayScreenTableSync::COL_DESTINATION_FORCE_DELAY = "destination_force_delay";
		const string DisplayScreenTableSync::COL_MAINTENANCE_CHECKS_PER_DAY = "maintenance_checks";
		const string DisplayScreenTableSync::COL_MAINTENANCE_IS_ONLINE = "is_online";
		const string DisplayScreenTableSync::COL_MAINTENANCE_MESSAGE = "maintenance_message";
		const string DisplayScreenTableSync::COL_DISPLAY_TEAM("display_team");

		DisplayScreenTableSync::DisplayScreenTableSync()
			: SQLiteRegistryTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>()
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
			addTableColumn(COL_DISPLAY_TEAM, "INTEGER");
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


	    
	    vector<shared_ptr<DisplayScreen> > DisplayScreenTableSync::search(
			const security::RightsOfSameClassMap& rights 
			, bool totalControl 
			, RightLevel neededLevel
			, uid duid
			, uid localizationid
			, uid lineid
			, uid typeuid
			, string cityName
			, string stopName
			, string name
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
			SQLite* sqlite = DBModule::GetSQLite();
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
			    query << " AND ll." << LineTableSync::COL_COMMERCIAL_LINE_ID << " IN (" << CommercialLineTableSync::getSQLLinesList(rights, totalControl, neededLevel, false) << ")";
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
			    SQLiteResultSPtr rows = sqlite->execQuery(query.str());
			    vector<shared_ptr<DisplayScreen> > objects;
			    while (rows->next ())
			    {
				shared_ptr<DisplayScreen> object(new DisplayScreen());
				load(object.get(), rows);
				link(object.get(), rows, GET_AUTO);
				objects.push_back(object);
				
				DisplayScreen::Complements c;
				DateTime now (TIME_CURRENT);
				
				// No news test
				if (object->getMaintenanceChecksPerDay())
				{
				    if (rows->getLongLong(_COL_LAST_MAINTENANCE_CONTROL))
				    {
					shared_ptr<const DBLogEntry> le = DBLogEntryTableSync::Get(rows->getLongLong(_COL_LAST_MAINTENANCE_CONTROL));
					if ((now - le->getDate()) > ((1440 / object->getMaintenanceChecksPerDay()) * 2))
					{
					    if ((now - le->getDate()) < ((1440 / object->getMaintenanceChecksPerDay()) * 5))
						c.status = DISPLAY_STATUS_NO_NEWS_WARNING;
					    else
						c.status = DISPLAY_STATUS_NO_NEWS_ERROR;
					    c.statusText = string("L'afficheur n'a pas envoyé de message de maintenance depuis ") + 
						Conversion::ToString(now - le->getDate()) + string(" minutes.");
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
				if (rows->getLongLong (_COL_LAST_MAINTENANCE_CONTROL))
				{
				    shared_ptr<const DBLogEntry> le = DBLogEntryTableSync::Get(
					rows->getLongLong (_COL_LAST_MAINTENANCE_CONTROL));

				    if (le->getLevel() == DBLogEntry::DB_LOG_ERROR)
					c.status = DISPLAY_STATUS_HARDWARE_ERROR;
				    else
					c.status = DISPLAY_STATUS_HARDWARE_WARNING;
				    
				    c.statusText = le->getStringContent();
					}

				// Last OK control
				c.lastOKStatus = DateTime::FromSQLTimestamp(rows->getText (_COL_LAST_OK_MAINTENANCE_CONTROL));

					// Data control
//					if (!Conversion::ToLongLong(rows->getText(_COL_BROADCAST_POINT_ID)))
//					{
//						c.dataControl = DISPLAY_DATA_CORRUPTED;
//						c.dataControlText = "L'afficheur est relié à un point de diffusion inexistant.";
						/// @todo Put here a dblog entry writing
						/// @todo Put a control of the link between broadcast point and its place
//					}
				/*		else*/ if (!rows->getInt (_COL_LINE_EXISTS))
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
				
				c.lastOKDataControl = DateTime::FromSQLTimestamp(rows->getText (_COL_LAST_OK_MAINTENANCE_CONTROL));
				
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
