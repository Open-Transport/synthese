
/** CalendarTemplateElementTableSync class implementation.
	@file CalendarTemplateElementTableSync.cpp
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

#include <sstream>

#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplateElement.h"
#include "Log.h"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "SelectQuery.hpp"
#include "ReplaceQuery.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,CalendarTemplateElementTableSync>::FACTORY_KEY("55.11 Calendar Template Elements");
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
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<CalendarTemplateElementTableSync>::TABLE(
			"t055_calendar_template_elements"
		);
		
		
		
		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<CalendarTemplateElementTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_CALENDAR_ID, SQL_INTEGER),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_RANK, SQL_INTEGER),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_MIN_DATE, SQL_DATE),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_MAX_DATE, SQL_DATE),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_INTERVAL, SQL_INTEGER),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_POSITIVE, SQL_INTEGER),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_INCLUDE_ID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};



		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<CalendarTemplateElementTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(CalendarTemplateElementTableSync::COL_CALENDAR_ID.c_str(), ""),
			SQLiteTableSync::Index(CalendarTemplateElementTableSync::COL_INCLUDE_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>::Load(
			CalendarTemplateElement* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Columns reading
			RegistryKeyType id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setRank(static_cast<size_t>(rows->getInt(CalendarTemplateElementTableSync::COL_RANK)));
			object->setMinDate(
				rows->getText(CalendarTemplateElementTableSync::COL_MIN_DATE).empty() ?
				date(neg_infin) :
				from_string(rows->getText(CalendarTemplateElementTableSync::COL_MIN_DATE))
			);
			object->setMaxDate(
				rows->getText(CalendarTemplateElementTableSync::COL_MAX_DATE).empty() ?
				date(pos_infin) :
				from_string(rows->getText(CalendarTemplateElementTableSync::COL_MAX_DATE))
			);
			object->setInterval(days(rows->getInt(CalendarTemplateElementTableSync::COL_INTERVAL)));
			object->setOperation(
				static_cast<CalendarTemplateElement::Operation>(
					rows->getInt(CalendarTemplateElementTableSync::COL_POSITIVE)
			)	);

			if(linkLevel == UP_LINKS_LOAD_LEVEL || linkLevel == UP_DOWN_LINKS_LOAD_LEVEL || linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL)
			{
				try
				{
					boost::shared_ptr<CalendarTemplate> cal(CalendarTemplateTableSync::GetEditable(rows->getLongLong(CalendarTemplateElementTableSync::COL_CALENDAR_ID), env, FIELDS_ONLY_LOAD_LEVEL));
					object->setCalendar(cal.get());
					cal->addElement(*object);
				}
				catch (ObjectNotFoundException<CalendarTemplate> e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + CalendarTemplateElementTableSync::COL_CALENDAR_ID, e);
				}
			}

			if(linkLevel == DOWN_LINKS_LOAD_LEVEL || linkLevel == UP_DOWN_LINKS_LOAD_LEVEL || linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL)
			{
				try
				{
					RegistryKeyType iid(rows->getLongLong(CalendarTemplateElementTableSync::COL_INCLUDE_ID));
					if(iid > 0)
					{
						object->setInclude(CalendarTemplateTableSync::Get(rows->getLongLong(CalendarTemplateElementTableSync::COL_INCLUDE_ID), env, UP_DOWN_LINKS_LOAD_LEVEL).get());
					}
				}
				catch (ObjectNotFoundException<CalendarTemplate> e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + CalendarTemplateElementTableSync::COL_INCLUDE_ID, e);
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>::Save(
			CalendarTemplateElement* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<CalendarTemplateElementTableSync> query(*object);
			query.addField(object->getCalendar() ? object->getCalendar()->getKey() : RegistryKeyType(0));
			query.addField(object->getRank());
			query.addField(object->getMinDate().is_special() ? string() : to_iso_extended_string(object->getMinDate()));
			query.addField(object->getMaxDate().is_special() ? string() : to_iso_extended_string(object->getMaxDate()));
			query.addField(object->getInterval().days());
			query.addField(static_cast<int>(object->getOperation()));
			query.addField(object->getInclude() ? object->getInclude()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>::Unlink(
			CalendarTemplateElement* obj
		){
			if(obj->getCalendar())
			{
				const_cast<CalendarTemplate*>(obj->getCalendar())->removeElement(*obj);
			}
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



		void CalendarTemplateElementTableSync::Shift( RegistryKeyType calendarId , int rank , int delta )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "UPDATE " << TABLE.NAME
				<< " SET " << COL_RANK << "=" << COL_RANK << ((delta > 0) ? "+" : "") << delta
				<< " WHERE " << COL_CALENDAR_ID << "=" << calendarId
				<< " AND " << COL_RANK << ">=" << rank
				;

			sqlite->execUpdate(query.str());
		}



		int CalendarTemplateElementTableSync::GetMaxRank( RegistryKeyType calendarId )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "SELECT MAX(" << COL_RANK << ") AS mr "
				<< " FROM " << TABLE.NAME
				<< " WHERE " << COL_CALENDAR_ID << "=" << calendarId
				;

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				while (rows->next ())
				{
					return rows->getText("mr").empty() ? UNKNOWN_VALUE : rows->getInt("mr");
				}
				return UNKNOWN_VALUE;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}



		void CalendarTemplateElementTableSync::Clean( util::RegistryKeyType calendarId )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "DELETE FROM " << TABLE.NAME
				<< " WHERE " << COL_CALENDAR_ID << "=" << calendarId
			;

			sqlite->execUpdate(query.str());
		}
	}
}
