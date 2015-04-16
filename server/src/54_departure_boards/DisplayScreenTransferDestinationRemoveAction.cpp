
/** DisplayScreenTransferDestinationRemoveAction class implementation.
	@file DisplayScreenTransferDestinationRemoveAction.cpp
	@author Hugues
	@date 2009

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

#include "DisplayScreenTransferDestinationRemoveAction.h"

#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "User.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "ArrivalDepartureTableLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace pt;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, departure_boards::DisplayScreenTransferDestinationRemoveAction>::FACTORY_KEY("DisplayScreenTransferDestinationRemoveAction");
	}

	namespace departure_boards
	{
		const string DisplayScreenTransferDestinationRemoveAction::PARAMETER_DISPLAY_SCREEN_ID = Action_PARAMETER_PREFIX + "ds";
		const string DisplayScreenTransferDestinationRemoveAction::PARAMETER_TRANSFER_PLACE_ID = Action_PARAMETER_PREFIX + "tp";
		const string DisplayScreenTransferDestinationRemoveAction::PARAMETER_DESTINATION_PLACE_ID = Action_PARAMETER_PREFIX + "dp";


		ParametersMap DisplayScreenTransferDestinationRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_screen.get())
				map.insert(PARAMETER_DISPLAY_SCREEN_ID, _screen->getKey());
			if(_transferPlace.get())
				map.insert(PARAMETER_TRANSFER_PLACE_ID, _transferPlace->getKey());
			if(_destinationPlace.get())
			{
				map.insert(PARAMETER_DESTINATION_PLACE_ID, _destinationPlace->getKey());
			}
			return map;
		}



		void DisplayScreenTransferDestinationRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_DISPLAY_SCREEN_ID), *_env);
				_transferPlace = StopAreaTableSync::Get(map.get<RegistryKeyType>(PARAMETER_TRANSFER_PLACE_ID), *_env);
				_destinationPlace = StopAreaTableSync::Get(map.get<RegistryKeyType>(PARAMETER_DESTINATION_PLACE_ID), *_env);
			}
			catch(ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Display screen not found");
			}
			catch(ObjectNotFoundException<StopArea>&)
			{
				throw ActionException("Transfer place not found");
			}
		}



		void DisplayScreenTransferDestinationRemoveAction::run(Request& request)
		{
			// Action
			_screen->removeTransferDestination(
				_transferPlace.get(),
				_destinationPlace.get()
			);

			// Save
			DisplayScreenTableSync::Save(_screen.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				"Retrait de destination avec correspondance : "+ _destinationPlace->getFullName() + " à "+ _transferPlace->getFullName(),
				*request.getUser()
			);
		}



		bool DisplayScreenTransferDestinationRemoveAction::isAuthorized(const Session* session
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



		void DisplayScreenTransferDestinationRemoveAction::setScreen( boost::shared_ptr<const DisplayScreen> value )
		{
			_screen = const_pointer_cast<DisplayScreen>(value);
		}



		void DisplayScreenTransferDestinationRemoveAction::setTransferPlace( boost::shared_ptr<const pt::StopArea> value )
		{
			_transferPlace = value;
		}



		void DisplayScreenTransferDestinationRemoveAction::setDestinationPlace( boost::shared_ptr<const pt::StopArea> value )
		{
			_destinationPlace = value;
		}
	}
}
