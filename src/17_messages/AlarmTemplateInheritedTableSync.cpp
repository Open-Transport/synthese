
/** AlarmTemplateInheritedTableSync class implementation.
	@file AlarmTemplateInheritedTableSync.cpp

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

#include "AlarmTemplateInheritedTableSync.h"
#include "ReplaceQuery.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace util;

	template<>
	const string util::FactorableTemplate<AlarmTableSync, AlarmTemplateInheritedTableSync>::FACTORY_KEY("AlarmTemplateInheritedTableSync");

	namespace db
	{

		template<>
		void DBInheritedTableSyncTemplate<AlarmTableSync,AlarmTemplateInheritedTableSync,AlarmTemplate>::Load(
			AlarmTemplate* obj,
			const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				obj->setScenario(
					ScenarioTemplateInheritedTableSync::Get(
						rows->getLongLong(AlarmTableSync::COL_SCENARIO_ID),
						env, linkLevel
					).get()
				);
			}
		}

		template<>
		void DBInheritedTableSyncTemplate<AlarmTableSync,AlarmTemplateInheritedTableSync,AlarmTemplate>::Unlink(
			AlarmTemplate* obj
		){
			obj->setScenario(NULL);
		}



		template<>
		void DBInheritedTableSyncTemplate<AlarmTableSync,AlarmTemplateInheritedTableSync,AlarmTemplate>::Save(
			AlarmTemplate* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<AlarmTableSync> query(*object);
			query.addField(1);
			query.addField(0);
			query.addField(object->getLevel());
			query.addField(object->getShortMessage());
			query.addField(object->getLongMessage());
			query.addFieldNull();
			query.addFieldNull();
			query.addField(object->getScenario() ? object->getScenario()->getKey() : RegistryKeyType(0));
			query.addField(0);
			query.addField(object->getRawEditor());
			query.execute(transaction);
		}
	}

	namespace messages
	{
		AlarmTemplateInheritedTableSync::SearchResult AlarmTemplateInheritedTableSync::Search(
			Env& env,
			optional<RegistryKeyType> scenarioId,
			int first /*= 0 */,
			boost::optional<std::size_t> number /*= 0 */,
			bool orderByLevel /*= false */,
			bool raisingOrder /*= false*/,
			LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT a.*"
				<< " FROM " << TABLE.NAME << " AS a "
				<< " WHERE "
				<< COL_IS_TEMPLATE << "=1"
				<< " AND " << COL_SCENARIO_ID << "=" << *scenarioId;
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
