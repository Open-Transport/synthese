
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

#include "UpdateDisplayScreenAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	
	namespace departurestable
	{
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
		

		


		Request::ParametersMap UpdateDisplayScreenAction::getParametersMap() const
		{
			Request::ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateDisplayScreenAction::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it = map.find(PARAMETER_LOCALIZATION_COMMENT);
			if (it != map.end())
				_localizationComment = it->second;
			it = map.find(PARAMETER_WIRING_CODE);
			if (it != map.end())
				_wiringCode = Conversion::ToInt(it->second);
	/*		int							_wiringCode;
			int							_blinkingDelay;
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
		}
	}
}