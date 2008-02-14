
/** ScenarioTemplateInheritedTableSync class header.
	@file ScenarioTemplateInheritedTableSync.h

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

#ifndef SYNTHESE_messages_ScenarioTemplateInheritedTableSync_h__
#define SYNTHESE_messages_ScenarioTemplateInheritedTableSync_h__

#include "02_db/SQLiteInheritedNoSyncTableSyncTemplate.h"

#include "17_messages/ScenarioTableSync.h"
#include "17_messages/ScenarioTemplate.h"

namespace synthese
{
	namespace messages
	{
		/** ScenarioTemplateInheritedTableSync class.
			@ingroup m17
		*/
		class ScenarioTemplateInheritedTableSync
			: public db::SQLiteInheritedNoSyncTableSyncTemplate<ScenarioTableSync, ScenarioTemplateInheritedTableSync, ScenarioTemplate>
		{
		public:
			ScenarioTemplateInheritedTableSync();

		};
	}
}

#endif // SYNTHESE_messages_ScenarioTemplateInheritedTableSync_h__
