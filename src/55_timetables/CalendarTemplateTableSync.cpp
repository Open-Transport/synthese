
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

#include <sstream>

#include "CalendarTemplateTableSync.h"
#include "CalendarTemplate.h"

#include "CalendarTemplateElement.h"
#include "CalendarTemplateElementTableSync.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace timetables;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,CalendarTemplateTableSync>::FACTORY_KEY("55.10 Calendar templates");
	}
	
	namespace timetables
	{
		const std::string CalendarTemplateTableSync::COL_TEXT("name");
	}
	
	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<CalendarTemplateTableSync>::TABLE(
			"t054_calendar_templates"
		);
		

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<CalendarTemplateTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(CalendarTemplateTableSync::COL_TEXT, SQL_TEXT),
			SQLiteTableSync::Field()
		};


		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<CalendarTemplateTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>::Load(
			CalendarTemplate* object
			, const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setText(rows->getText(CalendarTemplateTableSync::COL_TEXT));
			
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				Env env;
				CalendarTemplateElementTableSync::Search(
					env,
					object->getKey()
				);
				BOOST_FOREACH(shared_ptr<CalendarTemplateElement> e, env.getRegistry<CalendarTemplateElement>())
				{
					object->addElement(*e);
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>::Save(
			CalendarTemplate* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getText())
				<< ")";
			sqlite->execUpdate(query.str());
		}


		
		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>::Unlink(
			CalendarTemplate* obj
		){
		}
	}
	
	
	
	namespace timetables
	{
		CalendarTemplateTableSync::CalendarTemplateTableSync()
			: SQLiteRegistryTableSyncTemplate<CalendarTemplateTableSync, CalendarTemplate>()
		{
		}



		void CalendarTemplateTableSync::Search(
			Env& env,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			/// @todo Fill Where criteria
			// if (!name.empty())
			// 	query << " AND " << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
				;
			//if (orderByName)
			//	query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
