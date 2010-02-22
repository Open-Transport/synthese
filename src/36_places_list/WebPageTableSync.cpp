
/** WebPageTableSync class implementation.
	@file SiteTableSync.cpp

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

#include "WebPageTableSync.h"
#include "ReplaceQuery.h"
#include "SQLiteResult.h"
#include "SiteTableSync.h"


using namespace boost;
using namespace std;

namespace synthese
{
	using namespace env;
	using namespace util;
	using namespace db;
	using namespace transportwebsite;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,WebPageTableSync>::FACTORY_KEY("36.10 Web pages");
	}

	namespace transportwebsite
	{
		const string WebPageTableSync::COL_TITLE = "title";
		const string WebPageTableSync::COL_SITE_ID = "site_id";
		const string WebPageTableSync::COL_CONTENT1 = "content1";
		const string WebPageTableSync::COL_INCLUDE1 = "include1";
		const string WebPageTableSync::COL_CONTENT2 = "content2";
		const string WebPageTableSync::COL_INCLUDE2 = "include2";
		const string WebPageTableSync::COL_CONTENT3 = "content3";
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<WebPageTableSync>::TABLE(
				"t063_web_pages"
		);
		
		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<WebPageTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(WebPageTableSync::COL_SITE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(WebPageTableSync::COL_TITLE, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_CONTENT1, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_INCLUDE1, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_CONTENT2, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_INCLUDE2, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_CONTENT3, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<WebPageTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index(WebPageTableSync::COL_SITE_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<WebPageTableSync,WebPage>::Load(
			WebPage* webpage,
			const SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			webpage->setTitle(rows->getText(WebPageTableSync::COL_TITLE));
			webpage->setContent1(rows->getText(WebPageTableSync::COL_CONTENT1));
			webpage->setInclude1(rows->getText(WebPageTableSync::COL_INCLUDE1));
			webpage->setContent2(rows->getText(WebPageTableSync::COL_CONTENT2));
			webpage->setInclude2(rows->getText(WebPageTableSync::COL_INCLUDE2));
			webpage->setContent3(rows->getText(WebPageTableSync::COL_CONTENT3));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				uid id(rows->getLongLong(WebPageTableSync::COL_SITE_ID));
				if (id > 0)
				{
					try
					{
						webpage->setSite(SiteTableSync::GetEditable(id, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<Site>& e)
					{
						Log::GetInstance().warn(
							"Data corrupted in "+ TABLE.NAME + " on web page " + lexical_cast<string>(webpage->getKey()) +" : site " +
							lexical_cast<string>(id) + " not found"
						);
					}
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<WebPageTableSync,WebPage>::Unlink(
			WebPage* obj
		){
			obj->setSite(NULL);
		}


		template<> void SQLiteDirectTableSyncTemplate<WebPageTableSync,WebPage>::Save(
			WebPage* webPage,
			optional<SQLiteTransaction&> transaction
		){
			// Query
			ReplaceQuery<WebPageTableSync> query(*webPage);
			query.addField(webPage->getSite() ? webPage->getSite()->getKey() : RegistryKeyType(0));
			query.addField(webPage->getTitle());
			query.addField(webPage->getContent1());
			query.addField(webPage->getInclude1());
			query.addField(webPage->getContent2());
			query.addField(webPage->getInclude2());
			query.addField(webPage->getContent3());
			query.execute(transaction);
		}
	}

	namespace transportwebsite
	{
		WebPageTableSync::SearchResult WebPageTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> siteId /*= boost::optional<util::RegistryKeyType>()*/, 
			int first /*= 0 */,
			boost::optional<std::size_t> number /* = boost::optional<std::size_t>()*/,
			bool orderByTitle /*= true*/,
			bool raisingOrder /*= true*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (siteId)
				query << " AND " << COL_SITE_ID << "=" << *siteId;
			;
			if (orderByTitle)
				query << " ORDER BY " << COL_TITLE << (raisingOrder ? " ASC" : " DESC");
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
