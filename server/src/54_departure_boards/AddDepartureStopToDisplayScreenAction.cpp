
/** AddDepartureStopToDisplayScreenAction class implementation.
	@file AddDepartureStopToDisplayScreenAction.cpp

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

#include "AddDepartureStopToDisplayScreenAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTUseRule.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "ArrivalDepartureTableRight.h"
#include "DisplayScreenTableSync.h"
#include "StopPointTableSync.hpp"
#include "ArrivalDepartureTableLog.h"
#include "StopPoint.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace pt;
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<Action, departure_boards::AddDepartureStopToDisplayScreenAction>::FACTORY_KEY("adstdsa");
	}

	namespace departure_boards
	{
		const string AddDepartureStopToDisplayScreenAction::PARAMETER_SCREEN_ID = Action_PARAMETER_PREFIX + "sc";
		const string AddDepartureStopToDisplayScreenAction::PARAMETER_STOP = Action_PARAMETER_PREFIX + "sto";


		ParametersMap AddDepartureStopToDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			if(_stop.get())
			{
				map.insert(PARAMETER_STOP, _stop->getKey());
			}
			if(_screen.get()) map.insert(PARAMETER_SCREEN_ID, _screen->getKey());
			return map;
		}

		void AddDepartureStopToDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_SCREEN_ID),
					*_env
				);
				setStopId(map.get<RegistryKeyType>(PARAMETER_STOP));
			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Display screen not found");
			}
		}



		void AddDepartureStopToDisplayScreenAction::run(Request& request)
		{
			// Preparation
			ArrivalDepartureTableGenerator::PhysicalStops stops(_screen->getPhysicalStops(false));
			stops[_stop->getKey()] = _stop.get();
			_screen->setStops(stops);

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				"Ajout de l'arrêt de départ "+ _stop->getCodeBySources() +"/"+ _stop->getName(),
				*request.getUser()
			);

			// Action
			DisplayScreenTableSync::Save(_screen.get());
		}



		bool AddDepartureStopToDisplayScreenAction::isAuthorized(const Session* session
		) const {
			assert(_screen.get() != NULL);
			if (_screen->getLocation() != NULL)
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_screen->getLocation()->getKey()));
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE);
			}
		}

		void AddDepartureStopToDisplayScreenAction::setStopId(
			RegistryKeyType id
		){
			try
			{
				_stop = StopPointTableSync::Get(id, *_env, UP_LINKS_LOAD_LEVEL);
			}
			catch (ObjectNotFoundException<StopPoint>& e)
			{
				throw ActionException("Departure physical stop", e, *this);
			}
		}

		void AddDepartureStopToDisplayScreenAction::setScreen(
			boost::shared_ptr<DisplayScreen> screen
		){
			_screen = screen;
		}

		void AddDepartureStopToDisplayScreenAction::setScreen(
			boost::shared_ptr<const DisplayScreen> screen
		){
			_screen = const_pointer_cast<DisplayScreen>(screen);
		}
	}
}
