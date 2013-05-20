////////////////////////////////////////////////////////////////////////////////
/// DisplayTypeTableSync class implementation.
///	@file DisplayTypeTableSync.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "DisplayTypeTableSync.h"

#include "ArrivalDepartureTableRight.h"
#include "ArrivalDepartureTableLog.h"
#include "Conversion.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "DisplayScreenTableSync.h"
#include "InterfaceTableSync.h"
#include "MessageTypeTableSync.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "Session.h"
#include "User.h"
#include "WebPageTableSync.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace departure_boards;
	using namespace interfaces;
	using namespace messages;
	using namespace util;
	using namespace cms;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DisplayTypeTableSync>::FACTORY_KEY("54.00 Display Types");
	}

	namespace departure_boards
	{
		const string DisplayTypeTableSync::COL_NAME = "name";
		const string DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID = "interface_id";
		const string DisplayTypeTableSync::COL_AUDIO_INTERFACE_ID = "audio_interface_id";
		const string DisplayTypeTableSync::COL_MONITORING_INTERFACE_ID = "monitoring_interface_id";
		const string DisplayTypeTableSync::COL_ROWS_NUMBER = "rows_number";
		const string DisplayTypeTableSync::COL_MAX_STOPS_NUMBER("max_stops_number");
		const string DisplayTypeTableSync::COL_TIME_BETWEEN_CHECKS("time_between_checks");
		const string DisplayTypeTableSync::COL_DISPLAY_MAIN_PAGE_ID("display_main_page_id");
		const string DisplayTypeTableSync::COL_DISPLAY_ROW_PAGE_ID("display_row_page_id");
		const string DisplayTypeTableSync::COL_DISPLAY_DESTINATION_PAGE_ID("display_destination_page_id");
		const string DisplayTypeTableSync::COL_DISPLAY_TRANSFER_DESTINATION_PAGE_ID("display_transfer_destination_page_id");
		const string DisplayTypeTableSync::COL_MONITORING_PARSER_PAGE_ID("monitoring_parser_page_id");
		const string DisplayTypeTableSync::COL_IS_DISPLAYED_MESSAGE_PAGE_ID = "is_displayed_message_page_id";
		const string DisplayTypeTableSync::COL_MESSAGE_TYPE = "message_type_id";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DisplayTypeTableSync>::TABLE(
			"t036_display_types"
		);

		template<> const Field DBTableSyncTemplate<DisplayTypeTableSync>::_FIELDS[] =
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_NAME, SQL_TEXT),
			Field(DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_AUDIO_INTERFACE_ID, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_MONITORING_INTERFACE_ID, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_ROWS_NUMBER, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_MAX_STOPS_NUMBER, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_TIME_BETWEEN_CHECKS, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_DISPLAY_MAIN_PAGE_ID, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_DISPLAY_ROW_PAGE_ID, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_DISPLAY_DESTINATION_PAGE_ID, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_DISPLAY_TRANSFER_DESTINATION_PAGE_ID, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_MONITORING_PARSER_PAGE_ID, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_IS_DISPLAYED_MESSAGE_PAGE_ID, SQL_INTEGER),
			Field(DisplayTypeTableSync::COL_MESSAGE_TYPE, SQL_INTEGER),
			Field()
		};

		template<> DBTableSync::Indexes DBTableSyncTemplate<DisplayTypeTableSync>::GetIndexes()
		{
			return DBTableSyncTemplate::Indexes();
		}

		template<> void OldLoadSavePolicy<DisplayTypeTableSync,DisplayType>::Load(
			DisplayType* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText ( DisplayTypeTableSync::COL_NAME));
			object->setRowNumber(rows->getInt ( DisplayTypeTableSync::COL_ROWS_NUMBER));
			object->setMaxStopsNumber(rows->getOptionalUnsignedInt(DisplayTypeTableSync::COL_MAX_STOPS_NUMBER));
			object->setTimeBetweenChecks(minutes(rows->getInt(DisplayTypeTableSync::COL_TIME_BETWEEN_CHECKS)));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				try
				{
					object->setDisplayInterface(NULL);
					util::RegistryKeyType id(rows->getLongLong(DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID));
					if (id > 0)
					{
						object->setDisplayInterface(InterfaceTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Interface>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayTypeTableSync::TABLE.NAME + "/" + DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID, e);
				}
				try
				{
					object->setAudioInterface(NULL);
					util::RegistryKeyType id(rows->getLongLong(DisplayTypeTableSync::COL_AUDIO_INTERFACE_ID));
					if (id > 0)
					{
						object->setAudioInterface(InterfaceTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Interface>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayTypeTableSync::TABLE.NAME + "/" + DisplayTypeTableSync::COL_AUDIO_INTERFACE_ID, e);
				}
				try
				{
					object->setMonitoringInterface(NULL);
					util::RegistryKeyType id(rows->getLongLong(DisplayTypeTableSync::COL_MONITORING_INTERFACE_ID));
					if (id > 0)
					{
						object->setMonitoringInterface(InterfaceTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Interface>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayTypeTableSync::TABLE.NAME + "/" + DisplayTypeTableSync::COL_MONITORING_INTERFACE_ID, e);
				}

				// Display pages
				try
				{
					object->setDisplayMainPage(NULL);
					util::RegistryKeyType id(rows->getLongLong(DisplayTypeTableSync::COL_DISPLAY_MAIN_PAGE_ID));
					if (id > 0)
					{
						object->setDisplayMainPage(WebPageTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Webpage>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayTypeTableSync::TABLE.NAME + "/" + DisplayTypeTableSync::COL_DISPLAY_MAIN_PAGE_ID, e);
				}

				try
				{
					object->setDisplayRowPage(NULL);
					util::RegistryKeyType id(rows->getLongLong(DisplayTypeTableSync::COL_DISPLAY_ROW_PAGE_ID));
					if (id > 0)
					{
						object->setDisplayRowPage(WebPageTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Webpage>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayTypeTableSync::TABLE.NAME + "/" + DisplayTypeTableSync::COL_DISPLAY_ROW_PAGE_ID, e);
				}

				try
				{
					object->setDisplayDestinationPage(NULL);
					util::RegistryKeyType id(rows->getLongLong(DisplayTypeTableSync::COL_DISPLAY_DESTINATION_PAGE_ID));
					if (id > 0)
					{
						object->setDisplayDestinationPage(WebPageTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Webpage>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayTypeTableSync::TABLE.NAME + "/" + DisplayTypeTableSync::COL_DISPLAY_DESTINATION_PAGE_ID, e);
				}


				try
				{
					object->setDisplayTransferDestinationPage(NULL);
					util::RegistryKeyType id(rows->getLongLong(DisplayTypeTableSync::COL_DISPLAY_TRANSFER_DESTINATION_PAGE_ID));
					if (id > 0)
					{
						object->setDisplayTransferDestinationPage(WebPageTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Webpage>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayTypeTableSync::TABLE.NAME + "/" + DisplayTypeTableSync::COL_DISPLAY_TRANSFER_DESTINATION_PAGE_ID, e);
				}

				try
				{
					object->setMessageIsDisplayedPage(NULL);
					util::RegistryKeyType id(rows->getLongLong(DisplayTypeTableSync::COL_IS_DISPLAYED_MESSAGE_PAGE_ID));
					if (id > 0)
					{
						object->setMessageIsDisplayedPage(WebPageTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Webpage>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayTypeTableSync::TABLE.NAME + "/" + DisplayTypeTableSync::COL_IS_DISPLAYED_MESSAGE_PAGE_ID, e);
				}

				try
				{
					object->setMonitoringParserPage(NULL);
					util::RegistryKeyType id(rows->getLongLong(DisplayTypeTableSync::COL_MONITORING_PARSER_PAGE_ID));
					if (id > 0)
					{
						object->setMonitoringParserPage(WebPageTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Webpage>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayTypeTableSync::TABLE.NAME + "/" + DisplayTypeTableSync::COL_MONITORING_PARSER_PAGE_ID, e);
				}

				// Message type
				try
				{
					object->setMessageType(NULL);
					RegistryKeyType id(rows->getLongLong(DisplayTypeTableSync::COL_MESSAGE_TYPE));
					if(id > 0)
					{
						object->setMessageType(
							MessageTypeTableSync::GetEditable(id, env, linkLevel).get()
						);
					}
				}
				catch (ObjectNotFoundException<MessageType>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayTypeTableSync::TABLE.NAME + "/" + DisplayTypeTableSync::COL_MESSAGE_TYPE, e);
				}
			}
		}

		template<> void OldLoadSavePolicy<DisplayTypeTableSync,DisplayType>::Unlink(
			DisplayType* obj
		){
		}



		template<> void OldLoadSavePolicy<DisplayTypeTableSync,DisplayType>::Save(
			DisplayType* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<DisplayTypeTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getDisplayInterface() ? object->getDisplayInterface()->getKey() : RegistryKeyType(0));
			query.addField(object->getAudioInterface() ? object->getAudioInterface()->getKey() : RegistryKeyType(0));
			query.addField(object->getMonitoringInterface() ? object->getMonitoringInterface()->getKey() : RegistryKeyType(0));
			query.addField(object->getRowNumber());
			query.addField(object->getMaxStopsNumber() ? lexical_cast<string>(*object->getMaxStopsNumber()) : string());
			query.addField(object->getTimeBetweenChecks().total_seconds() / 60);
			query.addField(object->getDisplayMainPage() ? object->getDisplayMainPage()->getKey() : RegistryKeyType(0));
			query.addField(object->getDisplayRowPage() ? object->getDisplayRowPage()->getKey() : RegistryKeyType(0));
			query.addField(object->getDisplayDestinationPage() ? object->getDisplayDestinationPage()->getKey() : RegistryKeyType(0));
			query.addField(object->getDisplayTransferDestinationPage() ? object->getDisplayTransferDestinationPage()->getKey() : RegistryKeyType(0));
			query.addField(object->getMonitoringParserPage() ? object->getMonitoringParserPage()->getKey() : RegistryKeyType(0));
			query.addField(object->getMessageIsDisplayedPage() ? object->getMessageIsDisplayedPage()->getKey() : RegistryKeyType(0));
			query.addField(object->getMessageType() ? object->getMessageType()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}




		template<> bool DBTableSyncTemplate<DisplayTypeTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			Env env;
			DisplayScreenTableSync::Search(
				env,
				RightsOfSameClassMap(),
				true,
				UNKNOWN_RIGHT_LEVEL,
				optional<RegistryKeyType>(),
				optional<RegistryKeyType>(),
				optional<RegistryKeyType>(),
				object_id,
				std::string(),
				std::string(),
				std::string(),
				optional<int>(),
				optional<int>(),
				0,
				1
			);
			if (!env.getRegistry<DisplayScreen>().empty())
			{
				return false;
			}

			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<DisplayTypeTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DisplayTypeTableSync>::AfterDelete(
			util::RegistryKeyType id,
			DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DisplayTypeTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			Env env;
			boost::shared_ptr<const DisplayType> typ(DisplayTypeTableSync::Get(id, env));
			ArrivalDepartureTableLog::addDeleteTypeEntry(typ.get(), session->getUser().get());
		}
	}

	namespace departure_boards
	{
		DisplayTypeTableSync::SearchResult DisplayTypeTableSync::Search(
			Env& env,
			optional<string> likeName,
			boost::optional<util::RegistryKeyType> interfaceId,
			int first, /*= 0*/
			boost::optional<std::size_t> number, /*= 0*/
			bool orderByName,
			bool orderByInterfaceName,
			bool orderByRows,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT t.*"
				<< " FROM " << TABLE.NAME << " AS t";
			if (orderByInterfaceName)
			{
				query <<
					" LEFT JOIN " << InterfaceTableSync::TABLE.NAME << " AS i ON i." << TABLE_COL_ID  <<
					"=t." << DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID
				;
			}

			query << " WHERE 1";
			if (likeName)
			{
				query << " AND t." << COL_NAME << " LIKE " << Conversion::ToDBString(*likeName);
			}
			if(interfaceId)
			{
				query << " AND t." << COL_DISPLAY_INTERFACE_ID << "=" << *interfaceId;
			}

			if (orderByName)
			{
				query << " ORDER BY t." << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			}
			else if(orderByInterfaceName)
			{
				query << " ORDER BY i." << InterfaceTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			}
			else if(orderByRows)
			{
				query << " ORDER BY t." << COL_ROWS_NUMBER << (raisingOrder ? " ASC" : " DESC");
			}

			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
