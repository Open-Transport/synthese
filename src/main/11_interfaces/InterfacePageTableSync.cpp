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

#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include "02_db/Constants.h"
#include "02_db/SQLiteResult.h"

#include "11_interfaces/InterfaceModule.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/InterfacePageTableSync.h"

using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;

	namespace interfaces
	{
		const std::string InterfacePageTableSync::TABLE_NAME = "t023_interface_pages";
		const std::string InterfacePageTableSync::TABLE_COL_ID = "id";
		const std::string InterfacePageTableSync::TABLE_COL_INTERFACE = "interface_id";
		const std::string InterfacePageTableSync::TABLE_COL_PAGE = "page_code";
		const std::string InterfacePageTableSync::TABLE_COL_CONTENT = "content";

		InterfacePageTableSync::InterfacePageTableSync()
			: SQLiteTableSync ( TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_INTERFACE, "INTEGER", false);
			addTableColumn(TABLE_COL_PAGE, "TEXT", false);
			addTableColumn(TABLE_COL_CONTENT, "TEXT", true);
		}


		void InterfacePageTableSync::rowsUpdated( SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
			    shared_ptr<InterfacePage> page = InterfaceModule::getInterfacePages().getUpdateable (rows->getLongLong (TABLE_COL_ID));
			    page->parse (rows->getText(TABLE_COL_CONTENT) );
			}
		}


		void InterfacePageTableSync::rowsAdded( SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
			    // Search the specified interface
			    try
			    {
				
				shared_ptr<InterfacePage> page;
				if (Factory<InterfacePage>::contains(rows->getText (TABLE_COL_PAGE)))
				    page = Factory<InterfacePage>::create( rows->getText (TABLE_COL_PAGE) );
				else
				    page.reset(new InterfacePage);
				page->setKey(rows->getLongLong ( TABLE_COL_ID));
				page->setCode(rows->getText (TABLE_COL_PAGE));

				InterfaceModule::getInterfacePages().add(page);
				
				page->parse( rows->getText ( TABLE_COL_CONTENT) );	// Needs the page to be already registered
				
				shared_ptr<Interface> interf = InterfaceModule::getInterfaces().getUpdateable(
				    rows->getLongLong ( TABLE_COL_INTERFACE ));
				
				interf->addPage(rows->getText ( TABLE_COL_PAGE), page );
				}
				catch (Interface::RegistryKeyException& e)
				{
					Log::GetInstance().warn("Corrupted data on "+ TABLE_NAME +" table : Interface not found", e);
				}
			}
		}


		void InterfacePageTableSync::rowsRemoved( SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				InterfaceModule::getInterfaces().getUpdateable (rows->getLongLong ( TABLE_COL_INTERFACE))
				    ->removePage( rows->getText ( TABLE_COL_PAGE) );
			}
		}
	}
}



