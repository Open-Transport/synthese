
/** UpdateAllStopsDisplayScreenAction class implementation.
	@file UpdateAllStopsDisplayScreenAction.cpp

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

#include "UpdateAllStopsDisplayScreenAction.h"
#include "ArrivalDepartureTableLog.h"
#include "DisplayScreenTableSync.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "DBLogModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace dblog;

	template<> const string util::FactorableTemplate<Action, departurestable::UpdateAllStopsDisplayScreenAction>::FACTORY_KEY("uasdsa");

	namespace departurestable
	{
		const string UpdateAllStopsDisplayScreenAction::PARAMETER_SCREEN_ID = Action_PARAMETER_PREFIX + "s";
		const string UpdateAllStopsDisplayScreenAction::PARAMETER_VALUE = Action_PARAMETER_PREFIX + "val";


		ParametersMap UpdateAllStopsDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			if(_screen.get()) map.insert(PARAMETER_SCREEN_ID, _screen->getKey());
			return map;
		}

		void UpdateAllStopsDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_SCREEN_ID),
					*_env
				);

				_value = map.getBool(PARAMETER_VALUE, true, false, FACTORY_KEY);

			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Display screen not found");
			}
		}

		void UpdateAllStopsDisplayScreenAction::run(Request& request)
		{
			// Comparison for log text generation
			stringstream log;
			DBLogModule::appendToLogIfChange(
				log,
				"Affichage tous arrêts",
				_screen->getAllPhysicalStopsDisplayed() ? "OUI" : "NON",
				_value ? "OUI" : "NON"
			);
			
			_screen->setAllPhysicalStopsDisplayed(_value);
			DisplayScreenTableSync::Save(_screen.get());
			
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				log.str(),
				*request.getUser()
			);
		}



		bool UpdateAllStopsDisplayScreenAction::isAuthorized(const Profile& profile
		) const {
			assert(_screen.get() != NULL);
			if (_screen->getLocalization() != NULL)
			{
				return profile.isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_screen->getLocalization()->getKey()));
			}
			else
			{
				return profile.isAuthorized<ArrivalDepartureTableRight>(WRITE);
			}
		}
		
		
		void UpdateAllStopsDisplayScreenAction::setScreen(
			boost::shared_ptr<const DisplayScreen> value
		){
			_screen = const_pointer_cast<DisplayScreen>(value);
		}
	}
}
