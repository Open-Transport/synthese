
/** TransportNetworkTableSync class implementation.
	@file TransportNetworkTableSync.cpp

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

#include "TransportNetworkTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "ImportableTableSync.hpp"
#include "TransportNetworkRight.h"
#include "CommercialLineTableSync.h"
#include "CalendarTemplateTableSync.h"

#include <boost/logic/tribool.hpp>
#include <assert.h>

using boost::logic::tribool;
using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace impex;
	using namespace security;
	using namespace calendar;

	template<> const string util::FactorableTemplate<DBTableSync,TransportNetworkTableSync>::FACTORY_KEY(
		"35.20.02 Network transport"
	);

	namespace pt
	{
		const string TransportNetworkTableSync::COL_NAME("name");
		const string TransportNetworkTableSync::COL_CREATOR_ID("creator_id");
		const string TransportNetworkTableSync::COL_DAYS_CALENDARS_PARENT_ID("days_calendars_parent_id");
		const string TransportNetworkTableSync::COL_PERIODS_CALENDARS_PARENT_ID("periods_calendars_parent_id");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TransportNetworkTableSync>::TABLE(
			"t022_transport_networks"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<TransportNetworkTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(TransportNetworkTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(TransportNetworkTableSync::COL_CREATOR_ID, SQL_TEXT),
			DBTableSync::Field(TransportNetworkTableSync::COL_DAYS_CALENDARS_PARENT_ID, SQL_INTEGER),
			DBTableSync::Field(TransportNetworkTableSync::COL_PERIODS_CALENDARS_PARENT_ID, SQL_INTEGER),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<TransportNetworkTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(TransportNetworkTableSync::COL_CREATOR_ID.c_str(), ""),
			DBTableSync::Index(TransportNetworkTableSync::COL_NAME.c_str(), ""),
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::Load(
			TransportNetwork* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Name
			std::string name (rows->getText (TransportNetworkTableSync::COL_NAME));
			object->setName(name);

			// Data source links
			std::string creatorId(rows->getText (TransportNetworkTableSync::COL_CREATOR_ID));
			object->setDataSourceLinks(ImportableTableSync::GetDataSourceLinksFromSerializedString(creatorId, env));

			{ // Days calendars parent
				object->setDaysCalendarsParent(NULL);
				RegistryKeyType id(rows->getLongLong(TransportNetworkTableSync::COL_DAYS_CALENDARS_PARENT_ID));
				if(id > 0) try
				{
					object->setDaysCalendarsParent(
						CalendarTemplateTableSync::GetEditable(
							id, env, linkLevel
						).get()
					);
				}
				catch(ObjectNotFoundException<CalendarTemplate>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + TransportNetworkTableSync::COL_DAYS_CALENDARS_PARENT_ID, e);
			}	}

			{ // Periods calendars parent
				object->setPeriodsCalendarsParent(NULL);
				RegistryKeyType id(rows->getLongLong(TransportNetworkTableSync::COL_PERIODS_CALENDARS_PARENT_ID));
				if(id > 0) try
				{
					object->setPeriodsCalendarsParent(
						CalendarTemplateTableSync::GetEditable(
							id, env, linkLevel
						).get()
					);
				}
				catch(ObjectNotFoundException<CalendarTemplate>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + TransportNetworkTableSync::COL_PERIODS_CALENDARS_PARENT_ID, e);
			}	}
		}



		template<> void DBDirectTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::Save(
			TransportNetwork* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<TransportNetworkTableSync> query(*object);
			query.addField(object->getName());
			query.addField(ImportableTableSync::SerializeDataSourceLinks(object->getDataSourceLinks()));
			query.addField(object->getDaysCalendarsParent() ? object->getDaysCalendarsParent()->getKey() : RegistryKeyType(0));
			query.addField(object->getPeriodsCalendarsParent() ? object->getPeriodsCalendarsParent()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::Unlink(
			TransportNetwork* object
		){
		}



		template<> bool DBTableSyncTemplate<TransportNetworkTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<TransportNetworkTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			CommercialLineTableSync::SearchResult lines(CommercialLineTableSync::Search(env, id));
			BOOST_FOREACH(const CommercialLineTableSync::SearchResult::value_type& line, lines)
			{
				CommercialLineTableSync::Remove(NULL, line->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<TransportNetworkTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<TransportNetworkTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		TransportNetworkTableSync::SearchResult TransportNetworkTableSync::Search(
			Env& env,
			string name,
			string creatorId,
			int first, /*= 0*/
			boost::optional<std::size_t> number,
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<TransportNetworkTableSync> query;
			if(!name.empty())
			{
				query.addWhereField(COL_NAME, name, ComposedExpression::OP_LIKE);
			}
			if (!creatorId.empty())
			{
				query.addWhereField(COL_CREATOR_ID, creatorId, ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
			}
			if(number)
			{
				query.setNumber(*number + 1);
			}
			if(first > 0)
			{
				query.setFirst(first);
			}
			return LoadFromQuery(query, env, linkLevel);
		}
}	}
