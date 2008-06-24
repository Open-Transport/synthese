
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

#include "35_timetables/CalendarTemplateElement.h"
#include "35_timetables/CalendarTemplateElementTableSync.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

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
	
	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<CalendarTemplateTableSync>::TABLE_NAME("t054_calendar_templates");
		template<> const int SQLiteTableSyncTemplate<CalendarTemplateTableSync>::TABLE_ID(54);
		template<> const bool SQLiteTableSyncTemplate<CalendarTemplateTableSync>::HAS_AUTO_INCREMENT(true);



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>::load(
			CalendarTemplate* object
			, const db::SQLiteResultSPtr& rows
		){
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setText(rows->getText(CalendarTemplateTableSync::COL_TEXT));
		}



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>::save(
			CalendarTemplate* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getText())
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>::_link(
			CalendarTemplate* object
			, const SQLiteResultSPtr& rows
			, GetSource temporary
		){
			vector<shared_ptr<CalendarTemplateElement> > elements(CalendarTemplateElementTableSync::Search(object->getKey()));
			for (vector<shared_ptr<CalendarTemplateElement> >::const_iterator it(elements.begin()); it != elements.end(); ++it)
				object->addElement(**it);
		}


		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>::_unlink(
			CalendarTemplate* obj
		){
		}
	}
	
	
	
	namespace timetables
	{
		const std::string CalendarTemplateTableSync::COL_TEXT("name");



		CalendarTemplateTableSync::CalendarTemplateTableSync()
			: SQLiteRegistryTableSyncTemplate<CalendarTemplateTableSync, CalendarTemplate>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_TEXT, "TEXT");
		}



		vector<shared_ptr<CalendarTemplate> > CalendarTemplateTableSync::Search(int first /*= 0*/, int number /*= 0*/ )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
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

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<CalendarTemplate> > objects;
				while (rows->next ())
				{
					shared_ptr<CalendarTemplate> object(new CalendarTemplate);
					load(object.get(), rows);
					link(object.get(), rows, GET_AUTO);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
