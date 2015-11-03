
/** ArrivalDepartureTableLog class implementation.
	@file ArrivalDepartureTableLog.cpp

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

#include "ArrivalDepartureTableLog.h"
#include "DisplayScreen.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenTableSync.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "User.h"
#include "ArrivalDepartureTableRight.h"
#include "Request.h"
#include "AdminFunctionRequest.hpp"
#include "DisplayAdmin.h"
#include "DisplayTypeAdmin.h"
#include "DisplayScreenCPUAdmin.h"
#include "HTMLModule.h"

#include <boost/lexical_cast.hpp>

using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace dblog;
	using namespace security;
	using namespace departure_boards;
	using namespace util;
	using namespace server;
	using namespace admin;
	using namespace html;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBLog, ArrivalDepartureTableLog>::FACTORY_KEY = "departure_boards";
	}

	namespace departure_boards
	{
		DBLog::ColumnsVector ArrivalDepartureTableLog::getColumnNames() const
		{
			DBLog::ColumnsVector v;
			v.push_back("Action");
			return v;
		}



		std::string ArrivalDepartureTableLog::getObjectName(
			util::RegistryKeyType id,
			const server::Request& searchRequest
		) const	{
			try
			{
				Env env;
				if (decodeTableId(id) == DisplayScreenTableSync::TABLE.ID)
				{
					AdminFunctionRequest<DisplayAdmin> openRequest(
						dynamic_cast<const Request& >(searchRequest)
					);
					boost::shared_ptr<const DisplayScreen> ds = DisplayScreenTableSync::Get(id, env);
					openRequest.getPage()->setScreen(ds);
					return HTMLModule::getHTMLLink(openRequest.getURL(), ds->getFullName());
				}
				else if (decodeTableId(id) == DisplayTypeTableSync::TABLE.ID)
				{
					AdminFunctionRequest<DisplayTypeAdmin> openRequest(
						dynamic_cast<const Request& >(searchRequest)
					);
					boost::shared_ptr<const DisplayType> dt(DisplayTypeTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					openRequest.getPage()->setType(dt);
					return HTMLModule::getHTMLLink(openRequest.getURL(), dt->get<Name>());
				}
				else if(decodeTableId(id) == DisplayScreenCPUTableSync::TABLE.ID)
				{
					AdminFunctionRequest<DisplayScreenCPUAdmin> openRequest(
						dynamic_cast<const Request& >(searchRequest)
					);
					boost::shared_ptr<const DisplayScreenCPU> cpu(DisplayScreenCPUTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL));
					openRequest.getPage()->setCPU(cpu);
					return HTMLModule::getHTMLLink(openRequest.getURL(), cpu->getName());
				}
			}
			catch(...)
			{
			}
			return DBLog::getObjectName(id, searchRequest);
		}



		void ArrivalDepartureTableLog::addUpdateEntry(
			const DisplayScreen& screen,
			const std::string& text, const User& user
		){
			DBLogEntry::Content content;
			content.push_back(text);
			_addEntry(FACTORY_KEY, DB_LOG_INFO, content, &user, screen.getKey());
		}



		void ArrivalDepartureTableLog::addUpdateEntry( const DisplayScreenCPU& cpu , const std::string& text , const security::User& user )
		{
			DBLogEntry::Content content;
			content.push_back(text);
			_addEntry(FACTORY_KEY, DB_LOG_INFO, content, &user, cpu.getKey());
		}




		std::string ArrivalDepartureTableLog::getName() const
		{
			return "SAI : Afficheurs";
		}

		void ArrivalDepartureTableLog::addRemoveEntry(
			const DisplayScreen* screen
			, const security::User* user
		){
			DBLogEntry::Content content;
			content.push_back("Suppression de l'afficheur " + screen->getFullName());
			_addEntry(FACTORY_KEY, DB_LOG_INFO, content, user, screen->getKey());
		}

		void ArrivalDepartureTableLog::addRemoveEntry(
			const DisplayScreenCPU& cpu,
			const security::User& user
		){
			DBLogEntry::Content content;
			content.push_back("Suppression de l'unité centrale " + cpu.getFullName());
			_addEntry(FACTORY_KEY, DB_LOG_INFO, content, &user, cpu.getKey());
		}

		void ArrivalDepartureTableLog::addUpdateTypeEntry(const DisplayType* type , const security::User* user , const std::string& text )
		{
			DBLogEntry::Content content;
			content.push_back("Mise à jour type d'afficheur " + type->get<Name>() + text);
			_addEntry(FACTORY_KEY, DB_LOG_INFO, content, user, type->getKey());
		}

		void ArrivalDepartureTableLog::addCreateEntry(const DisplayType& type , const security::User& user )
		{
			DBLogEntry::Content content;
			content.push_back("Création type d'afficheur " + type.get<Name>());
			_addEntry(FACTORY_KEY, DB_LOG_INFO, content, &user, type.getKey());
		}

		void ArrivalDepartureTableLog::addDeleteTypeEntry(const DisplayType* type , const security::User* user )
		{
			DBLogEntry::Content content;
			content.push_back("Suppression type d'afficheur " + type->get<Name>());
			_addEntry(FACTORY_KEY, DB_LOG_INFO, content, user, type->getKey());
		}

		void ArrivalDepartureTableLog::addCreateEntry( const DisplayScreenCPU& cpu , const security::User& user )
		{
			DBLogEntry::Content content;
			content.push_back("Création");
			_addEntry(FACTORY_KEY, DB_LOG_INFO, content, &user, cpu.getKey());
		}

		void ArrivalDepartureTableLog::addCreateEntry( const DisplayScreen& cpu , const security::User& user )
		{
			DBLogEntry::Content content;
			content.push_back("Création");
			_addEntry(FACTORY_KEY, DB_LOG_INFO, content, &user, cpu.getKey());
		}
	}
}
