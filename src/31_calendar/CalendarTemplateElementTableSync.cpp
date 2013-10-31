
/** CalendarTemplateElementTableSync class implementation.
	@file CalendarTemplateElementTableSync.cpp
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

#include "CalendarTemplateElementTableSync.h"

#include "CalendarTemplateElement.h"
#include "CalendarRight.h"
#include "DaysField.hpp"
#include "Log.h"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "DeleteQuery.hpp"
#include "Profile.h"
#include "RankUpdateQuery.hpp"
#include "ReplaceQuery.h"
#include "Session.h"
#include "SelectQuery.hpp"
#include "UpdateQuery.hpp"
#include "User.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace calendar;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,CalendarTemplateElementTableSync>::FACTORY_KEY("55.11 Calendar Template Elements");
	}

	namespace calendar
	{
		const std::string CalendarTemplateElementTableSync::COL_CALENDAR_ID("calendar_id");
		const std::string CalendarTemplateElementTableSync::COL_RANK("rank");
		const std::string CalendarTemplateElementTableSync::COL_MIN_DATE("min_date");
		const std::string CalendarTemplateElementTableSync::COL_MAX_DATE("max_date");
		const std::string CalendarTemplateElementTableSync::COL_INTERVAL("interval");
		const std::string CalendarTemplateElementTableSync::COL_POSITIVE("positive");
		const std::string CalendarTemplateElementTableSync::COL_INCLUDE_ID("include_id");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CalendarTemplateElementTableSync>::TABLE(
			"t055_calendar_template_elements"
		);



		template<> const Field DBTableSyncTemplate<CalendarTemplateElementTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(CalendarTemplateElementTableSync::COL_CALENDAR_ID, SQL_INTEGER),
			Field(CalendarTemplateElementTableSync::COL_RANK, SQL_INTEGER),
			Field(CalendarTemplateElementTableSync::COL_MIN_DATE, SQL_DATE),
			Field(CalendarTemplateElementTableSync::COL_MAX_DATE, SQL_DATE),
			Field(CalendarTemplateElementTableSync::COL_INTERVAL, SQL_INTEGER),
			Field(CalendarTemplateElementTableSync::COL_POSITIVE, SQL_INTEGER),
			Field(CalendarTemplateElementTableSync::COL_INCLUDE_ID, SQL_INTEGER),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<CalendarTemplateElementTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(CalendarTemplateElementTableSync::COL_CALENDAR_ID.c_str(), ""));
			r.push_back(DBTableSync::Index(CalendarTemplateElementTableSync::COL_INCLUDE_ID.c_str(), ""));
			return r;
		}



		template<>
		void OldLoadSavePolicy<CalendarTemplateElementTableSync,CalendarTemplateElement>::Load(
			CalendarTemplateElement* object,
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
		void OldLoadSavePolicy<CalendarTemplateElementTableSync,CalendarTemplateElement>::Save(
			CalendarTemplateElement* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<CalendarTemplateElementTableSync> query(*object);
			query.addField(object->getCalendar() ? object->getCalendar()->getKey() : RegistryKeyType(0));
			query.addField(object->getRank());
			query.addField(object->getMinDate().is_special() ? string() : to_iso_extended_string(object->getMinDate()));
			query.addField(object->getMaxDate().is_special() ? string() : to_iso_extended_string(object->getMaxDate()));
			query.addFrameworkField<DaysField>(object->getStep());
			query.addField(static_cast<int>(object->getOperation()));
			query.addField(object->getInclude() ? object->getInclude()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<>
		void OldLoadSavePolicy<CalendarTemplateElementTableSync,CalendarTemplateElement>::Unlink(
			CalendarTemplateElement* obj
		){
			obj->unlink();
		}



		template<> bool DBTableSyncTemplate<CalendarTemplateElementTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CalendarRight>(WRITE);
		}



		template<> void DBTableSyncTemplate<CalendarTemplateElementTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CalendarTemplateElementTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			try
			{
				Env env;
				boost::shared_ptr<const CalendarTemplateElement> element(CalendarTemplateElementTableSync::Get(id,env));
				if(element->getCalendar())
				{
					CalendarTemplateElementTableSync::Shift(element->getCalendar()->getKey(), element->getRank(), -1, transaction);
				}
			}
			catch(ObjectNotFoundException<CalendarTemplateElement>&)
			{

			}
		}



		template<> void DBTableSyncTemplate<CalendarTemplateElementTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log calendar template element removal
		}
	}



	namespace calendar
	{
		CalendarTemplateElementTableSync::SearchResult CalendarTemplateElementTableSync::Search(
			Env& env,
			optional<RegistryKeyType> calendarId,
			optional<RegistryKeyType> calendarIncludeId,
			int first /*= 0*/,
			boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<CalendarTemplateElementTableSync> query;
			if(calendarId)
			{
				query.addWhereField(COL_CALENDAR_ID, *calendarId);
			}
			if(calendarIncludeId)
			{
				query.addWhereField(COL_INCLUDE_ID, *calendarIncludeId);
			}
			query.addOrderField(COL_RANK, true);
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



		void CalendarTemplateElementTableSync::Shift(
			RegistryKeyType calendarId,
			size_t rank,
			int delta,
			boost::optional<DBTransaction&> transaction
		){
			RankUpdateQuery<CalendarTemplateElementTableSync> query(COL_RANK, delta, rank);
			query.addWhereField(COL_CALENDAR_ID, calendarId);
			query.execute(transaction);
		}



		optional<size_t> CalendarTemplateElementTableSync::GetMaxRank(
			RegistryKeyType calendarId
		){
			DB* db = DBModule::GetDB();

			stringstream query;

			// Content
			query
				<< "SELECT MAX(" << COL_RANK << ") AS mr "
				<< " FROM " << TABLE.NAME
				<< " WHERE " << COL_CALENDAR_ID << "=" << calendarId
				;

			try
			{
				DBResultSPtr rows = db->execQuery(query.str());
				while (rows->next ())
				{
					return rows->getOptionalUnsignedInt("mr");
				}
				return optional<size_t>();
			}
			catch(DBException& e)
			{
				throw Exception(e.getMessage());
			}
		}



		void CalendarTemplateElementTableSync::Clean(
			RegistryKeyType calendarId,
			boost::optional<DBTransaction&> transaction
		){
			DeleteQuery<CalendarTemplateElementTableSync> query;
			query.addWhereField(COL_CALENDAR_ID, calendarId);
			query.execute(transaction);
		}
	}
}
