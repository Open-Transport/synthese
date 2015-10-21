////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenRemovePhysicalStopAction class implementation.
///	@file DisplayScreenRemovePhysicalStopAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DisplayScreenRemovePhysicalStopAction.h"

#include "Profile.h"
#include "Session.h"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "User.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "PTUseRule.h"
#include "ArrivalDepartureTableRight.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "ArrivalDepartureTableLog.h"
#include "Conversion.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace pt;
	using namespace util;
	using namespace security;
	using namespace pt;

	template<> const string util::FactorableTemplate<Action, departure_boards::DisplayScreenRemovePhysicalStopAction>::FACTORY_KEY("dsrps");

	namespace departure_boards
	{
		const string DisplayScreenRemovePhysicalStopAction::PARAMETER_SCREEN_ID(
			Action_PARAMETER_PREFIX + "s"
		);
		const string DisplayScreenRemovePhysicalStopAction::PARAMETER_PHYSICAL = Action_PARAMETER_PREFIX + "phy";


		ParametersMap DisplayScreenRemovePhysicalStopAction::getParametersMap() const
		{
			ParametersMap map;
			if (_stop.get())
			{
				map.insert(PARAMETER_PHYSICAL, _stop->getKey());
			}
			if(_screen.get()) map.insert(PARAMETER_SCREEN_ID, _screen->getKey());
			return map;
		}



		void DisplayScreenRemovePhysicalStopAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_SCREEN_ID)
					, *_env
				);

				setStopId(map.get<RegistryKeyType>(PARAMETER_PHYSICAL));
			}
			catch (ObjectNotFoundException<DisplayScreen>& e)
			{
				throw ActionException("Display screen", e, *this);
			}
		}

		void DisplayScreenRemovePhysicalStopAction::run(Request& request)
		{
			// Preparation
			ArrivalDepartureTableGenerator::PhysicalStops stops(_screen->getPhysicalStops(false));
			stops.erase(_stop->getKey());
			_screen->setStops(stops);

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				"Retrait de l'arrêt de départ "+ _stop->getCodeBySources() +"/"+ _stop->getName(),
				*request.getUser()
			);

			DisplayScreenTableSync::Save(_screen.get());
		}



		bool DisplayScreenRemovePhysicalStopAction::isAuthorized(const Session* session
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



		void DisplayScreenRemovePhysicalStopAction::setStopId(
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


		void DisplayScreenRemovePhysicalStopAction::setScreen(
			boost::shared_ptr<const DisplayScreen> value
		){
			_screen = const_pointer_cast<DisplayScreen>(value);
		}
	}
}
