
/** CalendarTemplateTableSync class implementation.
	@file CalendarTemplateTableSync.cpp
	@author Hugues Romain
	@date 2008

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

#include "CalendarTemplateTableSync.h"
#include "CalendarTemplateElementTableSync.h"

#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "SQLSingleOperatorExpression.hpp"
#include "CalendarRight.h"
#include "CalendarTemplateElementTableSync.h"
#include "ImportableTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace calendar;
	using namespace security;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,CalendarTemplateTableSync>::FACTORY_KEY("55.10 Calendar templates");
	}

	namespace calendar
	{
		const std::string CalendarTemplateTableSync::COL_TEXT("name");
		const std::string CalendarTemplateTableSync::COL_CATEGORY("category");
		const std::string CalendarTemplateTableSync::COL_DATASOURCE_LINKS("datasource_links");
		const std::string CalendarTemplateTableSync::COL_PARENT_ID("parent_id");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CalendarTemplateTableSync>::TABLE(
			"t054_calendar_templates"
		);


		template<> const Field DBTableSyncTemplate<CalendarTemplateTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(CalendarTemplateTableSync::COL_TEXT, SQL_TEXT),
			Field(CalendarTemplateTableSync::COL_CATEGORY, SQL_INTEGER),
			Field(CalendarTemplateTableSync::COL_DATASOURCE_LINKS, SQL_TEXT),
			Field(CalendarTemplateTableSync::COL_PARENT_ID, SQL_INTEGER),
			Field()
		};


		template<>
		DBTableSync::Indexes DBTableSyncTemplate<CalendarTemplateTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(CalendarTemplateTableSync::COL_PARENT_ID.c_str(), ""));
			return r;
		}


		template<> void DBDirectTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>::Load(
			CalendarTemplate* object
			, const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Columns reading
			RegistryKeyType id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setName(rows->getText(CalendarTemplateTableSync::COL_TEXT));
			object->setCategory(static_cast<CalendarTemplate::Category>(rows->getInt(CalendarTemplateTableSync::COL_CATEGORY)));

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Elements
 				CalendarTemplateElementTableSync::SearchResult elements(
 					CalendarTemplateElementTableSync::Search(
 						env,
 						object->getKey(),
						optional<RegistryKeyType>(),
 						0, optional<size_t>(),
 						UP_LINKS_LOAD_LEVEL
 				)	);
 				BOOST_FOREACH(const shared_ptr<CalendarTemplateElement>& e, elements)
 				{
 					object->addElement(*e);
 				}

				// Data source links
				object->setDataSourceLinksWithoutRegistration(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						rows->getText(CalendarTemplateTableSync::COL_DATASOURCE_LINKS),
						env
				)	);

				// Parent
				try
				{
					RegistryKeyType id(rows->getLongLong(CalendarTemplateTableSync::COL_PARENT_ID));
					if(id > 0)
					{
						object->setParent(
							CalendarTemplateTableSync::GetEditable(rows->getLongLong(CalendarTemplateTableSync::COL_PARENT_ID), env, linkLevel).get()
						);
					}
					else
					{
						object->setParent(NULL);
					}
				}
				catch (ObjectNotFoundException<CalendarTemplate> e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + CalendarTemplateTableSync::COL_PARENT_ID, e);
				}
			}
		}



		template<> void DBDirectTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>::Save(
			CalendarTemplate* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<CalendarTemplateTableSync> query(*object);
			query.addField(object->getName());
			query.addField(static_cast<int>(object->getCategory()));
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks(),
					ParametersMap::FORMAT_INTERNAL // temporary : to avoid double semicolons
			)	);
			query.addField(object->getParent(true) ? object->getParent()->getKey() : 0);
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>::Unlink(
			CalendarTemplate* obj
		){
		}



		template<> bool DBTableSyncTemplate<CalendarTemplateTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			Env env;
			CalendarTemplateElementTableSync::SearchResult result(
				CalendarTemplateElementTableSync::Search(
					env,
					optional<RegistryKeyType>(),
					object_id,
					0,
					1
			)	);
			if(!result.empty())
			{
				return false;
			}

			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CalendarRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<CalendarTemplateTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			CalendarTemplateElementTableSync::Clean(id, transaction);
		}



		template<> void DBTableSyncTemplate<CalendarTemplateTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CalendarTemplateTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log calendar template element removal
		}
	}



	namespace calendar
	{
		CalendarTemplateTableSync::SearchResult CalendarTemplateTableSync::Search(
			Env& env,
			boost::optional<std::string> name,
			boost::optional<util::RegistryKeyType> forbiddenId,
			bool orderByName,
			bool raisingOrder,
			int first /*= 0*/,
			boost::optional<std::size_t> number /*= 0*/,
			LinkLevel linkLevel,
			boost::optional<util::RegistryKeyType> parentId
		){
			SelectQuery<CalendarTemplateTableSync> query;
			if(name)
			{
				query.addWhereField(COL_TEXT, *name);
			}
			if(forbiddenId)
			{
				query.addWhereField(TABLE_COL_ID, *forbiddenId, ComposedExpression::OP_DIFF);
			}
			if(parentId)
			{
				if(*parentId)
				{
					query.addWhereField(COL_PARENT_ID,  *parentId);
				}
				else
				{
					query.addWhere(
						ComposedExpression::Get(
							ComposedExpression::Get(
								FieldExpression::Get(TABLE.NAME, COL_PARENT_ID),
								ComposedExpression::OP_EQ,
								ValueExpression<int>::Get(0)
							),
							ComposedExpression::OP_OR,
							SQLSingleOperatorExpression::Get(
								SQLSingleOperatorExpression::OP_IS_NULL,
								FieldExpression::Get(TABLE.NAME, COL_PARENT_ID)
					)	)	);
				}
			}
			if (orderByName)
			{
				query.addOrderField(COL_TEXT, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}



		CalendarTemplateTableSync::CalendarTemplatesList CalendarTemplateTableSync::GetCalendarTemplatesList(
			CalendarTemplateTableSync::CalendarTemplatesList::value_type::second_type zeroName,
			optional<CalendarTemplateTableSync::CalendarTemplatesList::value_type::first_type> idToAvoid,
			boost::optional<util::RegistryKeyType> parentId
		){
			Env env;
			CalendarTemplateTableSync::SearchResult s(
				Search(
					env,
					optional<string>(),
					idToAvoid ? *idToAvoid : optional<RegistryKeyType>(),
					true,
					true,
					0,
					optional<size_t>(),
					UP_LINKS_LOAD_LEVEL,
					parentId
			)	);
			CalendarTemplatesList r;
			if(!zeroName.empty())
			{
				r.push_back(make_pair(CalendarTemplatesList::value_type::first_type(0), zeroName));
			}
			BOOST_FOREACH(const CalendarTemplateTableSync::SearchResult::value_type& c, s)
			{
				r.push_back(make_pair(c->getKey(), c->getName()));
			}
			return r;
		}
	}
}
