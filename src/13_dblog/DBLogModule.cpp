
/** DBLogModule class implementation.
	@file DBLogModule.cpp

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

#include "13_dblog/DBLogModule.h"

#include "05_html/Constants.h"

#include "01_util/Constants.h"

using namespace std;

namespace synthese
{
	using namespace html;

	namespace util
	{
		template<> const std::string util::FactorableTemplate<ModuleClass, dblog::DBLogModule>::FACTORY_KEY("13_dblog");
	}


	namespace dblog
	{

		vector<pair<int, std::string> > DBLogModule::getEntryLevelLabels( bool withAll/*=false*/ )
		{
			vector<pair<int, std::string> > m;
			if (withAll)
				m.push_back(make_pair((int) DBLogEntry::DB_LOG_UNKNOWN, "(tous)"));
			m.push_back(make_pair((int) DBLogEntry::DB_LOG_INFO, getEntryLevelLabel(DBLogEntry::DB_LOG_INFO)));
			m.push_back(make_pair((int) DBLogEntry::DB_LOG_WARNING, getEntryLevelLabel(DBLogEntry::DB_LOG_WARNING)));
			m.push_back(make_pair((int) DBLogEntry::DB_LOG_ERROR, getEntryLevelLabel(DBLogEntry::DB_LOG_ERROR)));
			return m;
		}

		std::string DBLogModule::getEntryLevelLabel( const DBLogEntry::Level& level )
		{
			switch (level)
			{
			case DBLogEntry::DB_LOG_UNKNOWN : return "(inconnu)";
			case DBLogEntry::DB_LOG_INFO : return "Information";
			case DBLogEntry::DB_LOG_WARNING : return "Alerte";
			case DBLogEntry::DB_LOG_ERROR : return "Erreur";
			}
		}

		std::string DBLogModule::getEntryIcon( const DBLogEntry::Level& level )
		{
			switch(level)
			{
			case DBLogEntry::DB_LOG_OK : return IMG_URL_OK;
			case DBLogEntry::DB_LOG_INFO : return IMG_URL_INFO;
			case DBLogEntry::DB_LOG_WARNING : return IMG_URL_WARNING;
			case DBLogEntry::DB_LOG_ERROR : return IMG_URL_ERROR;
			default : return string();
			}
		}

		void DBLogModule::initialize()
		{

		}

		std::string DBLogModule::getName() const
		{
			return "Journaux";
		}
	}
}
