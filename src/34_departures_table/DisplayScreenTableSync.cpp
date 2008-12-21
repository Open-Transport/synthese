
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

#include "DisplayScreenTableSync.h"
#include "DisplayTypeTableSync.h"
#include "DisplayScreen.h"
#include "DisplayType.h"

#include "LineStopTableSync.h"
#include "LineTableSync.h"
#include "PhysicalStopTableSync.h"
#include "ConnectionPlaceTableSync.h"
#include "CommercialLineTableSync.h"
#include "CityTableSync.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "PhysicalStop.h"
#include "Line.h"

#include "DBLogEntryTableSync.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Conversion.h"

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
	}

	namespace db
	{
		template<> const SQLiteTableFormat SQLiteTableSyncTemplate<DisplayScreenTableSync>::TABLE(
			DisplayScreenTableSync::CreateFormat(
				"t041_display_screens",
				SQLiteTableFormat::CreateFields(
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_PLACE_ID, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_NAME, TEXT),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_TYPE_ID, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_WIRING_CODE, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_TITLE, TEXT),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_BLINKING_DELAY, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_TRACK_NUMBER_DISPLAY, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_SERVICE_NUMBER_DISPLAY, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_DISPLAY_TEAM, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS, TEXT),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_ALL_PHYSICAL_DISPLAYED, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS, TEXT),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_FORBIDDEN_LINES_IDS, TEXT),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_DIRECTION, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_ORIGINS_ONLY, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS, TEXT),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_MAX_DELAY, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_CLEARING_DELAY, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_FIRST_ROW, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_GENERATION_METHOD, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS, TEXT),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_DESTINATION_FORCE_DELAY, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_MAINTENANCE_CHECKS_PER_DAY, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_MAINTENANCE_IS_ONLINE, INTEGER),
					SQLiteTableFormat::Field(DisplayScreenTableSync::COL_MAINTENANCE_MESSAGE, TEXT),
					SQLiteTableFormat::Field()
				), SQLiteTableFormat::CreateIndexes(
					SQLiteTableFormat::Index(DisplayScreenTableSync::COL_PLACE_ID),
					SQLiteTableFormat::Index()
		)	)	);
					
		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::Load(
			DisplayScreen* object,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
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
			object->setMaintenanceIsOnline (rows->getBool ( DisplayScreenTableSync::COL_MAINTENANCE_IS_ONLINE));
			object->setMaintenanceMessage (rows->getText ( DisplayScreenTableSync::COL_MAINTENANCE_MESSAGE));
			object->setDisplayTeam(rows->getBool(DisplayScreenTableSync::COL_DISPLAY_TEAM));

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Column reading
				uid placeId(rows->getLongLong ( DisplayScreenTableSync::COL_PLACE_ID));
				uid typeId(rows->getLongLong ( DisplayScreenTableSync::COL_TYPE_ID));
			
				// Localization
				try
				{
					object->setLocalization(ConnectionPlaceTableSync::Get(placeId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
				{
					Log::GetInstance().warn("Data corrupted in "+ TABLE.NAME + " on display screen : localization "+ Conversion::ToString(placeId) + " not found");
				}

				// Type
				if (typeId > 0)
					object->setType(DisplayTypeTableSync::Get(typeId, env, linkLevel).get());

				// Physical stops
				vector<string> stops = Conversion::ToStringVector(rows->getText ( DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS));
				for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
				{
					try
					{
						uid id(Conversion::ToLongLong(*it));
						object->addPhysicalStop(PhysicalStopTableSync::Get(id, env, linkLevel).get());
					}
					catch (ObjectNotFoundException<PhysicalStop>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS);
					}
				}

				// Forbidden places
				stops = Conversion::ToStringVector (rows->getText (DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS));
				for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
				{
					try
					{
						object->addForbiddenPlace(ConnectionPlaceTableSync::Get(Conversion::ToLongLong(*it), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS, e);
					}
				}

				// Displayed places
				stops = Conversion::ToStringVector (rows->getText (DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS));
				for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
				{
					try
					{
						object->addDisplayedPlace(ConnectionPlaceTableSync::Get(Conversion::ToLongLong(*it), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS, e);
					}
				}

				// Forced destinations
				stops = Conversion::ToStringVector (rows->getText ( DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS));
				for (vector<string>::iterator it = stops.begin(); it != stops.end(); ++it)
				{
					try
					{
						object->addForcedDestination(ConnectionPlaceTableSync::Get (Conversion::ToLongLong(*it), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS, e);
					}
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::Unlink(
			DisplayScreen* object,
			Env* env
		){
			object->setLocalization(NULL);
			object->setType(NULL);
			object->clearPhysicalStops();
			object->clearForbiddenPlaces();
			object->clearDisplayedPlaces();
			object->clearForcedDestinations();
		}



		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::Save(
			DisplayScreen* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());

            query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
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
				<< ",''"
				<< "," << Conversion::ToString(object->getIsOnline())
				<< "," << Conversion::ToSQLiteString(object->getMaintenanceMessage())
				<< ")";
			
			sqlite->execUpdate(query.str());
		}

	}

	namespace departurestable
	{
		DisplayScreenTableSync::DisplayScreenTableSync()
			: SQLiteRegistryTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>()
		{
		}


	    
	    void DisplayScreenTableSync::Search(
			Env& env,
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
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
			    << " SELECT"
			    << " d.*"
			    << " FROM "
			    << TABLE.NAME << " AS d"
			    << " INNER JOIN " << ConnectionPlaceTableSync::TABLE.NAME << " AS p ON p." << TABLE_COL_ID << "=d." << COL_PLACE_ID
			    << " INNER JOIN " << CityTableSync::TABLE.NAME << " AS c ON c." << TABLE_COL_ID << "=p." << ConnectionPlaceTableSync::TABLE_COL_CITYID
			    ;
			if (lineid != UNKNOWN_VALUE || neededLevel > FORBIDDEN)
			    query
				<< " INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS ls " << " ON s." << TABLE_COL_ID << "= ls." << LineStopTableSync::COL_PHYSICALSTOPID 
				<< " INNER JOIN " << LineTableSync::TABLE.NAME << " AS ll ON ll." << TABLE_COL_ID << "= ls." << LineStopTableSync::COL_LINEID
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

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
