
//////////////////////////////////////////////////////////////////////////
///	SettingsTableSync class implementation.
///	@file SettingsTableSync.cpp
///	@author Hugues
///	@date 2010
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

#include "SettingsTableSync.h"

#include "ReplaceQuery.h"
#include "SelectQuery.hpp"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace settings;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,SettingsTableSync>::FACTORY_KEY("63 Settings");
		template<> const string Registry<settings::SettingsValue>::KEY("Setting");
	}


	namespace settings
	{
		const string SettingsTableSync::COL_NAME ("name");
		const string SettingsTableSync::COL_MODULE ("module");
		const string SettingsTableSync::COL_VALUE ("value");


		std::string SettingsValue::ToString()
		{
			return module + "." + name + " = " + value;
		}

		SettingsValue::SettingsValue(RegistryKeyType key)
			: Registrable(key)
		{
		}
	}


	namespace db
	{

		template<> const DBTableSync::Format DBTableSyncTemplate<SettingsTableSync>::TABLE(
			"t120_settings"
		);


		template<> const Field DBTableSyncTemplate<SettingsTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(SettingsTableSync::COL_NAME, SQL_TEXT),
			Field(SettingsTableSync::COL_MODULE, SQL_TEXT),
			Field(SettingsTableSync::COL_VALUE, SQL_TEXT),
			Field()
		};


		template<>
		DBTableSync::Indexes DBTableSyncTemplate<SettingsTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}


		template<> void OldLoadSavePolicy<SettingsTableSync,SettingsValue>::Load(
			SettingsValue* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->name = rows->getText(SettingsTableSync::COL_NAME);
			object->module = rows->getText(SettingsTableSync::COL_MODULE);
			object->value = rows->getText(SettingsTableSync::COL_VALUE);
		}


		template<> void OldLoadSavePolicy<SettingsTableSync,SettingsValue>::Save(
			SettingsValue* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<SettingsTableSync> query(*object);
			query.addField(object->name);
			query.addField(object->module);
			query.addField(object->value);
			query.execute(transaction);
		}


		template<> void OldLoadSavePolicy<SettingsTableSync,SettingsValue>::Unlink(
			SettingsValue* obj
		){
		}

		template<> bool DBTableSyncTemplate<SettingsTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}

		template<> void DBTableSyncTemplate<SettingsTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}


		template<> void DBTableSyncTemplate<SettingsTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}


		template<> void DBTableSyncTemplate<SettingsTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}
	}
}
