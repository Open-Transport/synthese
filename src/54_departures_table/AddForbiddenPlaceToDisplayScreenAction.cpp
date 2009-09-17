
/** AddForbiddenPlaceToDisplayScreenAction class implementation.
	@file AddForbiddenPlaceToDisplayScreenAction.cpp

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

#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "ArrivalDepartureTableRight.h"
#include "ArrivalDepartureTableLog.h"
#include "AddForbiddenPlaceToDisplayScreenAction.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "ConnectionPlaceTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace env;
	using namespace db;
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<Action, departurestable::AddForbiddenPlaceToDisplayScreenAction>::FACTORY_KEY("afptdsa");
	}
	
	namespace departurestable
	{
		const string AddForbiddenPlaceToDisplayScreenAction::PARAMETER_SCREEN_ID(
			Action_PARAMETER_PREFIX + "s"
		);
		const string AddForbiddenPlaceToDisplayScreenAction::PARAMETER_PLACE = Action_PARAMETER_PREFIX + "p";


		ParametersMap AddForbiddenPlaceToDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			if(_screen.get()) map.insert(PARAMETER_SCREEN_ID, _screen->getKey());
			return map;
		}

		void AddForbiddenPlaceToDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_SCREEN_ID),
					*_env
				);
				_place = ConnectionPlaceTableSync::Get(
					map.get<RegistryKeyType>(PARAMETER_PLACE),
					*_env
				);
			}
			catch (...)
			{
				throw ActionException("Display screen not found");
			}
		}

		void AddForbiddenPlaceToDisplayScreenAction::run()
		{
			_screen->addForbiddenPlace(_place.get());
			DisplayScreenTableSync::Save(_screen.get());
			
			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				"Ajout de l'arrêt à ne pas desservir "+ _place->getFullName(),
				*_request->getUser()
			);
		}



		bool AddForbiddenPlaceToDisplayScreenAction::_isAuthorized(
		) const {
			assert(_screen.get() != NULL);
			if (_screen->getLocalization() != NULL)
			{
				return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_screen->getLocalization()->getKey()));
			}
			else
			{
				return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE);
			}
		}
		
		
		void AddForbiddenPlaceToDisplayScreenAction::setScreen(boost::shared_ptr<DisplayScreen> value)
		{
			_screen = value;
		}
	
	
		void AddForbiddenPlaceToDisplayScreenAction::setScreen(
			boost::shared_ptr<const DisplayScreen> value)
		{
			_screen = const_pointer_cast<DisplayScreen>(value);
		}
	}
}
