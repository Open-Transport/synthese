
/** UpdateDisplayPreselectionParametersAction class implementation.
	@file UpdateDisplayPreselectionParametersAction.cpp

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

#include "34_departures_table/UpdateDisplayPreselectionParametersAction.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/DeparturesTableModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace departurestable
	{
		const std::string UpdateDisplayPreselectionParametersAction::PARAMETER_ACTIVATE_PRESELECTION = Action_PARAMETER_PREFIX + "ap";
		const std::string UpdateDisplayPreselectionParametersAction::PARAMETER_PRESELECTION_DELAY = Action_PARAMETER_PREFIX + "pd";


		ParametersMap UpdateDisplayPreselectionParametersAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateDisplayPreselectionParametersAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				_screen = DeparturesTableModule::getDisplayScreens().get(_request->getObjectId());

				it = map.find(PARAMETER_ACTIVATE_PRESELECTION);
				if (it == map.end())
					throw ActionException("Preselection activation not specified");
				_activatePreselection = Conversion::ToBool(it->second);

				it = map.find(PARAMETER_PRESELECTION_DELAY);
				if (it == map.end())
					throw ActionException("Preselection delay not specified");
				_preselectionDelay = Conversion::ToInt(it->second);
			}
			catch (DisplayScreen::RegistryKeyException e)
			{
				throw ActionException("Display screen not found");
			}
		}

		UpdateDisplayPreselectionParametersAction::UpdateDisplayPreselectionParametersAction()
			: Action()
		{}

		void UpdateDisplayPreselectionParametersAction::run()
		{
			_screen->setDestinationForceDelay(_preselectionDelay);
			_screen->setGenerationMethod(_activatePreselection ? DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD : DisplayScreen::STANDARD_METHOD);
			DisplayScreenTableSync::save(_screen);
		}
	}
}