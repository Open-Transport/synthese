
/** DisplayScreenRemoveDisplayedPlaceAction class implementation.
	@file DisplayScreenRemoveDisplayedPlaceAction.cpp

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

#include "DisplayScreenRemoveDisplayedPlaceAction.h"

#include "DisplayScreenTableSync.h"
#include "Profile.h"
#include "Session.h"
#include "StopAreaTableSync.hpp"
#include "User.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include <boost/lexical_cast.hpp>
#include "ArrivalDepartureTableLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace pt;
	using namespace util;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, departure_boards::DisplayScreenRemoveDisplayedPlaceAction>::FACTORY_KEY("dsrdp");

	namespace departure_boards
	{
		const string DisplayScreenRemoveDisplayedPlaceAction::PARAMETER_PLACE(Action_PARAMETER_PREFIX + "pl");
		const string DisplayScreenRemoveDisplayedPlaceAction::PARAMETER_SCREEN(Action_PARAMETER_PREFIX + "sc");


		ParametersMap DisplayScreenRemoveDisplayedPlaceAction::getParametersMap() const
		{
			ParametersMap map;
			if(_screen.get())
			{
				map.insert(PARAMETER_SCREEN, lexical_cast<string>(_screen->getKey()));
			}
			if(_place.get())
			{
				map.insert(PARAMETER_PLACE, lexical_cast<string>(_place->getKey()));
			}
			return map;
		}



		void DisplayScreenRemoveDisplayedPlaceAction::_setFromParametersMap(const ParametersMap& map)
		{
			setScreen(map.get<RegistryKeyType>(PARAMETER_SCREEN));
			setPlace(map.get<RegistryKeyType>(PARAMETER_PLACE));
		}



		void DisplayScreenRemoveDisplayedPlaceAction::run(Request& request)
		{
			_screen->removeDisplayedPlace(_place.get());
			DisplayScreenTableSync::Save(_screen.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				"Retrait de l'arrêt de sélection "+ _place->getFullName(),
				*request.getUser()
			);
		}



		bool DisplayScreenRemoveDisplayedPlaceAction::isAuthorized(const Session* session
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

		void DisplayScreenRemoveDisplayedPlaceAction::setScreen( const util::RegistryKeyType id )
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(id, *_env);
			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Display screen not found");
			}
		}

		void DisplayScreenRemoveDisplayedPlaceAction::setPlace( const util::RegistryKeyType id )
		{
			try
			{
				_place = StopAreaTableSync::Get(id, *_env);

			}
			catch (ObjectNotFoundException<StopArea>&)
			{
				throw ActionException("Specified place not found");
			}
		}
	}
}
