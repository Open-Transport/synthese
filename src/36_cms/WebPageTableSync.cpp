
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
#include "RankUpdateQuery.hpp"
#include "ReplaceQuery.h"
#include "DBResult.hpp"
#include "Fetcher.h"
#include "Website.hpp"
#include "SelectQuery.hpp"
#include "Conversion.h"

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
		template<> const string FactorableTemplate<DBTableSync,WebPageTableSync>::FACTORY_KEY("36.10 Web pages");
	}

	namespace cms
	{
		const string WebPageTableSync::COL_TITLE = "title";
		const string WebPageTableSync::COL_SITE_ID = "site_id";
		const string WebPageTableSync::COL_UP_ID = "up_id";
		const string WebPageTableSync::COL_RANK = "rank";
		const string WebPageTableSync::COL_CONTENT1 = "content1";
		const string WebPageTableSync::COL_START_TIME = "start_time";
		const string WebPageTableSync::COL_END_TIME = "end_time";
		const string WebPageTableSync::COL_MIME_TYPE = "mime_type";
		const string WebPageTableSync::COL_ABSTRACT = "abstract";
		const string WebPageTableSync::COL_IMAGE = "image";
		const string WebPageTableSync::COL_LINKS = "links";
		const string WebPageTableSync::COL_DO_NOT_USE_TEMPLATE = "do_not_use_template";
		const string WebPageTableSync::COL_HAS_FORUM = "has_forum";
		const string WebPageTableSync::COL_SMART_URL_PATH("smart_url_path");
		const string WebPageTableSync::COL_SMART_URL_DEFAULT_PARAMETER_NAME("smart_url_default_parameter_name");
		const string WebPageTableSync::COL_IGNORE_WHITE_CHARS("ignore_white_chars");
		const string WebPageTableSync::COL_RAW_EDITOR("raw_editor");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<WebPageTableSync>::TABLE(
			"t063_web_pages"
		);
		
		template<> const DBTableSync::Field DBTableSyncTemplate<WebPageTableSync>::_FIELDS[] =
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(WebPageTableSync::COL_SITE_ID, SQL_INTEGER),
			DBTableSync::Field(WebPageTableSync::COL_UP_ID, SQL_INTEGER),
			DBTableSync::Field(WebPageTableSync::COL_RANK, SQL_INTEGER),
			DBTableSync::Field(WebPageTableSync::COL_TITLE, SQL_TEXT),
			DBTableSync::Field(WebPageTableSync::COL_CONTENT1, SQL_TEXT),
			DBTableSync::Field(WebPageTableSync::COL_START_TIME, SQL_TEXT),
			DBTableSync::Field(WebPageTableSync::COL_END_TIME, SQL_TEXT),
			DBTableSync::Field(WebPageTableSync::COL_MIME_TYPE, SQL_TEXT),
			DBTableSync::Field(WebPageTableSync::COL_ABSTRACT, SQL_TEXT),
			DBTableSync::Field(WebPageTableSync::COL_IMAGE, SQL_TEXT),
			DBTableSync::Field(WebPageTableSync::COL_LINKS, SQL_TEXT),
			DBTableSync::Field(WebPageTableSync::COL_DO_NOT_USE_TEMPLATE, SQL_BOOLEAN),
			DBTableSync::Field(WebPageTableSync::COL_HAS_FORUM, SQL_BOOLEAN),
			DBTableSync::Field(WebPageTableSync::COL_SMART_URL_PATH, SQL_TEXT),
			DBTableSync::Field(WebPageTableSync::COL_SMART_URL_DEFAULT_PARAMETER_NAME, SQL_TEXT),
			DBTableSync::Field(WebPageTableSync::COL_IGNORE_WHITE_CHARS, SQL_BOOLEAN),
			DBTableSync::Field(WebPageTableSync::COL_RAW_EDITOR, SQL_BOOLEAN),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<WebPageTableSync>::_INDEXES[] =
		{
			DBTableSync::Index(WebPageTableSync::COL_SITE_ID.c_str(), ""),
			DBTableSync::Index(WebPageTableSync::COL_UP_ID.c_str(), WebPageTableSync::COL_RANK.c_str(), ""),
			DBTableSync::Index()
		};


		template<> void DBDirectTableSyncTemplate<WebPageTableSync,Webpage>::Load(
			Webpage* webpage,
			const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			webpage->setParent(NULL);
			webpage->setNullRoot();
			webpage->setName(rows->getText(WebPageTableSync::COL_TITLE));
			webpage->setContent(rows->getText(WebPageTableSync::COL_CONTENT1));
			webpage->setRank(rows->getInt(WebPageTableSync::COL_RANK));
			webpage->setMimeType(rows->getText(WebPageTableSync::COL_MIME_TYPE));
			webpage->setAbstract(rows->getText(WebPageTableSync::COL_ABSTRACT));
			webpage->setImage(rows->getText(WebPageTableSync::COL_IMAGE));
			webpage->setDoNotUseTemplate(rows->getBool(WebPageTableSync::COL_DO_NOT_USE_TEMPLATE));
			webpage->setHasForum(rows->getBool(WebPageTableSync::COL_HAS_FORUM));
			webpage->setSmartURLPath(rows->getText(WebPageTableSync::COL_SMART_URL_PATH));
			webpage->setSmartURLDefaultParameterName(rows->getText(WebPageTableSync::COL_SMART_URL_DEFAULT_PARAMETER_NAME));
			webpage->setIgnoreWhiteChars(rows->getBool(WebPageTableSync::COL_IGNORE_WHITE_CHARS));
			webpage->setRawEditor(rows->getBool(WebPageTableSync::COL_RAW_EDITOR));

			if(!rows->getText(WebPageTableSync::COL_START_TIME).empty())
			{
				webpage->setStartDate(rows->getDateTime(WebPageTableSync::COL_START_TIME));
			}
			if(!rows->getText(WebPageTableSync::COL_END_TIME).empty())
			{
				webpage->setEndDate(rows->getDateTime(WebPageTableSync::COL_END_TIME));
			}

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType id(rows->getLongLong(WebPageTableSync::COL_SITE_ID));
				if (id > 0)
				{
					try
					{
						webpage->setRoot(Fetcher<Website>::FetchEditable(id, env, linkLevel).get());
						webpage->getRoot()->addPage(*webpage);
					}
					catch(ObjectNotFoundException<Website>&)
					{
						Log::GetInstance().warn(
							"Data corrupted in "+ TABLE.NAME + " on web page " + lexical_cast<string>(webpage->getKey()) +" : website " +
							lexical_cast<string>(id) + " not found"
						);
					}
				}

				RegistryKeyType up_id(rows->getLongLong(WebPageTableSync::COL_UP_ID));
				if (up_id > 0)
				{
					try
					{
						Webpage::SetParent(*webpage, WebPageTableSync::GetEditable(up_id, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<Webpage>&)
					{
						Log::GetInstance().warn(
							"Data corrupted in "+ TABLE.NAME + " on web page " + lexical_cast<string>(webpage->getKey()) +" : up web page " +
							lexical_cast<string>(up_id) + " not found"
						);
					}
				}

				vector<string> links(Conversion::ToStringVector(rows->getText(WebPageTableSync::COL_LINKS)));
				Webpage::Links pageLinks;
				BOOST_FOREACH(const string& link, links)
				{
					try
					{
						pageLinks.push_back(
							WebPageTableSync::GetEditable(lexical_cast<RegistryKeyType>(link), env, linkLevel).get()
						);
					}
					catch(bad_lexical_cast&)
					{
						Log::GetInstance().warn(
							"Data corrupted in "+ TABLE.NAME + " on web page " + lexical_cast<string>(webpage->getKey()) +" : link " +
							link + " is not a page id"
						);
					}
					catch(ObjectNotFoundException<Webpage>&)
					{
						Log::GetInstance().warn(
							"Data corrupted in "+ TABLE.NAME + " on web page " + lexical_cast<string>(webpage->getKey()) +" : link " +
							link + " not found"
						);
					}
				}
				webpage->setLinks(pageLinks);
			}
		}



		template<> void DBDirectTableSyncTemplate<WebPageTableSync,Webpage>::Unlink(
			Webpage* obj
		){
			if(obj->getRoot())
			{
				obj->getRoot()->removePage(obj->getSmartURLPath());
			}
			Webpage::SetParent(*obj, NULL);
		}


		template<> void DBDirectTableSyncTemplate<WebPageTableSync,Webpage>::Save(
			Webpage* webPage,
			optional<DBTransaction&> transaction
		){
			// Links preparation
			stringstream linksStream;
			bool first(true);
			BOOST_FOREACH(const Webpage::Links::value_type& link, webPage->getLinks())
			{
				if(first)
				{
					first = false;
				}
				else
				{
					linksStream << ",";
				}
				linksStream << link->getKey();
			}

			// Query
			ReplaceQuery<WebPageTableSync> query(*webPage);
			query.addField(webPage->getRoot() ? webPage->getRoot()->getKey() : RegistryKeyType(0));
			query.addField(webPage->getParent() ? webPage->getParent()->getKey() : RegistryKeyType(0));
			query.addField(static_cast<int>(webPage->getRank()));
			query.addField(webPage->getName());
			query.addField(webPage->getContent());
			query.addField(webPage->getStartDate());
			query.addField(webPage->getEndDate());
			query.addField(webPage->_getMimeType());
			query.addField(webPage->getAbstract());
			query.addField(webPage->getImage());
			query.addField(linksStream.str());
			query.addField(webPage->getDoNotUseTemplate());
			query.addField(webPage->getHasForum());
			query.addField(webPage->getSmartURLPath());
			query.addField(webPage->getSmartURLDefaultParameterName());
			query.addField(webPage->getIgnoreWhiteChars());
			query.addField(webPage->getRawEditor());
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<WebPageTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Control user rights
			return true;
		}



		template<> void DBTableSyncTemplate<WebPageTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			SelectQuery<WebPageTableSync> query;
			Env env;
			query.addWhereField(WebPageTableSync::COL_LINKS, "%"+ lexical_cast<string>(id) +"%", ComposedExpression::OP_LIKE);
			WebPageTableSync::SearchResult pages(WebPageTableSync::LoadFromQuery(query, env, UP_LINKS_LOAD_LEVEL));
			BOOST_FOREACH(shared_ptr<Webpage> page, pages)
			{
				Webpage::Links newLinks;
				BOOST_FOREACH(Webpage* linkedPage, page->getLinks())
				{
					if(linkedPage->getKey() != id)
					{
						newLinks.push_back(linkedPage);
					}
				}
				page->setLinks(newLinks);
				WebPageTableSync::Save(page.get(), transaction);
			}
		}



		template<> void DBTableSyncTemplate<WebPageTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			shared_ptr<const Webpage> page(WebPageTableSync::Get(id, env));
			WebPageTableSync::ShiftRank(
				page->getRoot()->getKey(),
				page->getParent() ? page->getParent()->getKey() : RegistryKeyType(0),
				page->getRank(),
				false,
				transaction
			);
		}



		void DBTableSyncTemplate<WebPageTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace cms
	{
		WebPageTableSync::SearchResult WebPageTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> siteId /*= boost::optional<util::RegistryKeyType>()*/, 
			optional<RegistryKeyType> parentId,
			boost::optional<std::size_t> rank,
			int first /*= 0 */,
			boost::optional<std::size_t> number /* = boost::optional<std::size_t>()*/,
			bool orderByRank,
			bool orderByTitle /*= true*/,
			bool raisingOrder /*= true*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			SelectQuery<WebPageTableSync> query;
			if (siteId)
			{
				query.addWhereField(COL_SITE_ID, *siteId);
			}
			if(parentId)
			{
				if(*parentId == 0)
				{
					query.addWhere(
						ComposedExpression::Get(
							ComposedExpression::Get(FieldExpression::Get(TABLE.NAME, COL_UP_ID), ComposedExpression::OP_EQ, ValueExpression<RegistryKeyType>::Get(0)),
							ComposedExpression::OP_OR,
							IsNullExpression::Get(FieldExpression::Get(TABLE.NAME, COL_UP_ID))
					)	);
				}
				else
				{
					query.addWhereField(COL_UP_ID, *parentId);
				}
			}
			if(rank)
			{
				query.addWhereField(COL_RANK, *rank);
			}
			if(orderByRank)
			{
				query.addOrderField(COL_RANK, raisingOrder);
			}
			else if (orderByTitle)
			{
				query.addOrderField(COL_TITLE, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}



		WebPageTableSync::SiteWebPagesList WebPageTableSync::GetPagesList(
			util::RegistryKeyType siteId,
			const std::string& rootLabel,
			std::string prefix,
			RegistryKeyType upId
		){
			SiteWebPagesList result;
			if(!rootLabel.empty())
			{
				result.push_back(make_pair(0, rootLabel));
			}
			Env env;
			SearchResult pages(Search(env, siteId, upId, optional<size_t>(), 0, optional<size_t>(), true));
			BOOST_FOREACH(const SearchResult::value_type& page, pages)
			{
				result.push_back(make_pair(page->getKey(), prefix + page->getName()));
				SiteWebPagesList subResult(GetPagesList(siteId, string(), prefix + "   ", page->getKey()));
				result.insert(result.end(), subResult.begin(), subResult.end());
			}
			return result;
		}



		void WebPageTableSync::ShiftRank(
			RegistryKeyType siteId,
			util::RegistryKeyType parentId,
			std::size_t rank,
			bool add,
			db::DBTransaction& transaction
		){
			RankUpdateQuery<WebPageTableSync> query(COL_RANK, add ? 1 : -1, rank);
			query.addWhereField(COL_SITE_ID, siteId);
			query.addWhereField(COL_UP_ID, parentId);
			query.execute(transaction);
		}
}	}
