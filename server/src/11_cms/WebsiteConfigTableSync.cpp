
/** WebPageTableSync class implementation.
	@file WebPageTableSync.cpp

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

#include "WebsiteConfigTableSync.hpp"

#include "DBRecord.hpp"
#include "DBResult.hpp"
#include "SelectQuery.hpp"
#include "SQLSingleOperatorExpression.hpp"
#include "Website.hpp"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace cms;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,WebsiteConfigTableSync>::FACTORY_KEY("36.12 Web site configs");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<WebsiteConfigTableSync>::TABLE(
			"t103_website_configs"
		);

		template<> const Field DBTableSyncTemplate<WebsiteConfigTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<WebsiteConfigTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(SimpleObjectFieldDefinition<Website>::FIELD.name.c_str(), ""));
			return r;
		}



		template<> bool DBTableSyncTemplate<WebsiteConfigTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<WebsiteConfigTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<WebsiteConfigTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<WebsiteConfigTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace cms
	{
		WebsiteConfigTableSync::SearchResult WebsiteConfigTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> siteId /*= boost::optional<util::RegistryKeyType>()*/,
			int first /*= 0 */,
			boost::optional<std::size_t> number /* = boost::optional<std::size_t>()*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			SelectQuery<WebsiteConfigTableSync> query;
			if (siteId)
			{
				query.addWhereField(SimpleObjectFieldDefinition<Website>::FIELD.name, *siteId);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
}	}
