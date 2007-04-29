
/** DisplayDataControlLog class implementation.
	@file DisplayDataControlLog.cpp

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

#include "34_departures_table/DisplayDataControlLog.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DeparturesTableModule.h"

#include "12_security/User.h"

using namespace boost;

namespace synthese
{
	using namespace dblog;
	using namespace security;
	using namespace departurestable;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBLog, DisplayDataControlLog>::FACTORY_KEY = "displaydata";
		
	}

	namespace departurestable
	{


		void DisplayDataControlLog::addDataControlEntry(
			boost::shared_ptr<const DisplayScreen> screen
			, const DBLogEntry::Level& level
			, const std::string& text )
		{
			DBLogEntry::Content c;
			c.push_back(text);
			DBLog::_addEntry(
				FACTORY_KEY
				, level
				, c
				, boost::shared_ptr<const User>()
				, screen->getKey()
				);

		}

		DBLog::ColumnsVector DisplayDataControlLog::getColumnNames() const
		{
			ColumnsVector v;
			v.push_back("Texte");
			return v;
		}

		DBLog::ColumnsVector DisplayDataControlLog::parse( const dblog::DBLogEntry::Content& cols ) const
		{
			ColumnsVector v;
			v.push_back(cols.front());
			return v;
		}

		std::string DisplayDataControlLog::getObjectName( uid id ) const
		{
			// Screen
			try
			{
				shared_ptr<const DisplayScreen> screen = DeparturesTableModule::getDisplayScreens().get(id);
				return screen->getFullName();
			}
			catch (...)
			{
				return DBLog::getObjectName(id);
			}

		}

		std::string DisplayDataControlLog::getName() const
		{
			return "Contrôles de cohérence des données des afficheurs (tableaux de départs)";
		}
	}
}
