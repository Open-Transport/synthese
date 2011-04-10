
/** CalendarTemplateTableSync class implementation.
	@file CalendarTemplateTableSync.cpp
	@author Hugues Romain
	@date 2008

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

#include "CalendarTemplateTableSync.h"
#include "CalendarTemplateElementTableSync.h"

#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "CalendarRight.h"
#include "CalendarTemplateElementTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace calendar;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,CalendarTemplateTableSync>::FACTORY_KEY("55.10 Calendar templates");
	}
	
	namespace calendar
	{
		const std::string CalendarTemplateTableSync::COL_TEXT("name");
		const std::string CalendarTemplateTableSync::COL_CATEGORY("category");
	}
	
	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CalendarTemplateTableSync>::TABLE(
			"t054_calendar_templates"
		);
		

		template<> const DBTableSync::Field DBTableSyncTemplate<CalendarTemplateTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(CalendarTemplateTableSync::COL_TEXT, SQL_TEXT),
			DBTableSync::Field(CalendarTemplateTableSync::COL_CATEGORY, SQL_INTEGER),
			DBTableSync::Field()
		};


		template<> const DBTableSync::Index DBTableSyncTemplate<CalendarTemplateTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};


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
			object->setText(rows->getText(CalendarTemplateTableSync::COL_TEXT));
			object->setCategory(static_cast<CalendarTemplate::Category>(rows->getInt(CalendarTemplateTableSync::COL_CATEGORY)));
			
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
 				CalendarTemplateElementTableSync::SearchResult elements(
 					CalendarTemplateElementTableSync::Search(
 						env,
 						object->getKey(),
						optional<RegistryKeyType>(),
 						0, optional<size_t>(),
 						DOWN_LINKS_LOAD_LEVEL
 				)	);
 				BOOST_FOREACH(shared_ptr<CalendarTemplateElement> e, elements)
 				{
 					object->addElement(*e);
 				}
			}
		}



		template<> void DBDirectTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>::Save(
			CalendarTemplate* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<CalendarTemplateTableSync> query(*object);
			query.addField(object->getText());
			query.addField(static_cast<int>(object->getCategory()));
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



		void DBTableSyncTemplate<CalendarTemplateTableSync>::LogRemoval(
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
			LinkLevel linkLevel
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
			optional<CalendarTemplateTableSync::CalendarTemplatesList::value_type::first_type> idToAvoid
		){
			Env env;
			CalendarTemplateTableSync::SearchResult s(Search(env));
			CalendarTemplatesList r;
			if(!zeroName.empty())
			{
				r.push_back(make_pair(CalendarTemplatesList::value_type::first_type(0), zeroName));
			}
			BOOST_FOREACH(const CalendarTemplateTableSync::SearchResult::value_type& c, s)
			{
				if(idToAvoid && c->getKey() == *idToAvoid) continue;
				r.push_back(make_pair(c->getKey(), c->getText()));
			}
			return r;
		}
	}
}
