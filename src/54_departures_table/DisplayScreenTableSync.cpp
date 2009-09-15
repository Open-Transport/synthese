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
#include "ScenarioTableSync.h"

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
		const string DisplayScreenTableSync::COL_MAC_ADDRESS("mac_address");
		const string DisplayScreenTableSync::COL_ROUTE_PLANNING_WITH_TRANSFER("route_planning_with_transfer");
		const string DisplayScreenTableSync::COL_TRANSFER_DESTINATIONS("transfer_destinations");
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
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_MAC_ADDRESS, SQL_TEXT),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_ROUTE_PLANNING_WITH_TRANSFER, SQL_BOOLEAN),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_TRANSFER_DESTINATIONS, SQL_TEXT),
			SQLiteTableSync::Field()
		};
		
		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<DisplayScreenTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index(DisplayScreenTableSync::COL_PLACE_ID.c_str(), ""),
			SQLiteTableSync::Index(DisplayScreenTableSync::COL_MAC_ADDRESS.c_str(), ""),
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
			object->setMacAddress(rows->getText(DisplayScreenTableSync::COL_MAC_ADDRESS));
			object->setRoutePlanningWithTransfer(rows->getBool(DisplayScreenTableSync::COL_ROUTE_PLANNING_WITH_TRANSFER));
			object->setLocalization(NULL);
			object->setCPU(NULL);
			object->setType(NULL);
			object->clearForbiddenPlaces();
			object->clearDisplayedPlaces();
			object->clearForcedDestinations();
			object->clearPhysicalStops();
			object->clearTransferDestinations();

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Localization
				RegistryKeyType placeId(rows->getLongLong( DisplayScreenTableSync::COL_PLACE_ID));
				if(placeId != 0) try
				{
					object->setLocalization(ConnectionPlaceTableSync::Get(placeId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
				{
					Log::GetInstance().warn(
						"Data corrupted in "+ TABLE.NAME + " on display screen : localization "+ lexical_cast<string>(placeId) + " not found"
					);
				}
				
				// CPU
				RegistryKeyType cpuId(rows->getLongLong(DisplayScreenTableSync::COL_CPU_HOST_ID));
				if (cpuId > 0) try
				{
					object->setCPU(DisplayScreenCPUTableSync::Get(cpuId, env, linkLevel).get());
					DisplayScreenCPUTableSync::GetEditable(cpuId, env, linkLevel)->addWiredScreen(object);
				}
				catch(ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
				{
					Log::GetInstance().warn(
						"Data corrupted in "+ TABLE.NAME + " on display screen : cpu host " +
						lexical_cast<string>(cpuId) + " not found"
					);
				}

				// Type
				RegistryKeyType typeId(rows->getLongLong ( DisplayScreenTableSync::COL_TYPE_ID));
				if (typeId > 0) try
				{
					object->setType(DisplayTypeTableSync::Get(typeId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<DisplayType>& e)
				{
					Log::GetInstance().warn(
						"Data corrupted in "+ TABLE.NAME + " on display screen : type " +
						lexical_cast<string>(typeId) + " not found"
					);
				}

				// Physical stops
				vector<string> stops = Conversion::ToStringVector(rows->getText ( DisplayScreenTableSync::COL_PHYSICAL_STOPS_IDS));
				BOOST_FOREACH(const string& stop, stops)
				{
					try
					{
						RegistryKeyType id(Conversion::ToLongLong(stop));
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
				}	}

				// Transfers
				stops = Conversion::ToStringVector(rows->getText(DisplayScreenTableSync::COL_TRANSFER_DESTINATIONS));
				BOOST_FOREACH(const string& stop, stops)
				{
					typedef tokenizer<char_separator<char> > tokenizer;
					tokenizer tokens (stop, char_separator<char>(":"));
					tokenizer::iterator it(tokens.begin());
					string id1(*it);
					++it;
					string id2(*it);

					try
					{
						object->addTransferDestination(
							ConnectionPlaceTableSync::Get(lexical_cast<RegistryKeyType>(id1), env, linkLevel).get(),
							ConnectionPlaceTableSync::Get(lexical_cast<RegistryKeyType>(id2), env, linkLevel).get()
						);
					}
					catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS, e);
					}
				}
			
		}	}



		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::Unlink(
			DisplayScreen* object
		){
			if(object->getCPU())
			{
				const_cast<DisplayScreenCPU*>(object->getCPU())->removeWiredScreen(object);
			}
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
				<< object->getKey()
				<< "," << (object->getLocalization() ? object->getLocalization()->getKey() : RegistryKeyType(0))
				<< "," << Conversion::ToSQLiteString(object->getLocalizationComment())
				<< "," << (object->getType() ? object->getType()->getKey() : RegistryKeyType(0))
				<< "," << object->getWiringCode()
				<< "," << Conversion::ToSQLiteString(object->getTitle())
				<< "," << object->getBlinkingDelay()
				<< "," << object->getTrackNumberDisplay()
				<< "," << object->getServiceNumberDisplay()
				<< "," << object->getDisplayTeam()
				<< ",'";

			int count=0;
			const ArrivalDepartureTableGenerator::PhysicalStops& pss = object->getPhysicalStops(false);
			BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& itp, pss)
			{
				assert(itp.second->getKey() > 0);

				if (count++)
					query << ",";
				query << itp.first;
			}

			query
				<< "'," << object->getAllPhysicalStopsDisplayed()
				<< ",'";

			count = 0;
			for (ForbiddenPlacesList::const_iterator itf = object->getForbiddenPlaces().begin(); itf != object->getForbiddenPlaces().end(); ++itf)
			{
				assert(itf->second->getKey() > 0);
				if (count++)
					query << ",";
				query << itf->first;
			}

			query << "','";

			count = 0;
			for (LineFilter::const_iterator itl = object->getForbiddenLines().begin(); itl != object->getForbiddenLines().end(); ++itl)
			{
				assert(itl->second->getKey() > 0);
				if (count++)
					query << ",";
				query << itl->first;
			}

			query
				<< "'," << static_cast<int>(object->getDirection())
				<< "," << static_cast<int>(object->getEndFilter())
				<< ",'";

			count = 0;
			BOOST_FOREACH(const DisplayedPlacesList::value_type& itd, object->getDisplayedPlaces())
			{
				assert(itd.second->getKey() > 0);
				if (count++)
					query << ",";
				query << itd.second->getKey();
			}

			query
				<< "'," << object->getMaxDelay()
				<< "," << object->getClearingDelay()
				<< "," << object->getFirstRow()
				<< "," << static_cast<int>(object->getGenerationMethod())
				<< ",'";

			count = 0;
			BOOST_FOREACH(const DisplayedPlacesList::value_type& itd2, object->getForcedDestinations())
			{
				assert(itd2.second->getKey() > 0);
				if (count++)
					query << ",";
				query << itd2.second->getKey();
			}

			query <<
				"'," << object->getForceDestinationDelay() <<
				",''" <<
				"," << object->getIsOnline() <<
				"," << Conversion::ToSQLiteString(object->getMaintenanceMessage()) << "," <<
				object->getDisplayClock() << "," <<
				object->getComPort() << "," <<
				(object->getCPU() != NULL ? object->getCPU()->getKey() : RegistryKeyType(0)) << "," <<
				Conversion::ToSQLiteString(object->getMacAddress()) << "," <<
				object->getRoutePlanningWithTransfer() << ",'";
			count = 0;
			BOOST_FOREACH(const TransferDestinationsList::value_type& it, object->getTransferdestinations())
			{
				BOOST_FOREACH(const TransferDestinationsList::mapped_type::value_type& it2, it.second)
				{
					if (count++) query << ",";
					query << it.first->getKey() << ":" << it2->getKey();
				}
			}
			query << "'";

			query << ")";
			
			sqlite->execUpdate(query.str());
	}	}

	namespace departurestable
	{
	    DisplayScreenTableSync::SearchResult DisplayScreenTableSync::Search(
			Env& env,
			optional<const security::RightsOfSameClassMap&> rights 
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
			, boost::optional<std::size_t> number
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
			    << TABLE.NAME << " AS d";
			if(localizationid != 0)
			{
				query
					<< " INNER JOIN " << ConnectionPlaceTableSync::TABLE.NAME << " AS p ON p." << TABLE_COL_ID << "=d." << COL_PLACE_ID
					<< " INNER JOIN " << CityTableSync::TABLE.NAME << " AS c ON c." << TABLE_COL_ID << "=p." << ConnectionPlaceTableSync::TABLE_COL_CITYID
					<< " INNER JOIN " << PhysicalStopTableSync::TABLE.NAME << " AS s ON s." << PhysicalStopTableSync::COL_PLACEID << "=p." << TABLE_COL_ID
				;
			
				if (lineid != UNKNOWN_VALUE || neededLevel > FORBIDDEN)
					query
					<< " INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS ls " << " ON s." << TABLE_COL_ID << "= ls." << LineStopTableSync::COL_PHYSICALSTOPID 
					<< " INNER JOIN " << LineTableSync::TABLE.NAME << " AS ll ON ll." << TABLE_COL_ID << "= ls." << LineStopTableSync::COL_LINEID
				;
			}
			
			// Filtering
			query << " WHERE 1 ";
			if(localizationid != 0)
			{
				if (neededLevel > FORBIDDEN && rights)
					query << " AND ll." << LineTableSync::COL_COMMERCIAL_LINE_ID << " IN (" << CommercialLineTableSync::getSQLLinesList(*rights, totalControl, neededLevel, false) << ")";
				if (!cityName.empty())
					query << " AND c." << CityTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(cityName, false) << "%'";
				if (!stopName.empty())
					query << " AND p." << ConnectionPlaceTableSync::TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(stopName, false) << "%'";
				if (lineid != UNKNOWN_VALUE)
					query << " AND ll." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << lineid;
			}
			if (!name.empty())
			    query << " AND d." << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
			if (duid != UNKNOWN_VALUE)
			    query << " AND d." << TABLE_COL_ID << "=" << duid;
			if (localizationid != UNKNOWN_VALUE)
			    query << " AND d." << COL_PLACE_ID << "=" << localizationid;
			if (typeuid != UNKNOWN_VALUE)
			    query << " AND d." << COL_TYPE_ID << "=" << typeuid;
			
			// Grouping
			query << " GROUP BY d." << TABLE_COL_ID;
			
			// Ordering
			if (orderByUid)
			{
				query << " ORDER BY d." << TABLE_COL_ID << (raisingOrder ? " ASC" : " DESC");
			}
			else if (localizationid != 0 && orderByCity)
			{
				query
					<< " ORDER BY c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",s." << ConnectionPlaceTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",d." << COL_NAME << (raisingOrder ? " ASC" : " DESC")
				;
			}
			else if (localizationid != 0 && orderByStopName)
			{
				query
					<< " ORDER BY s." << PhysicalStopTableSync::COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",d." << COL_NAME << (raisingOrder ? " ASC" : " DESC")
				;
			}
			else if (orderByName)
			{
				query << " ORDER BY d." << COL_NAME << (raisingOrder ? " ASC" : " DESC");
				if(localizationid != 0)
				{
					query <<
						",c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
						<< ",s." << PhysicalStopTableSync::COL_NAME << (raisingOrder ? " ASC" : " DESC")
					;
			}	}
			else if (orderByType)
			{
			    query <<
			    	" ORDER BY (SELECT " << DisplayTypeTableSync::COL_NAME << " FROM " <<
			    	DisplayTypeTableSync::TABLE.NAME << " WHERE " << 
			    	DisplayTypeTableSync::TABLE.NAME << "." << TABLE_COL_ID << " = d." << 
			    	COL_TYPE_ID << (raisingOrder ? ") ASC" : ") DESC");
				if(localizationid != 0)
				{
					query << ",c." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC")
					<< ",s." << PhysicalStopTableSync::COL_NAME << (raisingOrder ? " ASC" : " DESC");
				}
				query << ",d." << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			}
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		DisplayScreenTableSync::SearchResult DisplayScreenTableSync::SearchFromCPU(
			util::Env& env,
			util::RegistryKeyType cpuId,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			stringstream query;
			query <<
				" SELECT"
				<< " *"
				<< " FROM "
				<< TABLE.NAME <<
				" WHERE " <<
				COL_CPU_HOST_ID << "=" << cpuId <<
				" ORDER BY " <<
				COL_COM_PORT << "," << COL_WIRING_CODE
			;

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		vector<boost::shared_ptr<SentAlarm> > DisplayScreenTableSync::GetCurrentDisplayedMessage(
			util::Env& env,
			util::RegistryKeyType screenId,
			int limit
		){
			DateTime now(TIME_CURRENT);
			stringstream q;
			q	<< "SELECT " << AlarmObjectLinkTableSync::COL_ALARM_ID
				<< " FROM " << AlarmObjectLinkTableSync::TABLE.NAME << " AS aol "
				<< " INNER JOIN " << AlarmTableSync::TABLE.NAME << " AS a ON a." << TABLE_COL_ID << "=aol." << AlarmObjectLinkTableSync::COL_ALARM_ID
				<< " INNER JOIN " << ScenarioTableSync::TABLE.NAME << " AS s ON s." << TABLE_COL_ID << "=a." << AlarmTableSync::COL_SCENARIO_ID
				<< " WHERE aol." << AlarmObjectLinkTableSync::COL_OBJECT_ID << "=" << screenId
				<< " AND s." << ScenarioTableSync::COL_ENABLED
				<< " AND NOT s." << ScenarioTableSync::COL_IS_TEMPLATE
				<< " AND (s." << ScenarioTableSync::COL_PERIODSTART << " IS NULL OR s." << ScenarioTableSync::COL_PERIODSTART << "<" << now.toSQLString() << ")"
				<< " AND (s." << ScenarioTableSync::COL_PERIODEND << " IS NULL OR s." << ScenarioTableSync::COL_PERIODEND << ">" << now.toSQLString() << ")"
				<< " ORDER BY a." << AlarmTableSync::COL_LEVEL << " DESC, s." << ScenarioTableSync::COL_PERIODSTART << " DESC";
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
		){
			stringstream q;
			q	<< "SELECT l." << TABLE_COL_ID
				<< " FROM " << TABLE.NAME << " AS d"
				<< " INNER JOIN " << PhysicalStopTableSync::TABLE.NAME << " AS s ON s." << PhysicalStopTableSync::COL_PLACEID << "=d." << COL_PLACE_ID
				<< " INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS l ON l." << LineStopTableSync::COL_PHYSICALSTOPID << "=s." << TABLE_COL_ID
				<< " WHERE d." << TABLE_COL_ID << "=" << screenId
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
				<< " INNER JOIN " << ScenarioTableSync::TABLE.NAME << " AS s ON s." << TABLE_COL_ID << "=a." << AlarmTableSync::COL_SCENARIO_ID
				<< " WHERE aol." << AlarmObjectLinkTableSync::COL_OBJECT_ID << "=" << screenId
				<< " AND s." << ScenarioTableSync::COL_ENABLED
				<< " AND NOT s." << ScenarioTableSync::COL_IS_TEMPLATE
				<< " AND s." << ScenarioTableSync::COL_PERIODSTART << ">" << now.toSQLString()
				<< " ORDER BY s." << ScenarioTableSync::COL_PERIODSTART;
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
}	}	}
