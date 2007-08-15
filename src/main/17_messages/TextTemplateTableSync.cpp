
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
#include "02_db/SQLiteQueueThreadExec.h"
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

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<TextTemplate>::TABLE_NAME = "t038_text_templates";
		template<> const int SQLiteTableSyncTemplate<TextTemplate>::TABLE_ID = 38;
		template<> const bool SQLiteTableSyncTemplate<TextTemplate>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<TextTemplate>::load(TextTemplate* object, const db::SQLiteResultSPtr& rows )
		{
			object->setKey(rows->getLongLong (TABLE_COL_ID));
			object->setName(rows->getText (TextTemplateTableSync::COL_NAME));
			object->setShortMessage(rows->getText ( TextTemplateTableSync::COL_SHORT_TEXT));
			object->setLongMessage(rows->getText ( TextTemplateTableSync::COL_LONG_TEXT));
			object->setAlarmLevel((AlarmLevel) rows->getInt ( TextTemplateTableSync::COL_LEVEL));
		}


		template<> void SQLiteTableSyncTemplate<TextTemplate>::save(TextTemplate* object)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE_NAME << " SET "
					<< TextTemplateTableSync::COL_NAME << "=" << Conversion::ToSQLiteString(object->getName())
					<< "," << TextTemplateTableSync::COL_SHORT_TEXT << "=" << Conversion::ToSQLiteString(object->getShortMessage())
					<< "," << TextTemplateTableSync::COL_LONG_TEXT << "=" << Conversion::ToSQLiteString(object->getLongMessage())
					<< "," << TextTemplateTableSync::COL_LEVEL << "=" << Conversion::ToString((int) object->getAlarmLevel())
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId());
                query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					<< "," << Conversion::ToSQLiteString(object->getName())
					<< "," << Conversion::ToSQLiteString(object->getShortMessage())
					<< "," << Conversion::ToSQLiteString(object->getLongMessage())
					<< "," << Conversion::ToString((int) object->getAlarmLevel())
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

	}

	namespace messages
	{
		const std::string TextTemplateTableSync::COL_NAME = "name";
		const std::string TextTemplateTableSync::COL_SHORT_TEXT = "short_text";
		const std::string TextTemplateTableSync::COL_LONG_TEXT = "long_text";
		const std::string TextTemplateTableSync::COL_LEVEL = "level";

		TextTemplateTableSync::TextTemplateTableSync()
			: SQLiteTableSyncTemplate<TextTemplate>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_NAME, "TEXT");
			addTableColumn(COL_SHORT_TEXT, "TEXT");
			addTableColumn(COL_LONG_TEXT, "TEXT");
			addTableColumn(COL_LEVEL, "INTEGER");
		}

		void TextTemplateTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
		}

		void TextTemplateTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
		}

		void TextTemplateTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
		}

		vector<shared_ptr<TextTemplate> > TextTemplateTableSync::search(
			AlarmLevel level
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
				<< COL_LEVEL << "=" << Conversion::ToString((int) level)
				;
			if (!name.empty())
				query << " AND " << COL_NAME << "=" << Conversion::ToSQLiteString(name);
			if (templateToBeDifferentWith)
				query << " AND " << TABLE_COL_ID << "!=" << Conversion::ToString(templateToBeDifferentWith->getKey());
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByShortText)
				query << " ORDER BY " << COL_SHORT_TEXT << (raisingOrder ? " ASC" : " DESC");
			if (orderByLongText)
				query << " ORDER BY " << COL_LONG_TEXT << (raisingOrder ? " ASC" : " DESC");
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
