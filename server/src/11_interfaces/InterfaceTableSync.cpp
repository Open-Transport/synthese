
/** InterfaceTableSync class implementation.
	@file InterfaceTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "InterfaceTableSync.h"

#include "Conversion.h"
#include "DBResult.hpp"
#include "ReplaceQuery.h"
#include "InterfacePageTableSync.h"

#include <sstream>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace interfaces;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBTableSync,InterfaceTableSync>::FACTORY_KEY("16.01 Interfaces");
	}

	namespace interfaces
	{
		const std::string InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE = "no_session_default_page";
		const std::string InterfaceTableSync::TABLE_COL_NAME = "name";
		const string InterfaceTableSync::COL_DEFAULT_CLIENT_URL("default_client_url");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<InterfaceTableSync>::TABLE(
			"t024_interfaces"
		);

		template<> const Field DBTableSyncTemplate<InterfaceTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE, SQL_TEXT),
			Field(InterfaceTableSync::TABLE_COL_NAME, SQL_TEXT),
			Field(InterfaceTableSync::COL_DEFAULT_CLIENT_URL, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<InterfaceTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}

		template<>
		void OldLoadSavePolicy<InterfaceTableSync,Interface>::Load(
			Interface* interf,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			interf->setKey(rows->getLongLong ( TABLE_COL_ID));
			interf->setNoSessionDefaultPageCode(rows->getText ( InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE));
			interf->setName(rows->getText ( InterfaceTableSync::TABLE_COL_NAME));
			interf->setDefaultClientURL(rows->getText(InterfaceTableSync::COL_DEFAULT_CLIENT_URL));
		}


		template<>
		void OldLoadSavePolicy<InterfaceTableSync,Interface>::Unlink(
			Interface* interf
		){

		}

		template<>
		void OldLoadSavePolicy<InterfaceTableSync,Interface>::Save(
			Interface* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<InterfaceTableSync> query(*object);
			query.addField(object->getNoSessionDefaultPageCode());
			query.addField(object->getName());
			query.addField(object->getDefaultClientURL());
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<InterfaceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<InterfaceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<InterfaceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<InterfaceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}
	}

	namespace interfaces
	{
		InterfaceTableSync::SearchResult InterfaceTableSync::Search(
			util::Env& env,
			boost::optional<std::string> interfacePageKey,
			bool orderByName,
			bool raisingOrder,
			boost::optional<int> first,
			boost::optional<int> number,
			util::LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1";
			if (interfacePageKey)
				query << " AND EXISTS(SELECT * FROM " << InterfacePageTableSync::TABLE.NAME << " p"
					<< " WHERE p." << InterfacePageTableSync::TABLE_COL_INTERFACE << "=" << TABLE.NAME << "." << TABLE_COL_ID
					<< " AND p." << InterfacePageTableSync::TABLE_COL_CLASS << "=" << Conversion::ToDBString(*interfacePageKey)
					<< ")";
			if(orderByName)
				query << " ORDER BY " << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");

			if (number)
				query << " LIMIT " << (*number + 1);
			if (first)
				query << " OFFSET " << *first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}


		InterfaceTableSync::OrderedInterfaceLabels InterfaceTableSync::_GetInterfaceLabels(
			boost::optional<std::string> textWithNo,
			boost::optional<std::string> textWithUnknown,
			boost::optional<std::string> pageFilter
		){
			OrderedInterfaceLabels m;
			if (textWithUnknown)
			{
				m.push_back(make_pair(optional<RegistryKeyType>(), *textWithUnknown));
			}
			if (textWithNo)
			{
				m.push_back(make_pair(0, *textWithNo));
			}
			Env env;
			SearchResult interfaces(
				Search(env, pageFilter)
			);
			BOOST_FOREACH(const boost::shared_ptr<Interface>& interf, interfaces)
			{
				m.push_back(make_pair(interf->getKey(), interf->getName()));
			}
			return m;
		}

	}
}



