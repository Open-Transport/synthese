////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenTableSync class implementation.
///	@file DisplayScreenTableSync.cpp
///	@author Hugues Romain
///	@date 2008-12-21 20:16
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DisplayScreenTableSync.h"
#include "DisplayTypeTableSync.h"
#include "DisplayScreen.h"
#include "DisplayType.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "LineStopTableSync.h"
#include "LineTableSync.h"
#include "ConnectionPlaceTableSync.h"
#include "CommercialLineTableSync.h"
#include "CityTableSync.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "PhysicalStop.h"
#include "PhysicalStopTableSync.h"
#include "Line.h"
#include "DateTime.h"
#include "DBLogEntryTableSync.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "Conversion.h"
#include "SentAlarm.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"

#include <sstream>
#include <boost/foreach.hpp>

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
	using namespace messages;
	using namespace geography;
	

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, DisplayScreenTableSync>::FACTORY_KEY("34.50 Display Screens");
	}

	namespace departurestable
	{
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
		const string DisplayScreenTableSync::COL_DISPLAY_CLOCK("display_clock");
		const string DisplayScreenTableSync::COL_COM_PORT("com_port");
		const string DisplayScreenTableSync::COL_CPU_HOST_ID("cpu_host_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<DisplayScreenTableSync>::TABLE(
			"t041_display_screens"
		);
		
		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<DisplayScreenTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_PLACE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_TYPE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_WIRING_CODE, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_TITLE, SQL_TEXT),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_BLINKING_DELAY, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_TRACK_NUMBER_DISPLAY, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_SERVICE_NUMBER_DISPLAY, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_DISPLAY_TEAM, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS, SQL_TEXT),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_ALL_PHYSICAL_DISPLAYED, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS, SQL_TEXT),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_FORBIDDEN_LINES_IDS, SQL_TEXT),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_DIRECTION, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_ORIGINS_ONLY, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS, SQL_TEXT),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_MAX_DELAY, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_CLEARING_DELAY, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_FIRST_ROW, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_GENERATION_METHOD, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS, SQL_TEXT),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_DESTINATION_FORCE_DELAY, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_MAINTENANCE_CHECKS_PER_DAY, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_MAINTENANCE_IS_ONLINE, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_MAINTENANCE_MESSAGE, SQL_TEXT),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_DISPLAY_CLOCK, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_COM_PORT, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_CPU_HOST_ID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};
		
		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<DisplayScreenTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index(DisplayScreenTableSync::COL_PLACE_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};
					
		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::Load(
			DisplayScreen* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
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
			object->setDisplayClock(rows->getBool(DisplayScreenTableSync::COL_DISPLAY_CLOCK));
			object->setComPort(rows->getInt(DisplayScreenTableSync::COL_COM_PORT));
			
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Localization
				uid placeId(rows->getLongLong ( DisplayScreenTableSync::COL_PLACE_ID));
				try
				{
					object->setLocalization(ConnectionPlaceTableSync::Get(placeId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
				{
					Log::GetInstance().warn("Data corrupted in "+ TABLE.NAME + " on display screen : localization "+ Conversion::ToString(placeId) + " not found");
				}
				
				// CPU
				RegistryKeyType cpuId(rows->getLongLong(DisplayScreenTableSync::COL_CPU_HOST_ID));
				if (cpuId > 0)
				{
					try
					{
						object->setCPU(DisplayScreenCPUTableSync::Get(cpuId, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
					{
						Log::GetInstance().warn(
							"Data corrupted in "+ TABLE.NAME + " on display screen : cpu host " +
							Conversion::ToString(cpuId) + " not found"
						);
					}
				}

				// Type
				uid typeId(rows->getLongLong ( DisplayScreenTableSync::COL_TYPE_ID));
				if (typeId > 0)
					object->setType(DisplayTypeTableSync::Get(typeId, env, linkLevel).get());

				// Physical stops
				vector<string> stops = Conversion::ToStringVector(rows->getText ( DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS));
				BOOST_FOREACH(const string& stop, stops)
				{
					try
					{
						uid id(Conversion::ToLongLong(stop));
						object->addPhysicalStop(PhysicalStopTableSync::Get(id, env, linkLevel).get());
					}
					catch (ObjectNotFoundException<PhysicalStop>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS);
					}
				}

				// Forbidden places
				stops = Conversion::ToStringVector (rows->getText (DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS));
				BOOST_FOREACH(const string& stop, stops)
				{
					try
					{
						object->addForbiddenPlace(ConnectionPlaceTableSync::Get(Conversion::ToLongLong(stop), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayScreenTableSync::COL_FORBIDDEN_ARRIVAL_PLACES_IDS, e);
					}
				}

				// Displayed places
				stops = Conversion::ToStringVector (rows->getText (DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS));
				BOOST_FOREACH(const string& stop, stops)
				{
					try
					{
						object->addDisplayedPlace(ConnectionPlaceTableSync::Get(Conversion::ToLongLong(stop), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayScreenTableSync::COL_DISPLAYED_PLACES_IDS, e);
					}
				}

				// Forced destinations
				stops = Conversion::ToStringVector (rows->getText ( DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS));
				BOOST_FOREACH(const string& stop, stops)
				{
					try
					{
						object->addForcedDestination(ConnectionPlaceTableSync::Get(Conversion::ToLongLong(stop), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS, e);
					}
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::Unlink(
			DisplayScreen* object
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
			const ArrivalDepartureTableGenerator::PhysicalStops& pss = object->getPhysicalStops(false);
			BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& itp, pss)
			{
				assert(itp.second->getKey() > 0);

				if (count++)
					query << ",";
				query << Conversion::ToString(itp.first);
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
			for (DisplayedPlacesList::const_iterator itd2 = object->getForcedDestinations().begin(); itd2 != object->getForcedDestinations().end(); ++itd2)
			{
				assert(itd2->second->getKey() > 0);
				if (count++)
					query << ",";
				query << Conversion::ToString(itd2->first);
			}

			query <<
				"'," << Conversion::ToString(object->getForceDestinationDelay()) <<
				",''" <<
				"," << Conversion::ToString(object->getIsOnline()) <<
				"," << Conversion::ToSQLiteString(object->getMaintenanceMessage()) << "," <<
				Conversion::ToString(object->getDisplayClock()) << "," <<
				Conversion::ToString(object->getComPort()) << "," <<
				(object->getCPU() != NULL ? Conversion::ToString(object->getCPU()->getKey()) : "0") <<
				")"
			;
			
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
			    << " INNER JOIN " << PhysicalStopTableSync::TABLE.NAME << " AS s ON s." << PhysicalStopTableSync::COL_PLACEID << "=p." << TABLE_COL_ID
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
				<< " ORDER BY s." << PhysicalStopTableSync::COL_NAME << (raisingOrder ? " ASC" : " DESC")
				<< ",c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
				<< ",d." << COL_NAME << (raisingOrder ? " ASC" : " DESC")
				;
			else if (orderByName)
			    query
				<< " ORDER BY d." << COL_NAME << (raisingOrder ? " ASC" : " DESC")
				<< ",c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
				<< ",s." << PhysicalStopTableSync::COL_NAME << (raisingOrder ? " ASC" : " DESC")
				;
			else if (orderByType)
			    query
				<< " ORDER BY (SELECT " << DisplayTypeTableSync::COL_NAME << " FROM " << DisplayTypeTableSync::TABLE.NAME << " WHERE " << DisplayTypeTableSync::TABLE.NAME << "." << TABLE_COL_ID << " = d." << COL_TYPE_ID << (raisingOrder ? ") ASC" : ") DESC")
				<< ",c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
				<< ",s." << PhysicalStopTableSync::COL_NAME << (raisingOrder ? " ASC" : " DESC")
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



		vector<boost::shared_ptr<SentAlarm> > DisplayScreenTableSync::GetCurrentDisplayedMessage(
			util::Env& env,
			util::RegistryKeyType screenId,
			int limit
		) {
			DateTime now(TIME_CURRENT);
			stringstream q;
			q	<< "SELECT " << AlarmObjectLinkTableSync::COL_ALARM_ID
				<< " FROM " << AlarmObjectLinkTableSync::TABLE.NAME << " AS aol "
				<< " INNER JOIN " << AlarmTableSync::TABLE.NAME << " AS a ON a." << TABLE_COL_ID << "=aol." << AlarmObjectLinkTableSync::COL_ALARM_ID
				<< " WHERE aol." << AlarmObjectLinkTableSync::COL_OBJECT_ID << "=" << Conversion::ToString(screenId)
				<< " AND a." << AlarmTableSync::COL_ENABLED
				<< " AND NOT a." << AlarmTableSync::COL_IS_TEMPLATE
				<< " AND (a." << AlarmTableSync::COL_PERIODSTART << " IS NULL OR a." << AlarmTableSync::COL_PERIODSTART << "<" << now.toSQLString() << ")"
				<< " AND (a." << AlarmTableSync::COL_PERIODEND << " IS NULL OR a." << AlarmTableSync::COL_PERIODEND << ">" << now.toSQLString() << ")"
				<< " ORDER BY a." << AlarmTableSync::COL_LEVEL << " DESC, a." << AlarmTableSync::COL_PERIODSTART << " DESC";
			if (limit > 0)
			{
				q << " LIMIT 1";
			}
			SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(q.str());
			vector<shared_ptr<SentAlarm> > result;
			while(rows->next())
			{
				result.push_back(static_pointer_cast<SentAlarm,Alarm>(AlarmTableSync::GetEditable(rows->getLongLong(AlarmObjectLinkTableSync::COL_ALARM_ID), env)));
			}
			return result;
		}



		bool DisplayScreenTableSync::GetIsAtLeastALineDisplayed(
			util::RegistryKeyType screenId
		) {
			stringstream q;
			q	<< "SELECT l." << TABLE_COL_ID
				<< " FROM " << TABLE.NAME << " AS d"
				<< " INNER JOIN " << PhysicalStopTableSync::TABLE.NAME << " AS s ON s." << PhysicalStopTableSync::COL_PLACEID << "=d." << COL_PLACE_ID
				<< " INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS l ON l." << LineStopTableSync::COL_PHYSICALSTOPID << "=s." << TABLE_COL_ID
				<< " WHERE d." << TABLE_COL_ID << "=" << Conversion::ToString(screenId)
				<< " AND (d." << COL_ALL_PHYSICAL_DISPLAYED << " OR d." << COL_PHYSICAL_STOPS_IDS << " LIKE ('%'|| s." << TABLE_COL_ID << " ||'%'))"
				<< " AND (l." << LineStopTableSync::COL_ISDEPARTURE << " AND d." << COL_DIRECTION << " OR l." << LineStopTableSync::COL_ISARRIVAL << " AND NOT d." << COL_DIRECTION << ")"
				<< " AND (NOT d." << COL_ORIGINS_ONLY << " OR l." << LineStopTableSync::COL_RANKINPATH << "=0)"
				<< " AND NOT EXISTS(SELECT p2." << PhysicalStopTableSync::COL_PLACEID << " FROM " << PhysicalStopTableSync::TABLE.NAME << " AS p2 INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS l2 ON l2." << LineStopTableSync::COL_PHYSICALSTOPID << "=p2." << TABLE_COL_ID
				<< " WHERE l2." << LineStopTableSync::COL_LINEID << "=l." << LineStopTableSync::COL_LINEID
				<< " AND l2." << LineStopTableSync::COL_RANKINPATH << ">l." << LineStopTableSync::COL_RANKINPATH
				<< " AND l2." << LineStopTableSync::COL_ISARRIVAL
				<< " AND ('%'|| p2." << PhysicalStopTableSync::COL_PLACEID << " ||'%') LIKE d." << COL_FORBIDDEN_ARRIVAL_PLACES_IDS
				<< ")"
				<< " LIMIT 1";
			SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(q.str());
			return rows->next();
		}



		vector<shared_ptr<SentAlarm> > DisplayScreenTableSync::GetFutureDisplayedMessages(
			Env& env,
			RegistryKeyType screenId,
			optional<int> number /*= UNKNOWN_VALUE */
		){
			DateTime now(TIME_CURRENT);
			stringstream q;
			q	<< "SELECT " << AlarmObjectLinkTableSync::COL_ALARM_ID
				<< " FROM " << AlarmObjectLinkTableSync::TABLE.NAME << " AS aol "
				<< " INNER JOIN " << AlarmTableSync::TABLE.NAME << " AS a ON a." << TABLE_COL_ID << "=aol." << AlarmObjectLinkTableSync::COL_ALARM_ID
				<< " WHERE aol." << AlarmObjectLinkTableSync::COL_OBJECT_ID << "=" << Conversion::ToString(screenId)
				<< " AND a." << AlarmTableSync::COL_ENABLED
				<< " AND NOT a." << AlarmTableSync::COL_IS_TEMPLATE
				<< " AND a." << AlarmTableSync::COL_PERIODSTART << ">" << now.toSQLString()
				<< " ORDER BY a." << AlarmTableSync::COL_PERIODSTART;
			if (number)
			{
				q << " LIMIT " << *number;
			}
			SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(q.str());
			vector<shared_ptr<SentAlarm> > result;
			while(rows->next())
			{
				result.push_back(static_pointer_cast<SentAlarm,Alarm>(AlarmTableSync::GetEditable(rows->getLongLong(AlarmObjectLinkTableSync::COL_ALARM_ID), env)));
			}
			return result;
		}
	}
}
