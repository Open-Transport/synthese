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

#include "InterfacePage.h"
#include "Interface.h"

#include "Conversion.h"
#include "01_util/Log.h"
#include "Factory.h"

#include "02_db/Constants.h"
#include "SQLiteResult.h"

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

	namespace interfaces
	{
		const string InterfacePageTableSync::TABLE_COL_INTERFACE("interface_id");
		const string InterfacePageTableSync::TABLE_COL_CLASS = "class_code";
		const string InterfacePageTableSync::TABLE_COL_PAGE = "page_code";
		const string InterfacePageTableSync::TABLE_COL_CONTENT = "content";
		const string InterfacePageTableSync::TABLE_COL_DIRECT_DISPLAY_ALLOWED("direct_display_allowed");
	}

	namespace db
	{
		template<> const SQLiteTableFormat SQLiteTableSyncTemplate<InterfacePageTableSync>::TABLE(
			InterfacePageTableSync::CreateFormat(
				"t023_interface_pages",
				SQLiteTableFormat::CreateFields(
					SQLiteTableFormat::Field(InterfacePageTableSync::TABLE_COL_INTERFACE, INTEGER, false),
					SQLiteTableFormat::Field(InterfacePageTableSync::TABLE_COL_CLASS, TEXT, false),
					SQLiteTableFormat::Field(InterfacePageTableSync::TABLE_COL_PAGE, TEXT, false),
					SQLiteTableFormat::Field(InterfacePageTableSync::TABLE_COL_DIRECT_DISPLAY_ALLOWED, INTEGER),
					SQLiteTableFormat::Field(InterfacePageTableSync::TABLE_COL_CONTENT, TEXT),
					SQLiteTableFormat::Field()
				), SQLiteTableFormat::Indexes()
		)	);



		template<> void SQLiteDirectTableSyncTemplate<InterfacePageTableSync,InterfacePage>::Load(
			InterfacePage* page,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			page->setPageCode(rows->getText(InterfacePageTableSync::TABLE_COL_PAGE));
			page->setDirectDisplayAllowed(rows->getBool(InterfacePageTableSync::TABLE_COL_DIRECT_DISPLAY_ALLOWED));
			page->parse(rows->getText (InterfacePageTableSync::TABLE_COL_CONTENT));
		}
	}

	namespace interfaces
	{
		InterfacePageTableSync::InterfacePageTableSync()
			: SQLiteDirectTableSyncTemplate<InterfacePageTableSync,InterfacePage> ()
		{
		}


		void InterfacePageTableSync::rowsUpdated( SQLite* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows )
		{
			Env& env(Env::GetOfficialEnv());
			Registry<InterfacePage>& registry(env.getEditableRegistry<InterfacePage>());
			while (rows->next ())
			{
			    shared_ptr<InterfacePage> page(registry.getEditable(rows->getKey()));
				Load(page.get(), rows, env, ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}


		void InterfacePageTableSync::rowsAdded( SQLite* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows, bool isFirstSync)
		{
			Env& env(Env::GetOfficialEnv());
			Registry<InterfacePage>& registry(env.getEditableRegistry<InterfacePage>());
			Registry<Interface>& interfRegistry(env.getEditableRegistry<Interface>());
			while (rows->next ())
			{
				if (!Factory<InterfacePage>::contains(rows->getText(TABLE_COL_CLASS)))
				{
					Log::GetInstance().warn("Corrupted data on "+ TABLE.NAME +" table : Interface page class not found : " + rows->getText(TABLE_COL_CLASS));
					continue;
				}

			    // Search the specified interface
			    try
			    {
					shared_ptr<InterfacePage> page(Factory<InterfacePage>::create(rows->getText(TABLE_COL_CLASS)));
					page->setKey(rows->getKey());

					Load(page.get(), rows, env, ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
					registry.add(page);
				
					shared_ptr<Interface> interf(interfRegistry.getEditable(
						rows->getLongLong ( TABLE_COL_INTERFACE )
					));
					
					interf->addPage(page.get());
				}
				catch (ObjectNotFoundException<Interface>& e)
				{
					Log::GetInstance().warn("Corrupted data on "+ TABLE.NAME +" table : Interface not found", e);
				}
			}
		}


		void InterfacePageTableSync::rowsRemoved( SQLite* sqlite,  SQLiteSync* sync, const SQLiteResultSPtr& rows )
		{
			Env& env(Env::GetOfficialEnv());
			Registry<InterfacePage>& registry(env.getEditableRegistry<InterfacePage>());
			Registry<Interface>& interfRegistry(env.getEditableRegistry<Interface>());
			while (rows->next ())
			{
				/// @todo to be reimplemented
				interfRegistry.getEditable(rows->getLongLong ( TABLE_COL_INTERFACE))
				    ->removePage(rows->getText(TABLE_COL_CLASS), rows->getText ( TABLE_COL_PAGE) );
				registry.remove(rows->getKey());
			}
		}
	}
}



