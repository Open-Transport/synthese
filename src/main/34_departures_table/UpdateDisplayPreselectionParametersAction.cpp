
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

#include "UpdateDisplayPreselectionParametersAction.h"

#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

#include "13_dblog/DBLogModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace util;
	using namespace dblog;

	template<> const string util::FactorableTemplate<Action, departurestable::UpdateDisplayPreselectionParametersAction>::FACTORY_KEY("udpp");

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
				_screen = DisplayScreenTableSync::GetUpdateable(_request->getObjectId());
				_activatePreselection =  map.getBool(PARAMETER_ACTIVATE_PRESELECTION, true, false, FACTORY_KEY);
				_preselectionDelay = map.getInt(PARAMETER_PRESELECTION_DELAY, true, FACTORY_KEY);
			}
			catch (DBEmptyResultException<DisplayScreen>&)
			{
				throw ActionException("Display screen not found");
			}
		}

		void UpdateDisplayPreselectionParametersAction::run()
		{
			// Log
			stringstream t;
			DBLogModule::appendToLogIfChange(t, "Mode de présélection", (_screen->getGenerationMethod() == DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD) ? "OUI" : "NON", _activatePreselection ? "OUI" : "NON");
			DBLogModule::appendToLogIfChange(t, "Délai de préselection", _screen->getForceDestinationDelay(), _preselectionDelay);

			// The update
			_screen->setDestinationForceDelay(_preselectionDelay);
			_screen->setGenerationMethod(_activatePreselection ? DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD : DisplayScreen::STANDARD_METHOD);
			DisplayScreenTableSync::save(_screen.get());

		
			ArrivalDepartureTableLog::addUpdateEntry(_screen.get(), t.str(), _request->getUser().get());

		}
	}
}
