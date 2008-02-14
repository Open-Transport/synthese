
/** ScenarioTemplateInheritedTableSync class implementation.
	@file ScenarioTemplateInheritedTableSync.cpp

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

#include "ScenarioTemplate.h"
#include "ScenarioTemplateInheritedTableSync.h"

#include "17_messages/ScenarioInheritedTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace messages;

	template<>
	const string util::FactorableTemplate<ScenarioTableSync, ScenarioTemplateInheritedTableSync>::FACTORY_KEY("ScenarioTemplateInheritedTableSync");

	namespace db
	{

		template<>
		void SQLiteInheritedTableSyncTemplate<ScenarioTableSync,ScenarioTemplateInheritedTableSync,ScenarioTemplate>::_Link(ScenarioTemplate* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			LoadScenarioAlarms<ScenarioTemplate>(obj);
		}

		template<>
		void SQLiteInheritedTableSyncTemplate<ScenarioTableSync,ScenarioTemplateInheritedTableSync,ScenarioTemplate>::_Unlink(ScenarioTemplate* obj)
		{

		}
	}

	namespace messages
	{

		ScenarioTemplateInheritedTableSync::ScenarioTemplateInheritedTableSync()
			: SQLiteInheritedNoSyncTableSyncTemplate<ScenarioTableSync, ScenarioTemplateInheritedTableSync, ScenarioTemplate>()
		{

		}
	}
}
