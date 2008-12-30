////////////////////////////////////////////////////////////////////////////////
/// InterfacePageTableSync class implementation.
///	@file InterfacePageTableSync.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "InterfacePageTableSync.h"
#include "InterfaceTableSync.h"
#include "InterfacePage.h"
#include "Interface.h"
#include "Conversion.h"
#include "Log.h"
#include "Factory.h"
#include "02_db/Constants.h"
#include "SQLiteResult.h"
#include "InterfacePageException.h"

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
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<InterfacePageTableSync>::TABLE(
				"t023_interface_pages"
				);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<InterfacePageTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(InterfacePageTableSync::TABLE_COL_INTERFACE, SQL_INTEGER, false),
			SQLiteTableSync::Field(InterfacePageTableSync::TABLE_COL_CLASS, SQL_TEXT, false),
			SQLiteTableSync::Field(InterfacePageTableSync::TABLE_COL_PAGE, SQL_TEXT, false),
			SQLiteTableSync::Field(InterfacePageTableSync::TABLE_COL_DIRECT_DISPLAY_ALLOWED, SQL_INTEGER),
			SQLiteTableSync::Field(InterfacePageTableSync::TABLE_COL_CONTENT, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<InterfacePageTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<InterfacePageTableSync,InterfacePage>::Load(
			InterfacePage* page,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			page->setPageCode(rows->getText(InterfacePageTableSync::TABLE_COL_PAGE));
			page->setDirectDisplayAllowed(rows->getBool(InterfacePageTableSync::TABLE_COL_DIRECT_DISPLAY_ALLOWED));
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				try
				{
					shared_ptr<Interface> interf(InterfaceTableSync::GetEditable(
							rows->getLongLong(InterfacePageTableSync::TABLE_COL_INTERFACE),
							env,
							linkLevel
					)	);
					page->setInterface(interf.get());
					if (linkLevel >= UP_DOWN_LINKS_LOAD_LEVEL)
					{
						interf->addPage(page);
					}
				}
				catch(ObjectNotFoundException<Interface>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + InterfacePageTableSync::TABLE_COL_INTERFACE, e);
				}
			}
			page->parse(rows->getText (InterfacePageTableSync::TABLE_COL_CONTENT));
		}


		template<> void SQLiteDirectTableSyncTemplate<InterfacePageTableSync,InterfacePage>::Save(
			InterfacePage* page
		){
			/// @todo Implementation
		}


		template<> void SQLiteDirectTableSyncTemplate<InterfacePageTableSync, InterfacePage>::Unlink(
			InterfacePage* obj
		){
			if (obj->getInterface() != NULL)
			{
				try
				{
					const_cast<Interface*>(obj->getInterface())->removePage(obj->getFactoryKey(), obj->getPageCode());
				}
				catch(InterfacePageException& e)
				{

				}
				obj->setInterface(NULL);
			}
		}
	}

	namespace interfaces
	{
		InterfacePageTableSync::InterfacePageTableSync()
			: SQLiteRegistryTableSyncTemplate<InterfacePageTableSync,InterfacePage> ()
		{
		}



		boost::shared_ptr<InterfacePage> InterfacePageTableSync::GetNewObject(
			const SQLiteResultSPtr& row
		){
			shared_ptr<InterfacePage> page(Factory<InterfacePage>::create(row->getText(InterfacePageTableSync::TABLE_COL_CLASS)));
			page->setKey(row->getKey());
			return page;
		}
	}
}



