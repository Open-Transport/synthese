
/** RemovePreselectionPlaceFromDisplayScreenAction class implementation.
	@file RemovePreselectionPlaceFromDisplayScreenAction.cpp

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

#include "RemovePreselectionPlaceFromDisplayScreenAction.h"
#include "DisplayScreenTableSync.h"
#include "ConnectionPlaceTableSync.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "ArrivalDepartureTableLog.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace env;
	using namespace util;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, departurestable::RemovePreselectionPlaceFromDisplayScreenAction>::FACTORY_KEY("rmpsfds");

	namespace departurestable
	{
		const string RemovePreselectionPlaceFromDisplayScreenAction::PARAMETER_SCREEN_ID = Action_PARAMETER_PREFIX + "s";
		const string RemovePreselectionPlaceFromDisplayScreenAction::PARAMETER_PLACE = Action_PARAMETER_PREFIX + "pla";


		ParametersMap RemovePreselectionPlaceFromDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			if(_screen.get()) map.insert(PARAMETER_SCREEN_ID, _screen->getKey());
			return map;
		}

		void RemovePreselectionPlaceFromDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_SCREEN_ID),
					*_env
				);

				uid id(map.getUid(PARAMETER_PLACE, true, FACTORY_KEY));
				_place = ConnectionPlaceTableSync::Get(id, *_env);

			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Display screen not found");
			}
			catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>&)
			{
				throw ActionException("Specified place not found");
			}
		}

		void RemovePreselectionPlaceFromDisplayScreenAction::run(Request& request)
		{
			_screen->removeForcedDestination(_place.get());
			DisplayScreenTableSync::Save(_screen.get());
			
			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				"Retrait de l'arrêt de présélection "+  _place->getFullName(),
				*request.getUser()
			);
		}



		bool RemovePreselectionPlaceFromDisplayScreenAction::isAuthorized(const Session* session
		) const {
			assert(_screen.get() != NULL);
			if (_screen->getLocalization() != NULL)
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_screen->getLocalization()->getKey()));
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE);
			}
		}
		
		
		void RemovePreselectionPlaceFromDisplayScreenAction::setScreen(
			boost::shared_ptr<const DisplayScreen> value
		){
			_screen = const_pointer_cast<DisplayScreen>(value);
		}
	}
}
