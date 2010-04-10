
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

#include "Conversion.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "TextTemplate.h"
#include "TextTemplateTableSync.h"

#include "ReplaceQuery.h"

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

	namespace messages
	{
		const string TextTemplateTableSync::COL_NAME = "name";
		const string TextTemplateTableSync::COL_SHORT_TEXT = "short_text";
		const string TextTemplateTableSync::COL_LONG_TEXT = "long_text";
		const string TextTemplateTableSync::COL_IS_FOLDER("is_folder");
		const string TextTemplateTableSync::COL_PARENT_ID("parent_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<TextTemplateTableSync>::TABLE(
			"t038_text_templates"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<TextTemplateTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(TextTemplateTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(TextTemplateTableSync::COL_SHORT_TEXT, SQL_TEXT),
			SQLiteTableSync::Field(TextTemplateTableSync::COL_LONG_TEXT, SQL_TEXT),
			SQLiteTableSync::Field(TextTemplateTableSync::COL_IS_FOLDER, SQL_INTEGER),
			SQLiteTableSync::Field(TextTemplateTableSync::COL_PARENT_ID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<TextTemplateTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(TextTemplateTableSync::COL_PARENT_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<TextTemplateTableSync,TextTemplate>::Load(
			TextTemplate* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText (TextTemplateTableSync::COL_NAME));
			object->setShortMessage(rows->getText ( TextTemplateTableSync::COL_SHORT_TEXT));
			object->setLongMessage(rows->getText ( TextTemplateTableSync::COL_LONG_TEXT));
			object->setIsFolder(rows->getBool(TextTemplateTableSync::COL_IS_FOLDER));
			object->setParentId(rows->getLongLong(TextTemplateTableSync::COL_PARENT_ID));
		}

		template<> void SQLiteDirectTableSyncTemplate<TextTemplateTableSync,TextTemplate>::Unlink(
			TextTemplate* obj
		){
		}

		template<> void SQLiteDirectTableSyncTemplate<TextTemplateTableSync,TextTemplate>::Save(
			TextTemplate* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<TextTemplateTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getShortMessage());
			query.addField(object->getLongMessage());
			query.addField(object->getIsFolder());
			query.addField(object->getParentId());
			query.execute(transaction);
		}

	}

	namespace messages
	{
		TextTemplateTableSync::SearchResult TextTemplateTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> parentId
			, bool isFolder
			, string name
			, const TextTemplate* templateToBeDifferentWith
			, int first
			, boost::optional<std::size_t> number
			, bool orderByName
			, bool orderByShortText
			, bool orderByLongText
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE "

			// Filtering
				<< "is_folder=" << isFolder
			;
			if (parentId)
				query << " AND " << COL_PARENT_ID << "=" << *parentId;
			if (!name.empty())
				query << " AND " << COL_NAME << "=" << Conversion::ToSQLiteString(name);
			if (templateToBeDifferentWith)
				query << " AND " << TABLE_COL_ID << "!=" << templateToBeDifferentWith->getKey();
			
			// Ordering
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByShortText)
				query << " ORDER BY " << COL_SHORT_TEXT << (raisingOrder ? " ASC" : " DESC");
			if (orderByLongText)
				query << " ORDER BY " << COL_LONG_TEXT << (raisingOrder ? " ASC" : " DESC");
			
			// Size
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
