
/** InterfaceTableSync class implementation.
	@file InterfaceTableSync.cpp

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

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"

#include "11_interfaces/InterfaceTableSync.h"

using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace interfaces;

	namespace util
	{
		template<> const std::string FactorableTemplate<SQLiteTableSync,InterfaceTableSync>::FACTORY_KEY("16.01 Interfaces");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<InterfaceTableSync>::TABLE_NAME = "t024_interfaces";
		template<> const int SQLiteTableSyncTemplate<InterfaceTableSync>::TABLE_ID = 24;
		template<> const bool SQLiteTableSyncTemplate<InterfaceTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<InterfaceTableSync,Interface>::load(Interface* interf, const db::SQLiteResultSPtr& rows)
		{
			interf->setKey(rows->getLongLong ( TABLE_COL_ID));
			interf->setNoSessionDefaultPageCode(rows->getText ( InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE));
			interf->setName(rows->getText ( InterfaceTableSync::TABLE_COL_NAME));
		}


		template<> void SQLiteDirectTableSyncTemplate<InterfaceTableSync,Interface>::save(Interface* interf)
		{
			/// @todo Implementation
		}

		template<> void SQLiteDirectTableSyncTemplate<InterfaceTableSync,Interface>::_link(Interface* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{

		}

		template<> void SQLiteDirectTableSyncTemplate<InterfaceTableSync, Interface>::_unlink(Interface* obj)
		{

		}

	}

	namespace interfaces
	{
		const std::string InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE = "no_session_default_page";
		const std::string InterfaceTableSync::TABLE_COL_NAME = "name";

		InterfaceTableSync::InterfaceTableSync()
			: db::SQLiteRegistryTableSyncTemplate<InterfaceTableSync,Interface> ()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NO_SESSION_DEFAULT_PAGE, "TEXT", true);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
		}
	}
}



