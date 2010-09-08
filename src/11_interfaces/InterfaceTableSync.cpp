
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

#include "Conversion.h"

#include "SQLiteResult.h"
#include "ReplaceQuery.h"
#include "InterfaceTableSync.h"
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
		template<> const std::string FactorableTemplate<SQLiteTableSync,InterfaceTableSync>::FACTORY_KEY("16.01 Interfaces");
	}

	namespace interfaces
	{
		const std::string InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE = "no_session_default_page";
		const std::string InterfaceTableSync::TABLE_COL_NAME = "name";
		const string InterfaceTableSync::COL_DEFAULT_CLIENT_URL("default_client_url");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<InterfaceTableSync>::TABLE(
			"t024_interfaces"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<InterfaceTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE, SQL_TEXT),
			SQLiteTableSync::Field(InterfaceTableSync::TABLE_COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(InterfaceTableSync::COL_DEFAULT_CLIENT_URL, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<InterfaceTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<InterfaceTableSync,Interface>::Load(
			Interface* interf,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			interf->setKey(rows->getLongLong ( TABLE_COL_ID));
			interf->setNoSessionDefaultPageCode(rows->getText ( InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE));
			interf->setName(rows->getText ( InterfaceTableSync::TABLE_COL_NAME));
			interf->setDefaultClientURL(rows->getText(InterfaceTableSync::COL_DEFAULT_CLIENT_URL));
		}


		template<> void SQLiteDirectTableSyncTemplate<InterfaceTableSync,Interface>::Unlink(
			Interface* interf
		){

		}

		template<> void SQLiteDirectTableSyncTemplate<InterfaceTableSync,Interface>::Save(
			Interface* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<InterfaceTableSync> query(*object);
			query.addField(object->getNoSessionDefaultPageCode());
			query.addField(object->getName());
			query.addField(object->getDefaultClientURL());
			query.execute(transaction);
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
					<< " AND p." << InterfacePageTableSync::TABLE_COL_CLASS << "=" << Conversion::ToSQLiteString(*interfacePageKey)
					<< ")";
			if(orderByName)
				query << " ORDER BY " << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");

			if (number)
				query << " LIMIT " << Conversion::ToString(*number + 1);
			if (first)
				query << " OFFSET " << Conversion::ToString(*first);

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
			BOOST_FOREACH(shared_ptr<Interface> interf, interfaces)
			{
				m.push_back(make_pair(interf->getKey(), interf->getName()));
			}
			return m;
		}

	}
}



