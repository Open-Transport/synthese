////////////////////////////////////////////////////////////////////////////////
/// InterfacePageTableSync class implementation.
///	@file InterfacePageTableSync.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
#include "Log.h"
#include "Factory.h"
#include "DBConstants.h"
#include "DBResult.hpp"
#include "InterfacePageException.h"
#include "ReplaceQuery.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,InterfacePageTableSync>::FACTORY_KEY("16.02 Interface Pages");
	}

	namespace interfaces
	{
		const string InterfacePageTableSync::TABLE_COL_INTERFACE("interface_id");
		const string InterfacePageTableSync::TABLE_COL_CLASS = "class_code";
		const string InterfacePageTableSync::TABLE_COL_PAGE = "page_code";
		const string InterfacePageTableSync::COL_MIME_TYPE = "mime_type";
		const string InterfacePageTableSync::TABLE_COL_CONTENT = "content";
		const string InterfacePageTableSync::TABLE_COL_DIRECT_DISPLAY_ALLOWED("direct_display_allowed");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<InterfacePageTableSync>::TABLE(
			"t023_interface_pages"
		);

		template<> const Field DBTableSyncTemplate<InterfacePageTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(InterfacePageTableSync::TABLE_COL_INTERFACE, SQL_INTEGER),
			Field(InterfacePageTableSync::TABLE_COL_CLASS, SQL_TEXT),
			Field(InterfacePageTableSync::TABLE_COL_PAGE, SQL_TEXT),
			Field(InterfacePageTableSync::TABLE_COL_DIRECT_DISPLAY_ALLOWED, SQL_INTEGER),
			Field(InterfacePageTableSync::COL_MIME_TYPE, SQL_TEXT),
			Field(InterfacePageTableSync::TABLE_COL_CONTENT, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<InterfacePageTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		};



		template<>
		void OldLoadSavePolicy<InterfacePageTableSync,InterfacePage>::Load(
			InterfacePage* page,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			page->setPageCode(rows->getText(InterfacePageTableSync::TABLE_COL_PAGE));
			page->setDirectDisplayAllowed(rows->getBool(InterfacePageTableSync::TABLE_COL_DIRECT_DISPLAY_ALLOWED));
			page->setMimeType(rows->getText(InterfacePageTableSync::COL_MIME_TYPE));
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				try
				{
					boost::shared_ptr<Interface> interf(InterfaceTableSync::GetEditable(
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
					Log::GetInstance().warn("Data corrupted in " + InterfacePageTableSync::TABLE.NAME + "/" + InterfacePageTableSync::TABLE_COL_INTERFACE, e);
				}
			}
			page->setSource(rows->getText (InterfacePageTableSync::TABLE_COL_CONTENT));
			page->parse();
		}


		template<>
		void OldLoadSavePolicy<InterfacePageTableSync,InterfacePage>::Save(
			InterfacePage* page,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<InterfacePageTableSync> query(*page);
			query.addField(page->getInterface() ? page->getInterface()->getKey() : RegistryKeyType(0));
			query.addField(page->getFactoryKey());
			query.addField(page->getPageCode());
			query.addField(page->getDirectDisplayAllowed());
			query.addField(page->getMimeType());
			query.addField(page->getSource());
			query.execute(transaction);
		}


		template<>
		void OldLoadSavePolicy<InterfacePageTableSync, InterfacePage>::Unlink(
			InterfacePage* obj
		){
			if (obj->getInterface() != NULL)
			{
				try
				{
					const_cast<Interface*>(obj->getInterface())->removePage(obj->getFactoryKey(), obj->getPageCode());
				}
				catch(InterfacePageException&)
				{

				}
				obj->setInterface(NULL);
			}
		}



		template<> bool DBTableSyncTemplate<InterfacePageTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<InterfacePageTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<InterfacePageTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<InterfacePageTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}
	}

	namespace interfaces
	{
		boost::shared_ptr<InterfacePage> InterfacePageTableSync::GetNewObject(
			const DBResultSPtr& row
		){
			boost::shared_ptr<InterfacePage> page(Factory<InterfacePage>::create(row->getText(InterfacePageTableSync::TABLE_COL_CLASS)));
			page->setKey(row->getKey());
			return page;
		}

		boost::shared_ptr<InterfacePage> InterfacePageTableSync::GetNewObject(
		){
			return boost::shared_ptr<InterfacePage>();
		}


		InterfacePageTableSync::SearchResult InterfacePageTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> interfaceId /*= boost::optional<util::RegistryKeyType>()*/,
			boost::optional<int> first /*= 0*/,
			boost::optional<int> number /*= boost::optional<int>()*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1";
			if (interfaceId)
				query << " AND " << TABLE_COL_INTERFACE << "=" << *interfaceId;

			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first)
					query << " OFFSET " << *first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}



