
//////////////////////////////////////////////////////////////////////////
///	FreeDRTTimeSlotTableSync class implementation.
///	@file FreeDRTTimeSlotTableSync.cpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <sstream>

#include "FreeDRTTimeSlotTableSync.hpp"

#include "CalendarLink.hpp"
#include "CalendarLinkTableSync.hpp"
#include "CityTableSync.h"
#include "CommercialLineTableSync.h"
#include "FreeDRTAreaTableSync.hpp"
#include "PTUseRuleTableSync.h"
#include "ReplaceQuery.h"
#include "RollingStock.hpp"
#include "SelectQuery.hpp"

#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace geography;
	using namespace graph;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,FreeDRTTimeSlotTableSync>::FACTORY_KEY("35.81 Free DRT Time slots");
		template<> const string FactorableTemplate<Fetcher<Calendar>, FreeDRTTimeSlotTableSync>::FACTORY_KEY("83");
	}

	namespace pt
	{
		const string FreeDRTTimeSlotTableSync::COL_AREA_ID = "area_id";
		const string FreeDRTTimeSlotTableSync::COL_SERVICE_NUMBER = "service_number";
		const string FreeDRTTimeSlotTableSync::COL_FIRST_DEPARTURE = "first_departure";
		const string FreeDRTTimeSlotTableSync::COL_LAST_ARRIVAL = "last_arrival";
		const string FreeDRTTimeSlotTableSync::COL_MAX_CAPACITY = "max_capacity";
		const string FreeDRTTimeSlotTableSync::COL_COMMERCIAL_SPEED = "commercial_speed";
		const string FreeDRTTimeSlotTableSync::COL_MAX_SPEED = "max_speed";
		const string FreeDRTTimeSlotTableSync::COL_USE_RULES = "use_rules";
		const string FreeDRTTimeSlotTableSync::COL_DATES = "dates";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::TABLE(
			"t083_free_drt_time_slots"
		);



		template<> const Field DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(FreeDRTTimeSlotTableSync::COL_AREA_ID, SQL_INTEGER),
			Field(FreeDRTTimeSlotTableSync::COL_SERVICE_NUMBER, SQL_TEXT),
			Field(FreeDRTTimeSlotTableSync::COL_FIRST_DEPARTURE, SQL_TEXT),
			Field(FreeDRTTimeSlotTableSync::COL_LAST_ARRIVAL, SQL_TEXT),
			Field(FreeDRTTimeSlotTableSync::COL_MAX_CAPACITY, SQL_INTEGER),
			Field(FreeDRTTimeSlotTableSync::COL_COMMERCIAL_SPEED, SQL_DOUBLE),
			Field(FreeDRTTimeSlotTableSync::COL_MAX_SPEED, SQL_DOUBLE),
			Field(FreeDRTTimeSlotTableSync::COL_USE_RULES, SQL_TEXT),
			Field(FreeDRTTimeSlotTableSync::COL_DATES, SQL_TEXT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					FreeDRTTimeSlotTableSync::COL_AREA_ID.c_str(),
			"")	);
			return r;
		};



		template<> void OldLoadSavePolicy<FreeDRTTimeSlotTableSync,FreeDRTTimeSlot>::Load(
			FreeDRTTimeSlot* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Area
			object->setArea(NULL);
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType areaId(rows->getLongLong(FreeDRTTimeSlotTableSync::COL_AREA_ID));
				try
				{
					FreeDRTArea* area(FreeDRTAreaTableSync::GetEditable(areaId, env, linkLevel).get());
					object->setArea(area);
				}
				catch(ObjectNotFoundException<CommercialLine>)
				{
					Log::GetInstance().warn("Bad value " + lexical_cast<string>(areaId) + " for aera in free DRT time slot " + lexical_cast<string>(object->getKey()));
				}
			}

			// Service number
			object->setServiceNumber(rows->getText(FreeDRTTimeSlotTableSync::COL_SERVICE_NUMBER));

			// First departure
			object->setFirstDeparture(time_duration(not_a_date_time));
			if(!rows->getText(FreeDRTTimeSlotTableSync::COL_FIRST_DEPARTURE).empty())
			{
				object->setFirstDeparture(duration_from_string(rows->getText(FreeDRTTimeSlotTableSync::COL_FIRST_DEPARTURE)));
			}

			// Last arrival
			object->setLastArrival(time_duration(not_a_date_time));
			if(!rows->getText(FreeDRTTimeSlotTableSync::COL_LAST_ARRIVAL).empty())
			{
				object->setLastArrival(duration_from_string(rows->getText(FreeDRTTimeSlotTableSync::COL_LAST_ARRIVAL)));
			}

			// Max capacity
			object->setMaxCapacity(rows->getOptionalUnsignedInt(FreeDRTTimeSlotTableSync::COL_MAX_CAPACITY));

			// Commercial speed
			object->setCommercialSpeed(lexical_cast<FreeDRTTimeSlot::KMHSpeed>(rows->getText(FreeDRTTimeSlotTableSync::COL_COMMERCIAL_SPEED)));

			// Max speed
			object->setMaxSpeed(lexical_cast<FreeDRTTimeSlot::KMHSpeed>(rows->getText(FreeDRTTimeSlotTableSync::COL_MAX_SPEED)));

			// Use rules
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				object->setRules(
					PTUseRuleTableSync::UnserializeUseRules(
						rows->getText(FreeDRTTimeSlotTableSync::COL_USE_RULES),
						env,
						linkLevel
				)	);
			}

			// Calendar
			if(linkLevel == DOWN_LINKS_LOAD_LEVEL || linkLevel == UP_DOWN_LINKS_LOAD_LEVEL || linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL)
			{
				// Search of calendar template links (overrides manually defined calendar)
				CalendarLinkTableSync::SearchResult links(
					CalendarLinkTableSync::Search(
						env,
						object->getKey()
				)	); // UP_LINK_LOAD_LEVEL to avoid multiple calls to setCalendarFromLinks
				if(links.empty())
				{
					object->setFromSerializedString(rows->getText(FreeDRTTimeSlotTableSync::COL_DATES));
				}
				else
				{
					BOOST_FOREACH(const boost::shared_ptr<CalendarLink>& link, links)
					{
						object->addCalendarLink(*link, false);
					}
				}
			}
			else
			{
				object->setFromSerializedString(rows->getText(FreeDRTTimeSlotTableSync::COL_DATES));
			}


			// Registration in the line and in the path
			if(	linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL &&
				object->getArea()
			){
				// Registration in the path
				object->getArea()->addService(*object, false);

				// Registration in the line
				if(object->getArea()->getLine())
				{
					object->getArea()->getLine()->registerService(*object);
				}
			}
		}



		template<> void OldLoadSavePolicy<FreeDRTTimeSlotTableSync,FreeDRTTimeSlot>::Save(
			FreeDRTTimeSlot* object,
			optional<DBTransaction&> transaction
		){
			// Dates preparation
			stringstream datesStr;
			if(object->getCalendarLinks().empty())
			{
				object->serialize(datesStr);
			}

			ReplaceQuery<FreeDRTTimeSlotTableSync> query(*object);
			query.addField(object->getArea() ? object->getArea()->getKey() : RegistryKeyType(0));
			query.addField(object->getServiceNumber());
			query.addField(object->getFirstDeparture().is_not_a_date_time() ? string() : to_simple_string(object->getFirstDeparture()));
			query.addField(object->getLastArrival().is_not_a_date_time() ? string() : to_simple_string(object->getLastArrival()));
			query.addField(object->getMaxCapacity() ? lexical_cast<string>(*object->getMaxCapacity()) : string());
			query.addField(object->getCommercialSpeed());
			query.addField(object->getMaxSpeed());
			query.addField(PTUseRuleTableSync::SerializeUseRules(object->getRules()));
			query.addField(
				datesStr.str()
			);
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<FreeDRTTimeSlotTableSync,FreeDRTTimeSlot>::Unlink(
			FreeDRTTimeSlot* obj
		){
			if(obj->getArea())
			{
				// Unregister from the area
				obj->getPath()->removeService(*obj);

				// Unregister from the line
				obj->getArea()->getLine()->unregisterService(*obj);
			}
		}



		template<> bool DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<FreeDRTTimeSlotTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}



	namespace pt
	{
		FreeDRTTimeSlotTableSync::SearchResult FreeDRTTimeSlotTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> areaId,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByServiceNumber,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<FreeDRTTimeSlotTableSync> query;
			if(areaId)
			{
			 	query.addWhereField(COL_AREA_ID, *areaId);
			}
			if(orderByServiceNumber)
			{
			 	query.addOrderField(COL_SERVICE_NUMBER, raisingOrder);
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
