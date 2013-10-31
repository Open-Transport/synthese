
/** CalendarLinkTableSync class implementation.
	@file CalendarLinkTableSync.cpp

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

#include "CalendarLinkTableSync.hpp"

#include "Calendar.h"
#include "CalendarLink.hpp"
#include "CalendarTemplateTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace calendar;

	template<> const string util::FactorableTemplate<DBTableSync, CalendarLinkTableSync>::FACTORY_KEY(
		"90.01 Calendar links"
	);

	namespace calendar
	{
		const string CalendarLinkTableSync::COL_SERVICE_ID = "service_id";
		const string CalendarLinkTableSync::COL_START_DATE = "start_date";
		const string CalendarLinkTableSync::COL_END_DATE = "end_date";
		const string CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID = "calendar_id";
		const string CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID2 = "calendar2_id";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CalendarLinkTableSync>::TABLE(
			"t076_service_calendar_links"
		);



		template<> const Field DBTableSyncTemplate<CalendarLinkTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(CalendarLinkTableSync::COL_SERVICE_ID, SQL_INTEGER),
			Field(CalendarLinkTableSync::COL_START_DATE, SQL_TEXT),
			Field(CalendarLinkTableSync::COL_END_DATE, SQL_TEXT),
			Field(CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID, SQL_INTEGER),
			Field(CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID2, SQL_INTEGER),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<CalendarLinkTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(CalendarLinkTableSync::COL_SERVICE_ID.c_str(), ""));
			return r;
		}



		template<>
		void OldLoadSavePolicy<CalendarLinkTableSync,CalendarLink>::Load(
			CalendarLink* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			DBModule::LoadObjects(object->getLinkedObjectsIds(*rows), env, linkLevel);
			object->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				object->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<>
		void OldLoadSavePolicy<CalendarLinkTableSync, CalendarLink>::Save(
			CalendarLink* object,
			optional<DBTransaction&> transaction
		){
			if(!object->getCalendar()) throw Exception("CalendarLink save error. Missing service");

			ReplaceQuery<CalendarLinkTableSync> query(*object);
			query.addField(object->getCalendar()->getKey());
			query.addField((object->getStartDate().is_special() || object->getStartDate().is_not_a_date()) ? string() : to_iso_extended_string(object->getStartDate()));
			query.addField((object->getEndDate().is_special() || object->getEndDate().is_not_a_date()) ? string() : to_iso_extended_string(object->getEndDate()));
			query.addField(object->getCalendarTemplate() ? object->getCalendarTemplate()->getKey() : RegistryKeyType(0));
			query.addField(object->getCalendarTemplate2() ? object->getCalendarTemplate2()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<>
		void OldLoadSavePolicy<CalendarLinkTableSync,CalendarLink>::Unlink(CalendarLink* obj)
		{
			if(obj->getCalendar())
			{
				obj->getCalendar()->removeCalendarLink(*obj, true);
			}
		}



		template<> bool DBTableSyncTemplate<CalendarLinkTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true; // TODO
		}



		template<> void DBTableSyncTemplate<CalendarLinkTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CalendarLinkTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CalendarLinkTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace calendar
	{
		CalendarLinkTableSync::SearchResult CalendarLinkTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> serviceId,
			int first,
			boost::optional<std::size_t> number,
			bool orderByService,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<CalendarLinkTableSync> query;
			if (serviceId)
			{
				query.addWhereField(COL_SERVICE_ID, *serviceId);
			}
			if (orderByService)
			{
				query.addOrderField(COL_SERVICE_ID, raisingOrder);
				query.addOrderField(COL_START_DATE, raisingOrder);
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
}	}
