
/** DisplayScreenRemoveDisplayedPlaceAction class implementation.
	@file DisplayScreenRemoveDisplayedPlaceAction.cpp

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

#include "DisplayScreenRemoveDisplayedPlaceAction.h"
#include "DisplayScreenTableSync.h"
#include "ConnectionPlaceTableSync.h"
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
	using namespace env;
	using namespace util;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, departurestable::DisplayScreenRemoveDisplayedPlaceAction>::FACTORY_KEY("dsrdp");
	
	namespace departurestable
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
			setScreen(map.getUid(PARAMETER_SCREEN, true, FACTORY_KEY));
			setPlace(map.getUid(PARAMETER_PLACE, true, FACTORY_KEY));
		}



		void DisplayScreenRemoveDisplayedPlaceAction::run()
		{
			_screen->removeDisplayedPlace(_place.get());
			DisplayScreenTableSync::Save(_screen.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				"Retrait de l'arrêt de sélection "+ _place->getFullName(),
				*_request->getUser()
			);
		}



		bool DisplayScreenRemoveDisplayedPlaceAction::_isAuthorized(
		) const {
			return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE);
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
				_place = ConnectionPlaceTableSync::Get(id, *_env);

			}
			catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>&)
			{
				throw ActionException("Specified place not found");
			}
		}
	}
}
