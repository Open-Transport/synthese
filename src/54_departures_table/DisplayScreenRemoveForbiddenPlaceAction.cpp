
/** DisplayScreenRemoveForbiddenPlaceAction class implementation.
@file DisplayScreenRemoveForbiddenPlaceAction.cpp

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

#include "DisplayScreenRemoveForbiddenPlaceAction.h"
#include "DisplayScreenTableSync.h"
#include "ConnectionPlaceTableSync.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
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
	using namespace dblog;

	template<> const string util::FactorableTemplate<Action, departurestable::DisplayScreenRemoveForbiddenPlaceAction>::FACTORY_KEY("dsrfp");

	namespace departurestable
	{
		const string DisplayScreenRemoveForbiddenPlaceAction::PARAMETER_SCREEN_ID = Action_PARAMETER_PREFIX + "s";
		const string DisplayScreenRemoveForbiddenPlaceAction::PARAMETER_PLACE = Action_PARAMETER_PREFIX + "pla";


		ParametersMap DisplayScreenRemoveForbiddenPlaceAction::getParametersMap() const
		{
			ParametersMap map;
			if(_screen.get()) map.insert(PARAMETER_SCREEN_ID, _screen->getKey());
			return map;
		}

		void DisplayScreenRemoveForbiddenPlaceAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_SCREEN_ID),
					*_env
				);
				_place = ConnectionPlaceTableSync::Get(map.get<RegistryKeyType>(PARAMETER_PLACE), *_env);
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

		void DisplayScreenRemoveForbiddenPlaceAction::run(Request& request)
		{
			_screen->removeForbiddenPlace(_place.get());
			DisplayScreenTableSync::Save(_screen.get());
			
						
			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				"Retrait de l'arrêt à ne pas desservir "+ _place->getFullName(),
				*request.getUser()
			);

		}



		bool DisplayScreenRemoveForbiddenPlaceAction::isAuthorized(const Session* session
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
		
		
		void DisplayScreenRemoveForbiddenPlaceAction::setScreen(
			boost::shared_ptr<const DisplayScreen> value
		){
			_screen = const_pointer_cast<DisplayScreen>(value);
		}
	}
}
