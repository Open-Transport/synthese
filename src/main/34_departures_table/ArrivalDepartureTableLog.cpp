
/** ArrivalDepartureTableLog class implementation.
	@file ArrivalDepartureTableLog.cpp

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

#include "34_departures_table/ArrivalDepartureTableLog.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayScreenTableSync.h"

#include "12_security/User.h"

using namespace boost;

namespace synthese
{
	using namespace dblog;
	using namespace security;
	using namespace departurestable;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBLog, ArrivalDepartureTableLog>::FACTORY_KEY = "departurestable";
	}

	namespace departurestable
	{
		DBLog::ColumnsVector ArrivalDepartureTableLog::getColumnNames() const
		{
			DBLog::ColumnsVector v;
			v.push_back("Action");
			return v;
		}

		void ArrivalDepartureTableLog::addUpdateEntry( boost::shared_ptr<const DisplayScreen> screen , const std::string& text, shared_ptr<const User> user )
		{
			DBLogEntry::Content content;
			content.push_back(text);
			_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, content, user, screen->getKey());
		}

		std::string ArrivalDepartureTableLog::getObjectName( uid id ) const
		{
			try
			{
				shared_ptr<DisplayScreen> ds = DisplayScreenTableSync::get(id);
				return ds->getFullName();
			}
			catch(...)
			{
				return "(unknown)";
			}
		}

		std::string ArrivalDepartureTableLog::getName() const
		{
			return "Administration des terminaux d'affichage (tableaux de départs)";
		}
	}
}
