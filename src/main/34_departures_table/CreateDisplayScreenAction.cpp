
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

#include "34_departures_table/CreateDisplayScreenAction.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/ConnectionPlaceTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace env;
	
	namespace departurestable
	{
		const std::string CreateDisplayScreenAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "pti";
		const string CreateDisplayScreenAction::PARAMETER_LOCALIZATION_ID(Action_PARAMETER_PREFIX + "pli");

		ParametersMap CreateDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_TEMPLATE_ID, _template ? Conversion::ToString(_template->getKey()) : "0"));
			map.insert(make_pair(PARAMETER_LOCALIZATION_ID, Conversion::ToString(_place.get() ? _place->getKey() : UNKNOWN_VALUE)));
			return map;
		}

		void CreateDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			it = map.find(PARAMETER_TEMPLATE_ID);
			if (it != map.end() && Conversion::ToLongLong(it->second))
			{
				if (!DeparturesTableModule::getDisplayScreens().contains(Conversion::ToLongLong(it->second)))
					throw ActionException("Specified template not found");
				_template = DeparturesTableModule::getDisplayScreens().get(Conversion::ToLongLong(it->second));
			}

			it = map.find(PARAMETER_LOCALIZATION_ID);
			if (it == map.end())
				throw ActionException("Localization not specified");
			try
			{
				_place = ConnectionPlaceTableSync::get(Conversion::ToLongLong(it->second));
			}
			catch (...)
			{
				throw ActionException("Specified localization not found");
			}

			_request->setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void CreateDisplayScreenAction::run()
		{
			// Preparation
			shared_ptr<DisplayScreen> screen(new DisplayScreen);
			if (_template)
				screen->copy(_template);
			screen->setLocalization(_place);
			screen->setMaintenanceIsOnline(true);

			// Action
			DisplayScreenTableSync::save(screen.get());

			// Request update
			_request->setObjectId(screen->getKey());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(screen, "Création", _request->getUser());
		}

		void CreateDisplayScreenAction::setPlace( boost::shared_ptr<const env::ConnectionPlace> place )
		{
			_place = place;
		}
	}
}
