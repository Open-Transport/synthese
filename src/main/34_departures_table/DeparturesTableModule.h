
/** DeparturesTableModule class header.
	@file DeparturesTableModule.h

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

#ifndef SYNTHESE_DeparturesTableModule_H__
#define SYNTHESE_DeparturesTableModule_H__

#include "01_util/ModuleClass.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/DisplayScreen.h"

/** @defgroup m34 Arrival/departures tables service module.

<table><tr><th>Content</th><th>Object classes</th><th>Hot load?</th><th>SQLite Table Sync</th><th>Admin screen</th></tr>
<tr><td>Screen intended to display arrival/departures table</td><td>DisplayScreen</td><td>YES</td><td>DisplayScreenTableSync</td><td>DisplayAdmin</td></tr>
<tr><td>Point belonging to a place where a display screen can be plugged</td><td>BroadcastPoint</td><td>YES</td><td>BroadcastPointTableSync</td><td>BroadcastPointAdmin</td></tr>
</table>

@{
*/

namespace synthese
{
	namespace env
	{
		class LineStop;
		class ConnectionPlace;
		class PhysicalStop;
		class Line;
	}
	namespace departurestable
	{
		class DeparturesTableModule : public util::ModuleClass
		{
			static DisplayType::Registry	_displayTypes;
			static BroadcastPoint::Registry	_broadcastPoints;
			static DisplayScreen::Registry	_displayScreens;

		public:
			static DisplayType::Registry& getDisplayTypes();
			static BroadcastPoint::Registry& getBroadcastPoints();
			static DisplayScreen::Registry& getDisplayScreens();
		};
	}
}

/** @} */

#endif // SYNTHESE_DeparturesTableModule_H__
