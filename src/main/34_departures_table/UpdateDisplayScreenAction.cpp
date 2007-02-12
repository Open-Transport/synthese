
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

#include "15_env/ConnectionPlace.h"
#include "15_env/PhysicalStop.h"

#include "30_server/ActionException.h"

#include "34_departures_table/UpdateDisplayScreenAction.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/DisplayScreenTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace env;
	
	namespace departurestable
	{
		const std::string UpdateDisplayScreenAction::PARAMETER_LOCALIZATION_ID = Action::PARAMETER_PREFIX + "li";
		const std::string UpdateDisplayScreenAction::PARAMETER_LOCALIZATION_COMMENT = Action::PARAMETER_PREFIX + "lc";
		const std::string UpdateDisplayScreenAction::PARAMETER_WIRING_CODE = Action::PARAMETER_PREFIX + "wc";
		const std::string UpdateDisplayScreenAction::PARAMETER_BLINKING_DELAY = Action::PARAMETER_PREFIX + "bd";
		const std::string UpdateDisplayScreenAction::PARAMETER_CLEANING_DELAY = Action::PARAMETER_PREFIX + "cd";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_PLATFORM = Action::PARAMETER_PREFIX + "dp";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_SERVICE_NUMBER = Action::PARAMETER_PREFIX + "ds";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_DEPARTURE_ARRIVAL = Action::PARAMETER_PREFIX + "da";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_END_FILTER = Action::PARAMETER_PREFIX + "ef";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_MAX_DELAY = Action::PARAMETER_PREFIX + "md";
		const std::string UpdateDisplayScreenAction::PARAMETER_ACTIVATE_PRESELECTION = Action::PARAMETER_PREFIX + "ap";
		const std::string UpdateDisplayScreenAction::PARAMETER_PRESELECTION_DELAY = Action::PARAMETER_PREFIX + "pd";
		const std::string UpdateDisplayScreenAction::PARAMETER_TYPE = Action::PARAMETER_PREFIX + "ty";
		const std::string UpdateDisplayScreenAction::PARAMETER_PHYSICAL = Action::PARAMETER_PREFIX + "ph";
		const std::string UpdateDisplayScreenAction::PARAMETER_ALL_PHYSICALS = Action::PARAMETER_PREFIX + "ah";



		Request::ParametersMap UpdateDisplayScreenAction::getParametersMap() const
		{
			Request::ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateDisplayScreenAction::setFromParametersMap(Request::ParametersMap& map)
		{
			try
			{
				_screen = DeparturesTableModule::getDisplayScreens().get(_request->getObjectId());

				Request::ParametersMap::iterator it = map.find(PARAMETER_LOCALIZATION_COMMENT);
				if (it != map.end())
					_localizationComment = it->second;

				it = map.find(PARAMETER_WIRING_CODE);
				if (it != map.end())
					_wiringCode = Conversion::ToInt(it->second);

				it = map.find(PARAMETER_LOCALIZATION_ID);
				if (it != map.end())
					_localization = DeparturesTableModule::getBroadcastPoints().get(Conversion::ToLongLong(it->second));

				if (_localization != NULL)
				{
					for (vector<const PhysicalStop*>::const_iterator itp = _localization->getConnectionPlace()->getPhysicalStops().begin(); itp != _localization->getConnectionPlace()->getPhysicalStops().end(); ++itp)
					{
						it = map.find(PARAMETER_PHYSICAL + Conversion::ToString((*itp)->getKey()));
						if (it != map.end())
							_physicalStopServe.insert(make_pair(*itp, it->second.size() > 0));
					}
				}

				it = map.find(PARAMETER_ALL_PHYSICALS);
				if (it != map.end())
					_allPhysicals = (it->second.size() > 0);

			}
			catch (DisplayScreen::RegistryKeyException e)
			{
				throw ActionException("Display screen not specified or specified display screen not found");
			}
			catch (BroadcastPoint::RegistryKeyException e)
			{
				throw ActionException("Specified localization not found");
			}

			

	/*		int							_blinkingDelay;
			bool						_displayPlatform;
			bool						_displayServiceNumber;
			DeparturesTableDirection	_direction;
			EndFilter					_endFilter;
			int							_maxDelay;
			bool						_activatePreselection;
			int							_preselectionDelay;
*/
		}

		void UpdateDisplayScreenAction::run()
		{
			_screen->setLocalizationComment(_localizationComment);
			_screen->setWiringCode(_wiringCode);
			if (_localization != NULL)
				_screen->setLocalization(_localization);
			_screen->setAllPhysicalStopsDisplayed(_allPhysicals);
			_screen->clearPhysicalStops();
			for (map<const PhysicalStop*, bool>::const_iterator it = _physicalStopServe.begin(); it != _physicalStopServe.end(); ++it)
				if (it->second)
					_screen->addPhysicalStop(it->first);

			DisplayScreenTableSync::save(_screen);
		}

		UpdateDisplayScreenAction::UpdateDisplayScreenAction()
			: Action()
			, _localization(NULL)
			, _screen(NULL)
		{

		}
	}
}
