
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

#include "WebPageTableSync.h"

#include "DBRecord.hpp"
#include "DBResult.hpp"
#include "RankUpdateQuery.hpp"
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
		template<> const string FactorableTemplate<DBTableSync,WebPageTableSync>::FACTORY_KEY("36.10 Web pages");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<WebPageTableSync>::TABLE(
			"t063_web_pages"
		);

		template<> const Field DBTableSyncTemplate<WebPageTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<WebPageTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[0].name.c_str(), ""));
			r.push_back(
				DBTableSync::Index(
					ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[1].name.c_str(),
					ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[2].name.c_str(),
			"")	);
			return r;
		}



		template<> bool DBTableSyncTemplate<WebPageTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<WebPageTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			SelectQuery<WebPageTableSync> query;
			Env env;
			query.addWhereField(
				SimpleObjectFieldDefinition<WebpageLinks>::FIELD.name,
				"%"+ lexical_cast<string>(id) +"%",
				ComposedExpression::OP_LIKE
			);
			WebPageTableSync::SearchResult pages(WebPageTableSync::LoadFromQuery(query, env, UP_LINKS_LOAD_LEVEL));
			BOOST_FOREACH(const boost::shared_ptr<Webpage>& page, pages)
			{
				WebpageLinks::Type newLinks;
				BOOST_FOREACH(Webpage* linkedPage, page->get<WebpageLinks>())
				{
					if(linkedPage->getKey() != id)
					{
						newLinks.push_back(linkedPage);
					}
				}
				page->set<WebpageLinks>(newLinks);
				WebPageTableSync::Save(page.get(), transaction);
			}
		}



		template<> void DBTableSyncTemplate<WebPageTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			boost::shared_ptr<const Webpage> page(WebPageTableSync::Get(id, env));
			WebPageTableSync::ShiftRank(
				page->getRoot()->getKey(),
				page->getParent() ? page->getParent()->getKey() : RegistryKeyType(0),
				page->getRank(),
				false,
				transaction
			);
		}



		template<> void DBTableSyncTemplate<WebPageTableSync>::LogRemoval(
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
				query.addWhereField(ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[0].name, *siteId);
			}
			if(parentId)
			{
				if(*parentId == 0)
				{
					query.addWhere(
						ComposedExpression::Get(
							ComposedExpression::Get(
								FieldExpression::Get(TABLE.NAME, ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[1].name),
								ComposedExpression::OP_EQ,
								ValueExpression<RegistryKeyType>::Get(0)
							),
							ComposedExpression::OP_OR,
							SQLSingleOperatorExpression::Get(
								SQLSingleOperatorExpression::OP_IS_NULL,
								FieldExpression::Get(TABLE.NAME, ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[1].name)
							)
					)	);
				}
				else
				{
					query.addWhereField(ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[1].name, *parentId);
				}
			}
			if(rank)
			{
				query.addWhereField(ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[2].name, *rank);
			}
			if(orderByRank)
			{
				query.addOrderField(ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[2].name, raisingOrder);
			}
			else if (orderByTitle)
			{
				query.addOrderField(SimpleObjectFieldDefinition<Title>::FIELD.name, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}



		void WebPageTableSync::ShiftRank(
			RegistryKeyType siteId,
			util::RegistryKeyType parentId,
			std::size_t rank,
			bool add,
			db::DBTransaction& transaction
		){
			RankUpdateQuery<WebPageTableSync> query(ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[2].name, add ? 1 : -1, rank);
			query.addWhereField(ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[0].name, siteId);
			query.addWhereField(ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[1].name, parentId);
			query.execute(transaction);
		}

		db::RowsList WebPageTableSync::SearchForAutoComplete(
				const boost::optional<std::string> prefix,
				const boost::optional<std::size_t> limit,
				const boost::optional<std::string> optionalParameter
		) const {
			RowsList result;

			SelectQuery<WebPageTableSync> query;
			Env env;
			if(prefix)
			{
				query.addWhereField(SimpleObjectFieldDefinition<Title>::FIELD.name, "%"+ *prefix +"%", ComposedExpression::OP_LIKE);
			}
			if(limit)
			{
				query.setNumber(*limit);
			}
			query.addOrderField(ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[2].name,true);
			WebPageTableSync::SearchResult pages(WebPageTableSync::LoadFromQuery(query, env, UP_LINKS_LOAD_LEVEL));
			BOOST_FOREACH(const boost::shared_ptr<Webpage>& page, pages)
			{
				result.push_back(std::make_pair(page->getKey(), page->getName()));
			}
			return result;
		} ;
}	}
