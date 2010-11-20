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

#include "SelectQuery.hpp"
#include "ReplaceQuery.h"
#include "SQLExpression.hpp"
#include "DisplayScreenTableSync.h"
#include "DisplayTypeTableSync.h"
#include "DisplayType.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "LineStopTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "CityTableSync.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "JourneyPattern.hpp"
#include "DBLogEntryTableSync.h"
#include "SentAlarm.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "ScenarioTableSync.h"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace departure_boards;
	using namespace pt;
	using namespace dblog;
	using namespace security;
	using namespace messages;
	using namespace geography;
	using namespace pt;
		

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, DisplayScreenTableSync>::FACTORY_KEY("54.50 Display Screens");
	}

	namespace departure_boards
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
		const string DisplayScreenTableSync::COL_UP_ID("up_id");
		const string DisplayScreenTableSync::COL_SUB_SCREEN_TYPE("sub_screen_type");
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
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_UP_ID, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenTableSync::COL_SUB_SCREEN_TYPE, SQL_INTEGER),
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
			object->setName(rows->getText ( DisplayScreenTableSync::COL_NAME));
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
			object->setSubScreenType(static_cast<DisplayScreen::SubScreenType>(rows->getInt(DisplayScreenTableSync::COL_SUB_SCREEN_TYPE)));
			object->setNullRoot();
			object->setDisplayedPlace(NULL);
			object->setType(NULL);
			object->clearForbiddenPlaces();
			object->clearDisplayedPlaces();
			object->clearForcedDestinations();
			object->clearPhysicalStops();
			object->clearTransferDestinations();

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Displayed place
				RegistryKeyType placeId(rows->getLongLong( DisplayScreenTableSync::COL_PLACE_ID));
				if(placeId != 0) try
				{
					object->setDisplayedPlace(StopAreaTableSync::Get(placeId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<StopArea>&)
				{
					Log::GetInstance().warn(
						"Data corrupted in "+ TABLE.NAME + " on display screen : localization "+ lexical_cast<string>(placeId) + " not found"
					);
				}

				// Up & root
				RegistryKeyType upId(rows->getLongLong(DisplayScreenTableSync::COL_UP_ID));
				if(upId > 0) try
				{
					DisplayScreen::SetParent(*object, DisplayScreenTableSync::GetEditable(upId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<DisplayScreen>&)
				{
					Log::GetInstance().warn(
						"Data corrupted in "+ TABLE.NAME + " on display screen : up display screen " +
						lexical_cast<string>(upId) + " not found"
					);
				}
				else
				{
					// CPU
					RegistryKeyType cpuId(rows->getLongLong(DisplayScreenTableSync::COL_CPU_HOST_ID));
					if (cpuId > 0) try
					{
						object->setRoot(DisplayScreenCPUTableSync::GetEditable(cpuId, env, linkLevel).get());
						DisplayScreenCPUTableSync::GetEditable(cpuId, env, linkLevel)->addWiredScreen(object);
					}
					catch(ObjectNotFoundException<StopArea>&)
					{
						Log::GetInstance().warn(
							"Data corrupted in "+ TABLE.NAME + " on display screen : cpu host " +
							lexical_cast<string>(cpuId) + " not found"
						);
					}
					else
					{
						object->setRoot(static_cast<NamedPlace*>(const_cast<StopArea*>(object->getDisplayedPlace())));
					}
				}


				// Type
				RegistryKeyType typeId(rows->getLongLong ( DisplayScreenTableSync::COL_TYPE_ID));
				if (typeId > 0) try
				{
					object->setType(DisplayTypeTableSync::Get(typeId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<DisplayType>&)
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
						object->addPhysicalStop(StopPointTableSync::Get(id, env, linkLevel).get());
					}
					catch (ObjectNotFoundException<StopPoint>&)
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
						object->addForbiddenPlace(StopAreaTableSync::Get(Conversion::ToLongLong(stop), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<StopArea>& e)
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
						object->addDisplayedPlace(StopAreaTableSync::Get(Conversion::ToLongLong(stop), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<StopArea>& e)
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
						object->addForcedDestination(StopAreaTableSync::Get(Conversion::ToLongLong(stop), env, linkLevel).get());
					}
					catch (ObjectNotFoundException<StopArea>& e)
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
							StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(id1), env, linkLevel).get(),
							StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(id2), env, linkLevel).get()
						);
					}
					catch (ObjectNotFoundException<StopArea>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayScreenTableSync::COL_FORCED_DESTINATIONS_IDS, e);
					}
				}
			
		}	}



		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::Unlink(
			DisplayScreen* object
		){
			if(object->getRoot<DisplayScreenCPU>() && !object->getParent())
			{
				const_cast<DisplayScreenCPU*>(object->getRoot<DisplayScreenCPU>())->removeWiredScreen(object);
			}
			DisplayScreen::SetParent(*object, NULL);
		}



		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenTableSync,DisplayScreen>::Save(
			DisplayScreen* object,
			optional<SQLiteTransaction&> transaction
		){
			// Physical stops
			stringstream psstream;
			bool first(true);
			const ArrivalDepartureTableGenerator::PhysicalStops& pss = object->getPhysicalStops(false);
			BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& itp, pss)
			{
				assert(itp.second->getKey() > 0);
				if(!first)
				{
					psstream << ",";
				}
				psstream << itp.first;
				first = false;
			}

			// Forbidden places
			stringstream fpstream;
			first = true;
			for (ForbiddenPlacesList::const_iterator itf = object->getForbiddenPlaces().begin(); itf != object->getForbiddenPlaces().end(); ++itf)
			{
				assert(itf->second->getKey() > 0);
				if(!first)
				{
					fpstream << ",";
				}
				fpstream << *itf->first;
				first = false;
			}

			// Lines filter
			stringstream lfstream;
			first = true;
			for (LineFilter::const_iterator itl = object->getForbiddenLines().begin(); itl != object->getForbiddenLines().end(); ++itl)
			{
				assert(itl->second->getKey() > 0);
				if(!first)
				{
					lfstream << ",";
				}
				lfstream << itl->first;
				first = false;
			}

			// Displayed places
			stringstream dpstream;
			first = true;
			BOOST_FOREACH(const DisplayedPlacesList::value_type& itd, object->getDisplayedPlaces())
			{
				assert(itd.second->getKey() > 0);
				if (!first)
				{
					dpstream << ",";
				}
				dpstream << itd.second->getKey();
				first = false;
			}

			// Forced destinations
			stringstream fdstream;
			first = true;
			BOOST_FOREACH(const DisplayedPlacesList::value_type& itd2, object->getForcedDestinations())
			{
				assert(itd2.second->getKey() > 0);
				if (!first)
				{
					fdstream << ",";
				}
				fdstream << itd2.second->getKey();
				first = false;
			}

			// Transfer destinations
			stringstream tdstream;
			first = true;
			BOOST_FOREACH(const TransferDestinationsList::value_type& it, object->getTransferdestinations())
			{
				BOOST_FOREACH(const TransferDestinationsList::mapped_type::value_type& it2, it.second)
				{
					if(!first)
					{
						tdstream << ",";
					}
					tdstream << it.first->getKey() << ":" << it2->getKey();
					first = false;
				}
			}


			// Query
			ReplaceQuery<DisplayScreenTableSync> query(*object);
			query.addField(object->getDisplayedPlace() ? object->getDisplayedPlace()->getKey() : RegistryKeyType(0));
			query.addField(object->getName());
			query.addField(object->getType() ? object->getType()->getKey() : RegistryKeyType(0));
			query.addField(object->getWiringCode());
			query.addField(object->getTitle());
			query.addField(object->getBlinkingDelay());
			query.addField(object->getTrackNumberDisplay());
			query.addField(object->getServiceNumberDisplay());
			query.addField(object->getDisplayTeam());
			query.addField(psstream.str());
			query.addField(object->getAllPhysicalStopsDisplayed());
			query.addField(fpstream.str());
			query.addField(lfstream.str());
			query.addField(static_cast<int>(object->getDirection()));
			query.addField(static_cast<int>(object->getEndFilter()));
			query.addField(dpstream.str());
			query.addField(object->getMaxDelay());
			query.addField(object->getClearingDelay());
			query.addField(object->getFirstRow());
			query.addField(static_cast<int>(object->getGenerationMethod()));
			query.addField(fdstream.str());
			query.addField(object->getForceDestinationDelay());
			query.addField(string());
			query.addField(object->getIsOnline());
			query.addField(object->getMaintenanceMessage());
			query.addField(object->getDisplayClock());
			query.addField(object->getComPort());
			query.addField((object->getRoot<DisplayScreenCPU>() && !object->getParent()) ? object->getRoot<DisplayScreenCPU>()->getKey() : RegistryKeyType(0));
			query.addField(object->getMacAddress());
			query.addField(object->getRoutePlanningWithTransfer());
			query.addField(tdstream.str());
			query.addField(object->getParent() ? object->getParent()->getKey() : RegistryKeyType(0));
			query.addField(static_cast<int>(object->getSubScreenType()));
			query.execute(transaction);
	}	}

	namespace departure_boards
	{
	    DisplayScreenTableSync::SearchResult DisplayScreenTableSync::Search(
			Env& env,
			optional<const security::RightsOfSameClassMap&> rights 
			, bool totalControl 
			, RightLevel neededLevel,
			optional<RegistryKeyType> duid,
			optional<RegistryKeyType> localizationid,
			optional<RegistryKeyType> lineid,
			optional<RegistryKeyType> typeuid,
			string cityName
			, string stopName
			, string name,
			optional<int> state,
			optional<int> message,
			size_t first /*= 0*/,
			boost::optional<std::size_t> number
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
			SelectQuery<DisplayScreenTableSync> query;

			// Tables
			if(!localizationid || *localizationid != 0)
			{
				query.addTableAndEqualJoin<StopAreaTableSync>(TABLE_COL_ID, COL_PLACE_ID);
				query.addTableAndEqualOtherJoin<CityTableSync,StopAreaTableSync>(TABLE_COL_ID, StopAreaTableSync::TABLE_COL_CITYID);
				query.addTableAndEqualOtherJoin<StopPointTableSync,StopAreaTableSync>(StopPointTableSync::COL_PLACEID, TABLE_COL_ID);
			
				if (lineid || neededLevel > FORBIDDEN)
				{
					query.addTableAndEqualOtherJoin<LineStopTableSync,StopPointTableSync>(LineStopTableSync::COL_PHYSICALSTOPID, TABLE_COL_ID);
					query.addTableAndEqualOtherJoin<JourneyPatternTableSync,LineStopTableSync>(TABLE_COL_ID, LineStopTableSync::COL_LINEID);
				}

				if(orderByType)
				{
					query.addTableAndEqualJoin<DisplayTypeTableSync>(TABLE_COL_ID, COL_TYPE_ID);
				}
			}
			
			// Filtering
			if(!localizationid || *localizationid != 0)
			{
				if (neededLevel > FORBIDDEN && rights)
				{
					query.addWhere(
						ComposedExpression::Get(
							FieldExpression::Get(
								JourneyPatternTableSync::TABLE.NAME, JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID
							),
							ComposedExpression::OP_IN,
							SubQueryExpression::Get(
								CommercialLineTableSync::getSQLLinesList(*rights, totalControl, neededLevel, false)
					)	)	);
				}
				if (!cityName.empty())
				{
					query.addWhereFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, "%"+cityName+"%", ComposedExpression::OP_LIKE);
				}
				if (!stopName.empty())
				{
					query.addWhereFieldOther<StopAreaTableSync>(StopAreaTableSync::TABLE_COL_NAME, "%"+stopName+"%", ComposedExpression::OP_LIKE);
				}
				if (lineid)
				{
					query.addWhereFieldOther<JourneyPatternTableSync>(JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID, *lineid);
				}
			}
			if(state)
			{
				/// @todo write status filter
			}
			if(message)
			{
				/// @todo write message filter
			}
			if (!name.empty())
			{
				query.addWhereField(COL_NAME, "%"+name+"%", ComposedExpression::OP_LIKE);
			}
			if (duid)
			{
				query.addWhereField(TABLE_COL_ID, *duid);
			}
			if (localizationid)
			{
				query.addWhereField(COL_PLACE_ID, *localizationid);
			}
			if (typeuid)
			{
				query.addWhereField(COL_TYPE_ID, *typeuid);
			}
			
			// Grouping
			query.addGroupByField();
			
			// Ordering
			if (orderByUid)
			{
				query.addOrderField(TABLE_COL_ID, raisingOrder);
			}
			else if ((!localizationid || *localizationid != 0) && orderByCity)
			{
				query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderFieldOther<StopAreaTableSync>(StopAreaTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderField(COL_NAME, raisingOrder);
			}
			else if ((!localizationid || *localizationid != 0) && orderByStopName)
			{
				query.addOrderFieldOther<StopAreaTableSync>(StopAreaTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderField(COL_NAME, raisingOrder);
			}
			else if (orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
				if(!localizationid || *localizationid != 0)
				{
					query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
					query.addOrderFieldOther<StopAreaTableSync>(StopAreaTableSync::TABLE_COL_NAME, raisingOrder);
			}	}
			else if (orderByType)
			{
			    query.addOrderFieldOther<DisplayTypeTableSync>(DisplayTypeTableSync::COL_NAME, raisingOrder);
				if(!localizationid || *localizationid != 0)
				{
					query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
					query.addOrderFieldOther<StopAreaTableSync>(StopAreaTableSync::TABLE_COL_NAME, raisingOrder);
				}
				query.addOrderField(COL_NAME, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}



		DisplayScreenTableSync::SearchResult DisplayScreenTableSync::SearchFromCPU(
			util::Env& env,
			util::RegistryKeyType cpuId,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			SelectQuery<DisplayScreenTableSync> query;
			query.addWhereField(COL_CPU_HOST_ID, cpuId);
			query.addOrderField(COL_COM_PORT, true);
			query.addOrderField(COL_WIRING_CODE, true);
			return LoadFromQuery(query, env, linkLevel);
		}



		vector<boost::shared_ptr<SentAlarm> > DisplayScreenTableSync::GetCurrentDisplayedMessage(
			util::Env& env,
			util::RegistryKeyType screenId,
			optional<int> limit
		){
			ptime now(second_clock::local_time());

			SelectQuery<AlarmObjectLinkTableSync> query;
			query.addTableField(AlarmObjectLinkTableSync::COL_ALARM_ID);
			query.addTableAndEqualJoin<AlarmTableSync>(TABLE_COL_ID, AlarmObjectLinkTableSync::COL_ALARM_ID);
			query.addTableAndEqualOtherJoin<ScenarioTableSync, AlarmTableSync>(TABLE_COL_ID, AlarmTableSync::COL_SCENARIO_ID);
			query.addWhereField(AlarmObjectLinkTableSync::COL_OBJECT_ID, screenId);
			query.addWhereFieldOther<ScenarioTableSync>(ScenarioTableSync::COL_ENABLED, 1);
			query.addWhere(
				NotExpression::Get(
					FieldExpression::Get(
						ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_IS_TEMPLATE
			)	)	);
			query.addWhere(
				ComposedExpression::Get(
					IsNullExpression::Get(
						FieldExpression::Get(
							ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_PERIODSTART
					)	),
					ComposedExpression::OP_OR,
					ComposedExpression::Get(
						FieldExpression::Get(
							ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_PERIODSTART
						),
						ComposedExpression::OP_INF,
						ValueExpression<ptime>::Get(now)
					)
			)	);
			query.addWhere(
				ComposedExpression::Get(
					IsNullExpression::Get(
						FieldExpression::Get(
							ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_PERIODEND
					)	),
					ComposedExpression::OP_OR,
					ComposedExpression::Get(
						FieldExpression::Get(
							ScenarioTableSync::TABLE.NAME, ScenarioTableSync::COL_PERIODEND
						),
						ComposedExpression::OP_SUP,
						ValueExpression<ptime>::Get(now)
					)
			)	);
			query.addOrderFieldOther<AlarmTableSync>(AlarmTableSync::COL_LEVEL, false);
			query.addOrderFieldOther<ScenarioTableSync>(ScenarioTableSync::COL_PERIODSTART, false);
			if (limit)
			{
				query.setNumber(*limit);
			}
			SQLiteResultSPtr rows = query.execute();
			vector<shared_ptr<SentAlarm> > result;
			while(rows->next())
			{
				result.push_back(
					static_pointer_cast<SentAlarm,Alarm>(
						AlarmTableSync::GetEditable(
							rows->getLongLong(AlarmObjectLinkTableSync::COL_ALARM_ID),
							env
				)	)	);
			}
			return result;
		}



		bool DisplayScreenTableSync::GetIsAtLeastALineDisplayed(
			util::RegistryKeyType screenId
		){
			stringstream q;
			q	<< "SELECT l." << TABLE_COL_ID
				<< " FROM " << TABLE.NAME << " AS d"
				<< " INNER JOIN " << StopPointTableSync::TABLE.NAME << " AS s ON s." << StopPointTableSync::COL_PLACEID << "=d." << COL_PLACE_ID
				<< " INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS l ON l." << LineStopTableSync::COL_PHYSICALSTOPID << "=s." << TABLE_COL_ID
				<< " WHERE d." << TABLE_COL_ID << "=" << screenId
				<< " AND (d." << COL_ALL_PHYSICAL_DISPLAYED << " OR d." << COL_PHYSICAL_STOPS_IDS << " LIKE ('%'|| s." << TABLE_COL_ID << " ||'%'))"
				<< " AND (l." << LineStopTableSync::COL_ISDEPARTURE << " AND d." << COL_DIRECTION << " OR l." << LineStopTableSync::COL_ISARRIVAL << " AND NOT d." << COL_DIRECTION << ")"
				<< " AND (NOT d." << COL_ORIGINS_ONLY << " OR l." << LineStopTableSync::COL_RANKINPATH << "=0)"
				<< " AND NOT EXISTS(SELECT p2." << StopPointTableSync::COL_PLACEID << " FROM " << StopPointTableSync::TABLE.NAME << " AS p2 INNER JOIN " << LineStopTableSync::TABLE.NAME << " AS l2 ON l2." << LineStopTableSync::COL_PHYSICALSTOPID << "=p2." << TABLE_COL_ID
				<< " WHERE l2." << LineStopTableSync::COL_LINEID << "=l." << LineStopTableSync::COL_LINEID
				<< " AND l2." << LineStopTableSync::COL_RANKINPATH << ">l." << LineStopTableSync::COL_RANKINPATH
				<< " AND l2." << LineStopTableSync::COL_ISARRIVAL
				<< " AND ('%'|| p2." << StopPointTableSync::COL_PLACEID << " ||'%') LIKE d." << COL_FORBIDDEN_ARRIVAL_PLACES_IDS
				<< ")"
				<< " LIMIT 1";
			SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(q.str());
			return rows->next();
		}



		vector<shared_ptr<SentAlarm> > DisplayScreenTableSync::GetFutureDisplayedMessages(
			Env& env,
			RegistryKeyType screenId,
			optional<int> number
		){
			ptime now(second_clock::local_time());
			stringstream q;
			q	<< "SELECT " << AlarmObjectLinkTableSync::COL_ALARM_ID
				<< " FROM " << AlarmObjectLinkTableSync::TABLE.NAME << " AS aol "
				<< " INNER JOIN " << AlarmTableSync::TABLE.NAME << " AS a ON a." << TABLE_COL_ID << "=aol." << AlarmObjectLinkTableSync::COL_ALARM_ID
				<< " INNER JOIN " << ScenarioTableSync::TABLE.NAME << " AS s ON s." << TABLE_COL_ID << "=a." << AlarmTableSync::COL_SCENARIO_ID
				<< " WHERE aol." << AlarmObjectLinkTableSync::COL_OBJECT_ID << "=" << screenId
				<< " AND s." << ScenarioTableSync::COL_ENABLED
				<< " AND NOT s." << ScenarioTableSync::COL_IS_TEMPLATE
				<< " AND s." << ScenarioTableSync::COL_PERIODSTART << ">'" << to_iso_extended_string(now.date()) << " " << to_simple_string(now.time_of_day()) << "'"
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
		}



		shared_ptr<DisplayScreen> DisplayScreenTableSync::GetByMACAddress(
			util::Env& env,
			const std::string& macAddress,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */ )
		{
			SelectQuery<DisplayScreenTableSync> query;
			query.addWhereField(COL_MAC_ADDRESS, macAddress);
			query.setNumber(1);
			SQLiteResultSPtr rows = query.execute();
			if(rows->next ())
			{
				util::Registry<DisplayScreen>& registry(env.getEditableRegistry<DisplayScreen>());
				if(registry.contains(rows->getKey()))
				{
					return registry.getEditable(rows->getKey());
				}
				else
				{
					boost::shared_ptr<DisplayScreen> object(DisplayScreenTableSync::GetNewObject(rows));
					registry.add(object);
					Load(object.get(), rows, env, linkLevel);
					return object;
				}
			}
			throw Exception("Display screen not found");
}	}	}
