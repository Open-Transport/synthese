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
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,InterfacePageTableSync>::FACTORY_KEY("16.02 Interface Pages");
	}

	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<InterfacePageTableSync>::TABLE_NAME = "t023_interface_pages";
		template<> const int SQLiteTableSyncTemplate<InterfacePageTableSync>::TABLE_ID = 23;
		template<> const bool SQLiteTableSyncTemplate<InterfacePageTableSync>::HAS_AUTO_INCREMENT = true;



		template<> void SQLiteDirectTableSyncTemplate<InterfacePageTableSync,InterfacePage>::load(InterfacePage* page, const db::SQLiteResultSPtr& rows )
		{
			page->setKey(rows->getLongLong (TABLE_COL_ID));
			page->setPageCode(rows->getText(InterfacePageTableSync::TABLE_COL_PAGE));
			page->setDirectDisplayAllowed(rows->getBool(InterfacePageTableSync::TABLE_COL_DIRECT_DISPLAY_ALLOWED));
			page->parse(rows->getText (InterfacePageTableSync::TABLE_COL_CONTENT));
		}

		template<> void SQLiteDirectTableSyncTemplate<InterfacePageTableSync,InterfacePage>::_link(InterfacePage* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{

		}

		template<> void SQLiteDirectTableSyncTemplate<InterfacePageTableSync,InterfacePage>::_unlink(InterfacePage* obj)
		{

		}

		template<> void SQLiteDirectTableSyncTemplate<InterfacePageTableSync,InterfacePage>::save(InterfacePage* object)
		{

		}
	}

	namespace interfaces
	{
		const string InterfacePageTableSync::TABLE_COL_INTERFACE = "interface_id";
		const string InterfacePageTableSync::TABLE_COL_CLASS = "class_code";
		const string InterfacePageTableSync::TABLE_COL_PAGE = "page_code";
		const string InterfacePageTableSync::TABLE_COL_CONTENT = "content";
		const string InterfacePageTableSync::TABLE_COL_DIRECT_DISPLAY_ALLOWED("direct_display_allowed");

		InterfacePageTableSync::InterfacePageTableSync()
			: SQLiteDirectTableSyncTemplate<InterfacePageTableSync,InterfacePage> ()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_INTERFACE, "INTEGER", false);
			addTableColumn(TABLE_COL_CLASS, "TEXT", false);
			addTableColumn(TABLE_COL_PAGE, "TEXT", false);
			addTableColumn(TABLE_COL_DIRECT_DISPLAY_ALLOWED, "INTEGER", true);
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
				if (!Factory<InterfacePage>::contains(rows->getText(TABLE_COL_CLASS)))
				{
					Log::GetInstance().warn("Corrupted data on "+ TABLE_NAME +" table : Interface page class not found : " + rows->getText(TABLE_COL_CLASS));
					continue;
				}

			    // Search the specified interface
			    try
			    {
					InterfacePage* page(Factory<InterfacePage>::create(rows->getText(TABLE_COL_CLASS)));

					load(page, rows);
					page->store();
				
					shared_ptr<Interface> interf = Interface::GetUpdateable(
						rows->getLongLong ( TABLE_COL_INTERFACE )
					);
					
					interf->addPage(page);
				}
				catch (Interface::ObjectNotFoundException& e)
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
				    ->removePage(rows->getText(TABLE_COL_CLASS), rows->getText ( TABLE_COL_PAGE) );
				InterfacePage::Remove(rows->getLongLong(TABLE_COL_ID));
			}
		}
	}
}



