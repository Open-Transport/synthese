
/** ServiceCalendarLinkTableSync class implementation.
	@file ServiceCalendarLinkTableSync.cpp

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

#include <sstream>

#include "ServiceCalendarLinkTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "CalendarTemplateTableSync.h"
#include "Fetcher.h"
#include "NonPermanentService.h"
#include "TransportNetworkRight.h"
#include "ServiceCalendarLink.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace calendar;
	using namespace graph;
	using namespace pt;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync,ServiceCalendarLinkTableSync>::FACTORY_KEY(
		"35.80.01 Service calendar links"
	);

	namespace pt
	{
		const string ServiceCalendarLinkTableSync::COL_SERVICE_ID = "service_id";
		const string ServiceCalendarLinkTableSync::COL_START_DATE = "start_date";
		const string ServiceCalendarLinkTableSync::COL_END_DATE = "end_date";
		const string ServiceCalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID = "calendar_id";
		const string ServiceCalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID2 = "calendar2_id";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ServiceCalendarLinkTableSync>::TABLE(
			"t076_service_calendar_links"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<ServiceCalendarLinkTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(ServiceCalendarLinkTableSync::COL_SERVICE_ID, SQL_INTEGER),
			DBTableSync::Field(ServiceCalendarLinkTableSync::COL_START_DATE, SQL_TEXT),
			DBTableSync::Field(ServiceCalendarLinkTableSync::COL_END_DATE, SQL_TEXT),
			DBTableSync::Field(ServiceCalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID, SQL_INTEGER),
			DBTableSync::Field(ServiceCalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID2, SQL_INTEGER),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<ServiceCalendarLinkTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(ServiceCalendarLinkTableSync::COL_SERVICE_ID.c_str(), ""),
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<ServiceCalendarLinkTableSync,ServiceCalendarLink>::Load(
			ServiceCalendarLink* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Start date
			object->setStartDate(rows->getDate(ServiceCalendarLinkTableSync::COL_START_DATE));

			// End date
			object->setEndDate(rows->getDate(ServiceCalendarLinkTableSync::COL_END_DATE));

			if (linkLevel == UP_LINKS_LOAD_LEVEL || linkLevel == UP_DOWN_LINKS_LOAD_LEVEL || linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL)
			{
				// Calendar template
				object->setCalendarTemplate(NULL);
				RegistryKeyType calendarTemplateId(rows->getLongLong(ServiceCalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID));
				if(calendarTemplateId > 0) try
				{
					object->setCalendarTemplate(CalendarTemplateTableSync::GetEditable(calendarTemplateId, env).get());
				}
				catch(ObjectNotFoundException<CalendarTemplate>)
				{
					Log::GetInstance().warn("Bad value " + lexical_cast<string>(calendarTemplateId) + " for calendar in service calendar link " + lexical_cast<string>(object->getKey()));
				}

				// Calendar template 2
				object->setCalendarTemplate2(NULL);
				RegistryKeyType calendarTemplateId2(rows->getLongLong(ServiceCalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID2));
				if(calendarTemplateId2 > 0) try
				{
					object->setCalendarTemplate2(CalendarTemplateTableSync::GetEditable(calendarTemplateId2, env).get());
				}
				catch(ObjectNotFoundException<CalendarTemplate>)
				{
					Log::GetInstance().warn("Bad value " + lexical_cast<string>(calendarTemplateId) + " for calendar 2 in service calendar link " + lexical_cast<string>(object->getKey()));
				}

				// Service
				// Must stay at the last position because the service reads the object content
				object->setService(NULL);
				RegistryKeyType serviceId(rows->getLongLong(ServiceCalendarLinkTableSync::COL_SERVICE_ID));
				if(serviceId > 0) try
				{
					object->setService(dynamic_cast<NonPermanentService*>(Fetcher<Service>::FetchEditable(serviceId, env).get()));
					if(linkLevel == UP_DOWN_LINKS_LOAD_LEVEL || linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL)
					{
						object->getService()->addCalendarLink(*object, true);
					}
				}
				catch(ObjectNotFoundException<Service>)
				{
					Log::GetInstance().warn("Bad value " + lexical_cast<string>(serviceId) + " for service in service calendar link " + lexical_cast<string>(object->getKey()));
				}
			}
		}



		template<> void DBDirectTableSyncTemplate<ServiceCalendarLinkTableSync,ServiceCalendarLink>::Save(
			ServiceCalendarLink* object,
			optional<DBTransaction&> transaction
		){
			if(!object->getService()) throw Exception("ServiceCalendarLink save error. Missing service");
			
			ReplaceQuery<ServiceCalendarLinkTableSync> query(*object);
			query.addField(object->getService()->getKey());
			query.addField((object->getStartDate().is_special() || object->getStartDate().is_not_a_date()) ? string() : to_iso_extended_string(object->getStartDate()));
			query.addField((object->getEndDate().is_special() || object->getEndDate().is_not_a_date()) ? string() : to_iso_extended_string(object->getEndDate()));
			query.addField(object->getCalendarTemplate() ? object->getCalendarTemplate()->getKey() : RegistryKeyType(0));
			query.addField(object->getCalendarTemplate2() ? object->getCalendarTemplate2()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<ServiceCalendarLinkTableSync,ServiceCalendarLink>::Unlink(ServiceCalendarLink* obj)
		{
			if(obj->getService())
			{
				obj->getService()->removeCalendarLink(*obj, true);
			}
		}



		template<> bool DBTableSyncTemplate<ServiceCalendarLinkTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<ServiceCalendarLinkTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ServiceCalendarLinkTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ServiceCalendarLinkTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		ServiceCalendarLinkTableSync::SearchResult ServiceCalendarLinkTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> serviceId,
			int first,
			boost::optional<std::size_t> number,
			bool orderByService,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<ServiceCalendarLinkTableSync> query;
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
