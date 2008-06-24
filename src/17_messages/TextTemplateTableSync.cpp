
/** TextTemplateTableSync class implementation.
	@file TextTemplateTableSync.cpp

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

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "TextTemplate.h"
#include "TextTemplateTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,TextTemplateTableSync>::FACTORY_KEY("17.10.10 Text templates");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<TextTemplateTableSync>::TABLE_NAME = "t038_text_templates";
		template<> const int SQLiteTableSyncTemplate<TextTemplateTableSync>::TABLE_ID = 38;
		template<> const bool SQLiteTableSyncTemplate<TextTemplateTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<TextTemplateTableSync,TextTemplate>::load(TextTemplate* object, const db::SQLiteResultSPtr& rows )
		{
			object->setKey(rows->getLongLong (TABLE_COL_ID));
			object->setName(rows->getText (TextTemplateTableSync::COL_NAME));
			object->setShortMessage(rows->getText ( TextTemplateTableSync::COL_SHORT_TEXT));
			object->setLongMessage(rows->getText ( TextTemplateTableSync::COL_LONG_TEXT));
			object->setAlarmLevel(static_cast<AlarmLevel>(rows->getInt ( TextTemplateTableSync::COL_LEVEL)));
			object->setIsFolder(rows->getBool(TextTemplateTableSync::COL_IS_FOLDER));
			object->setParentId(rows->getLongLong(TextTemplateTableSync::COL_PARENT_ID));
		}


		template<> void SQLiteDirectTableSyncTemplate<TextTemplateTableSync,TextTemplate>::_link(TextTemplate* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{
		}

		template<> void SQLiteDirectTableSyncTemplate<TextTemplateTableSync,TextTemplate>::_unlink(TextTemplate* obj)
		{
		}

		template<> void SQLiteDirectTableSyncTemplate<TextTemplateTableSync,TextTemplate>::save(TextTemplate* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
            query
				<< "REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getName())
				<< "," << Conversion::ToSQLiteString(object->getShortMessage())
				<< "," << Conversion::ToSQLiteString(object->getLongMessage())
				<< "," << Conversion::ToString(static_cast<int>(object->getAlarmLevel()))
				<< "," << Conversion::ToString(object->getIsFolder())
				<< "," << Conversion::ToString(object->getParentId())
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace messages
	{
		const string TextTemplateTableSync::COL_NAME = "name";
		const string TextTemplateTableSync::COL_SHORT_TEXT = "short_text";
		const string TextTemplateTableSync::COL_LONG_TEXT = "long_text";
		const string TextTemplateTableSync::COL_LEVEL = "level";
		const string TextTemplateTableSync::COL_IS_FOLDER("is_folder");
		const string TextTemplateTableSync::COL_PARENT_ID("parent_id");
			

		TextTemplateTableSync::TextTemplateTableSync()
			: SQLiteNoSyncTableSyncTemplate<TextTemplateTableSync,TextTemplate>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_NAME, "TEXT");
			addTableColumn(COL_SHORT_TEXT, "TEXT");
			addTableColumn(COL_LONG_TEXT, "TEXT");
			addTableColumn(COL_LEVEL, "INTEGER");
			addTableColumn(COL_IS_FOLDER, "INTEGER");
			addTableColumn(COL_PARENT_ID, "INTEGER");

			addTableIndex(COL_LEVEL);
			addTableIndex(COL_PARENT_ID);
		}


		vector<shared_ptr<TextTemplate> > TextTemplateTableSync::Search(
			AlarmLevel level
			, uid parentId
			, bool isFolder
			, string name
			, const TextTemplate* templateToBeDifferentWith
			, int first
			, int number
			, bool orderByName
			, bool orderByShortText
			, bool orderByLongText
			, bool raisingOrder
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE "

			// Filtering
				<< "is_folder=" << Conversion::ToString(isFolder)
			;
			if (level != ALARM_LEVEL_UNKNOWN)
				query << " AND " << COL_LEVEL << "=" << Conversion::ToString(static_cast<int>(level));
			if (parentId != static_cast<uid>(UNKNOWN_VALUE))
				query << " AND " << COL_PARENT_ID << "=" << parentId;
			if (!name.empty())
				query << " AND " << COL_NAME << "=" << Conversion::ToSQLiteString(name);
			if (templateToBeDifferentWith)
				query << " AND " << TABLE_COL_ID << "!=" << Conversion::ToString(templateToBeDifferentWith->getKey());
			
			// Ordering
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByShortText)
				query << " ORDER BY " << COL_SHORT_TEXT << (raisingOrder ? " ASC" : " DESC");
			if (orderByLongText)
				query << " ORDER BY " << COL_LONG_TEXT << (raisingOrder ? " ASC" : " DESC");
			
			// Size
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				vector<shared_ptr<TextTemplate> > objects;
				while (rows->next ())
				{
					shared_ptr<TextTemplate> object(new TextTemplate());
					load(object.get(), rows);
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
