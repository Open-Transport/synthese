
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
		template<> const string FactorableTemplate<SQLiteTableSync,WebPageTableSync>::FACTORY_KEY("36.10 Web pages");
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
			SQLiteTableSync::Field(WebPageTableSync::COL_UP_ID, SQL_INTEGER),
			SQLiteTableSync::Field(WebPageTableSync::COL_RANK, SQL_INTEGER),
			SQLiteTableSync::Field(WebPageTableSync::COL_TITLE, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_CONTENT1, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_START_TIME, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_END_TIME, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_MIME_TYPE, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_ABSTRACT, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_IMAGE, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_LINKS, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_DO_NOT_USE_TEMPLATE, SQL_BOOLEAN),
			SQLiteTableSync::Field(WebPageTableSync::COL_HAS_FORUM, SQL_BOOLEAN),
			SQLiteTableSync::Field(WebPageTableSync::COL_SMART_URL_PATH, SQL_TEXT),
			SQLiteTableSync::Field(WebPageTableSync::COL_SMART_URL_DEFAULT_PARAMETER_NAME, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<WebPageTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index(WebPageTableSync::COL_SITE_ID.c_str(), ""),
			SQLiteTableSync::Index(WebPageTableSync::COL_UP_ID.c_str(), WebPageTableSync::COL_RANK.c_str(), ""),
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<WebPageTableSync,Webpage>::Load(
			Webpage* webpage,
			const SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			string smartURLPath(webpage->getSmartURLPath());
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
						webpage->getRoot()->removePage(smartURLPath);
						webpage->getRoot()->addPage(*webpage);
					}
					catch(ObjectNotFoundException<Website>& e)
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
						webpage->setParent(WebPageTableSync::GetEditable(up_id, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<Webpage>& e)
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



		template<> void SQLiteDirectTableSyncTemplate<WebPageTableSync,Webpage>::Unlink(
			Webpage* obj
		){
		}


		template<> void SQLiteDirectTableSyncTemplate<WebPageTableSync,Webpage>::Save(
			Webpage* webPage,
			optional<SQLiteTransaction&> transaction
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
			query.execute(transaction);
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
			bool add
		){
			stringstream query;
			query <<
				"UPDATE " << TABLE.NAME <<
				" SET " << COL_RANK << "=" << COL_RANK << (add ? "+" : "-") << "1" <<
				" WHERE " <<
					COL_SITE_ID << "=" << siteId << " AND " <<
					COL_UP_ID << "=" << parentId << " AND " <<
					COL_RANK << ">=" << rank
			;
			DBModule::GetSQLite()->execUpdate(query.str());
		}
	}
}
