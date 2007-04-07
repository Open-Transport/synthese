
/** CreateDisplayScreenAction class implementation.
	@file CreateDisplayScreenAction.cpp

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

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "34_departures_table/CreateDisplayScreenAction.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DeparturesTableModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace departurestable
	{
		const std::string CreateDisplayScreenAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "pti";

		ParametersMap CreateDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_TEMPLATE_ID, _template ? Conversion::ToString(_template->getKey()) : "0"));
			return map;
		}

		void CreateDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			it = map.find(PARAMETER_TEMPLATE_ID);
			if (it != map.end())
			{
				if (!DeparturesTableModule::getDisplayScreens().contains(Conversion::ToLongLong(it->second)))
					throw ActionException("Specified template not found");
				_template = DeparturesTableModule::getDisplayScreens().get(Conversion::ToLongLong(it->second));
			}

			_request->setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void CreateDisplayScreenAction::run()
		{
			DisplayScreen* screen = new DisplayScreen;
			if (_template)
				screen->copy(_template);
			DisplayScreenTableSync::save(screen);

			_request->setObjectId(screen->getKey());

			delete screen;
		}

		CreateDisplayScreenAction::CreateDisplayScreenAction()
			: Action()
			, _template(NULL)
		{

		}
	}
}
