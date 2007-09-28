/** InterfacePageTableSync class implementation.
	@file InterfacePageTableSync.cpp

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

#include "InterfacePageTableSync.h"

#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/Interface.h"

#include "01_util/Conversion.h"
#include "01_util/Log.h"
#include "01_util/Factory.h"

#include "02_db/Constants.h"
#include "02_db/SQLiteResult.h"

using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace interfaces;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<InterfacePage>::TABLE_NAME = "t023_interface_pages";
		template<> const int SQLiteTableSyncTemplate<InterfacePage>::TABLE_ID = 23;
		template<> const bool SQLiteTableSyncTemplate<InterfacePage>::HAS_AUTO_INCREMENT = true;



		template<> void SQLiteTableSyncTemplate<InterfacePage>::load(InterfacePage* page, const db::SQLiteResultSPtr& rows )
		{
			page->setKey(rows->getLongLong (TABLE_COL_ID));
			page->setCode(rows->getText (InterfacePageTableSync::TABLE_COL_PAGE));
			page->parse(rows->getText (InterfacePageTableSync::TABLE_COL_CONTENT));
		}



		template<> void SQLiteTableSyncTemplate<InterfacePage>::save(InterfacePage* object)
		{

		}
	}

	namespace interfaces
	{
		const std::string InterfacePageTableSync::TABLE_COL_INTERFACE = "interface_id";
		const std::string InterfacePageTableSync::TABLE_COL_PAGE = "page_code";
		const std::string InterfacePageTableSync::TABLE_COL_CONTENT = "content";

		InterfacePageTableSync::InterfacePageTableSync()
			: SQLiteTableSyncTemplate<InterfacePage> (true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_INTERFACE, "INTEGER", false);
			addTableColumn(TABLE_COL_PAGE, "TEXT", false);
			addTableColumn(TABLE_COL_CONTENT, "TEXT", true);
		}


		void InterfacePageTableSync::rowsUpdated( SQLite* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
			    shared_ptr<InterfacePage> page = InterfacePage::GetUpdateable (rows->getLongLong (TABLE_COL_ID));
				load(page.get(), rows);
			}
		}


		void InterfacePageTableSync::rowsAdded( SQLite* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
			    // Search the specified interface
			    try
			    {
					InterfacePage* page(
						Factory<InterfacePage>::contains(rows->getText(TABLE_COL_PAGE))
						? Factory<InterfacePage>::create(rows->getText(TABLE_COL_PAGE))
						: new InterfacePage
					);

					load(page, rows);
					page->store();
				
					shared_ptr<Interface> interf = Interface::GetUpdateable(
						rows->getLongLong ( TABLE_COL_INTERFACE )
					);
					
					interf->addPage(rows->getText ( TABLE_COL_PAGE), page );
				}
				catch (Interface::RegistryKeyException& e)
				{
					Log::GetInstance().warn("Corrupted data on "+ TABLE_NAME +" table : Interface not found", e);
				}
			}
		}


		void InterfacePageTableSync::rowsRemoved( SQLite* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				/// @todo to be reimplemented
				Interface::GetUpdateable (rows->getLongLong ( TABLE_COL_INTERFACE))
				    ->removePage( rows->getText ( TABLE_COL_PAGE) );
				InterfacePage::Remove(rows->getLongLong(TABLE_COL_ID));
			}
		}
	}
}



