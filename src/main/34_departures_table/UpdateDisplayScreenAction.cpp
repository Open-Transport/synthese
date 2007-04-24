
/** UpdateDisplayScreenAction class implementation.
	@file UpdateDisplayScreenAction.cpp

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

#include "01_util/Conversion.h"

#include "02_db/DBEmptyResultException.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/PhysicalStop.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "34_departures_table/UpdateDisplayScreenAction.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/DisplayScreenTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace db;
	
	namespace departurestable
	{
		const std::string UpdateDisplayScreenAction::PARAMETER_WIRING_CODE = Action_PARAMETER_PREFIX + "wc";
		const std::string UpdateDisplayScreenAction::PARAMETER_BLINKING_DELAY = Action_PARAMETER_PREFIX + "bd";
		const std::string UpdateDisplayScreenAction::PARAMETER_CLEANING_DELAY = Action_PARAMETER_PREFIX + "cd";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_PLATFORM = Action_PARAMETER_PREFIX + "dp";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_SERVICE_NUMBER = Action_PARAMETER_PREFIX + "ds";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_DEPARTURE_ARRIVAL = Action_PARAMETER_PREFIX + "da";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_END_FILTER = Action_PARAMETER_PREFIX + "ef";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_MAX_DELAY = Action_PARAMETER_PREFIX + "md";
		const std::string UpdateDisplayScreenAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "ty";
		const std::string UpdateDisplayScreenAction::PARAMETER_TITLE = Action_PARAMETER_PREFIX + "tt";



		ParametersMap UpdateDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::get(_request->getObjectId());

				ParametersMap::const_iterator it;
				
				it= map.find(PARAMETER_TITLE);
				if (it == map.end())
					throw ActionException("Title not specified");
				_title = it->second;

				it = map.find(PARAMETER_WIRING_CODE);
				if (it != map.end())
					_wiringCode = Conversion::ToInt(it->second);

				it = map.find(PARAMETER_BLINKING_DELAY);
				if (it != map.end())
					_blinkingDelay = Conversion::ToInt(it->second);

				it = map.find(PARAMETER_CLEANING_DELAY);
				if (it != map.end())
					_cleaningDelay = Conversion::ToInt(it->second);

				it = map.find(PARAMETER_DISPLAY_MAX_DELAY);
				if (it != map.end())
					_maxDelay = Conversion::ToInt(it->second);

				it = map.find(PARAMETER_DISPLAY_PLATFORM);
				if (it != map.end())
					_displayPlatform = Conversion::ToBool(it->second);

				it = map.find(PARAMETER_DISPLAY_SERVICE_NUMBER);
				if (it != map.end())
					_displayServiceNumber = Conversion::ToBool(it->second);

				it = map.find(PARAMETER_DISPLAY_DEPARTURE_ARRIVAL);
				if (it != map.end())
					_direction = (DeparturesTableDirection) Conversion::ToInt(it->second);

				it = map.find(PARAMETER_DISPLAY_END_FILTER);
				if (it != map.end())
					_endFilter = (EndFilter) Conversion::ToInt(it->second);

				it = map.find(PARAMETER_TYPE);
				if (it != map.end())
					_type = DeparturesTableModule::getDisplayTypes().get(Conversion::ToLongLong(it->second));

			}
			catch (DBEmptyResultException<DisplayScreen>)
			{
				throw ActionException("Display screen not specified or specified display screen not found");
			}
			catch (BroadcastPoint::RegistryKeyException)
			{
				throw ActionException("Specified localization not found");
			}
			catch (DisplayType::RegistryKeyException)
			{
				throw ActionException("Specified display type not found");
			}
		}

		void UpdateDisplayScreenAction::run()
		{
			_screen->setWiringCode(_wiringCode);
			_screen->setBlinkingDelay(_blinkingDelay);
			_screen->setTrackNumberDisplay(_displayPlatform);
			_screen->setServiceNumberDisplay(_displayServiceNumber);
			_screen->setDirection(_direction);
			_screen->setOriginsOnly(_endFilter);
			_screen->setClearingDelay(_cleaningDelay);
			_screen->setMaxDelay(_maxDelay);
			_screen->setType(_type.get());
			_screen->setTitle(_title);

			DisplayScreenTableSync::save(_screen.get());
		}
	}
}
