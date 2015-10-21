
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
#include "CalendarRight.h"
#include "CalendarTemplateElementTableSync.h"
#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "SQLSingleOperatorExpression.hpp"
#include "User.h"

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
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CalendarTemplateTableSync>::TABLE(
			"t054_calendar_templates"
		);


		template<> const Field DBTableSyncTemplate<CalendarTemplateTableSync>::_FIELDS[]=
		{
			Field()
		};


		template<>
		DBTableSync::Indexes DBTableSyncTemplate<CalendarTemplateTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(ParentCalendarTemplate::FIELD.name.c_str(), ""));
			return r;
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
				query.addWhereField(Name::FIELD.name, *name);
			}
			if(forbiddenId)
			{
				query.addWhereField(Key::FIELD.name, *forbiddenId, ComposedExpression::OP_DIFF);
			}
			if(parentId)
			{
				if(*parentId)
				{
					query.addWhereField(ParentCalendarTemplate::FIELD.name,  *parentId);
				}
				else
				{
					query.addWhere(
						ComposedExpression::Get(
							ComposedExpression::Get(
								FieldExpression::Get(TABLE.NAME, ParentCalendarTemplate::FIELD.name),
								ComposedExpression::OP_EQ,
								ValueExpression<int>::Get(0)
							),
							ComposedExpression::OP_OR,
							SQLSingleOperatorExpression::Get(
								SQLSingleOperatorExpression::OP_IS_NULL,
								FieldExpression::Get(TABLE.NAME, ParentCalendarTemplate::FIELD.name)
					)	)	);
				}
			}
			if (orderByName)
			{
				query.addOrderField(Name::FIELD.name, raisingOrder);
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

		bool CalendarTemplateTableSync::allowList(const server::Session* session) const
		{
			return true;
		}

	}
}
