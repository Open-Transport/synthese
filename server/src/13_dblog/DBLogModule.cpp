
/** DBLogModule class implementation.
	@file DBLogModule.cpp

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

#include "DBLogModule.h"

#include "05_html/Constants.h"
#include "UtilConstants.h"

using namespace std;

namespace synthese
{
	using namespace html;
	using namespace server;
	using namespace dblog;

	namespace util
	{
		template<> const std::string util::FactorableTemplate<ModuleClass, dblog::DBLogModule>::FACTORY_KEY("13_dblog");
	}


	namespace server
	{
		template<> const string ModuleClassTemplate<DBLogModule>::NAME("Journaux");

		template<> void ModuleClassTemplate<DBLogModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<DBLogModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<DBLogModule>::End()
		{
		}


		template<> void ModuleClassTemplate<DBLogModule>::Start()
		{
		}



		template<> void ModuleClassTemplate<DBLogModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<DBLogModule>::CloseThread(
		){
		}
	}


	namespace dblog
	{

		DBLogModule::Labels DBLogModule::getEntryLevelLabels( bool withAll/*=false*/ )
		{
			Labels m;
			if (withAll)
				m.push_back(make_pair(boost::optional<int>(), "(tous)"));
			m.push_back(make_pair((int) DB_LOG_INFO, getEntryLevelLabel(DB_LOG_INFO)));
			m.push_back(make_pair((int) DB_LOG_WARNING, getEntryLevelLabel(DB_LOG_WARNING)));
			m.push_back(make_pair((int) DB_LOG_ERROR, getEntryLevelLabel(DB_LOG_ERROR)));
			return m;
		}

		std::string DBLogModule::getEntryLevelLabel( const Level& level )
		{
			switch (level)
			{
			case DB_LOG_UNKNOWN : return "(inconnu)";
			case DB_LOG_INFO : return "Information";
			case DB_LOG_WARNING : return "Alerte";
			case DB_LOG_ERROR : return "Erreur";
			case DB_LOG_OK : return "Ok";
			}
			return string();
		}

		std::string DBLogModule::getEntryIcon( const Level& level )
		{
			switch(level)
			{
			case DB_LOG_OK : return IMG_URL_OK;
			case DB_LOG_INFO : return IMG_URL_INFO;
			case DB_LOG_WARNING : return IMG_URL_WARNING;
			case DB_LOG_ERROR : return IMG_URL_ERROR;
			default : return string();
			}
		}
	}
}
