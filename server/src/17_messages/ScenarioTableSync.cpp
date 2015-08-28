
/** ScenarioTableSync class implementation.
	@file ScenarioTableSync.cpp

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

#include "ScenarioTableSync.h"

using namespace std;


namespace synthese
{
	namespace messages
	{
		const string ScenarioTableSync::COL_ENABLED = "is_enabled";
		const string ScenarioTableSync::COL_NAME = "name";
		const string ScenarioTableSync::COL_PERIODSTART = "period_start";
		const string ScenarioTableSync::COL_PERIODEND = "period_end";
		const string ScenarioTableSync::COL_FOLDER_ID("folder_id");
		const string ScenarioTableSync::COL_VARIABLES("variables");
		const string ScenarioTableSync::COL_TEMPLATE("template_id");
		const string ScenarioTableSync::COL_DATASOURCE_LINKS("datasource_links");
		const string ScenarioTableSync::COL_SECTIONS = "messages_section_ids";
		const string ScenarioTableSync::COL_EVENT_START = "event_start";
		const string ScenarioTableSync::COL_EVENT_END = "event_end";
		const string ScenarioTableSync::COL_ARCHIVED = "archived";
		const string ScenarioTableSync::COL_MANUAL_OVERRIDE = "manual_override";

		/*const string ScenarioTableSync::VARIABLES_SEPARATOR = "|";
		const string ScenarioTableSync::VARIABLES_OPERATOR = "&";
		*/
		const string ScenarioTableSync::SECTIONS_SEPARATOR = ",";
		
	}

}
