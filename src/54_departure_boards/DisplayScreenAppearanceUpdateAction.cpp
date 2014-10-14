
/** DisplayScreenAppearanceUpdateAction class implementation.
	@file DisplayScreenAppearanceUpdateAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "DisplayScreenAppearanceUpdateAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "ArrivalDepartureTableRight.h"
#include "Request.h"
#include "DBLogModule.h"
#include "ObjectNotFoundException.h"
#include "ArrivalDepartureTableLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace dblog;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, departure_boards::DisplayScreenAppearanceUpdateAction>::FACTORY_KEY("DisplayScreenAppearanceUpdateAction");
	}

	namespace departure_boards
	{
		const string DisplayScreenAppearanceUpdateAction::PARAMETER_DISPLAY_SCREEN(Action_PARAMETER_PREFIX + "id");
		const string DisplayScreenAppearanceUpdateAction::PARAMETER_BLINKING_DELAY = Action_PARAMETER_PREFIX + "bd";
		const string DisplayScreenAppearanceUpdateAction::PARAMETER_DISPLAY_PLATFORM = Action_PARAMETER_PREFIX + "dp";
		const string DisplayScreenAppearanceUpdateAction::PARAMETER_DISPLAY_SERVICE_NUMBER = Action_PARAMETER_PREFIX + "ds";
		const string DisplayScreenAppearanceUpdateAction::PARAMETER_DISPLAY_CLOCK(Action_PARAMETER_PREFIX + "dc");
		const string DisplayScreenAppearanceUpdateAction::PARAMETER_TITLE = Action_PARAMETER_PREFIX + "tt";
		const string DisplayScreenAppearanceUpdateAction::PARAMETER_DISPLAY_TEAM(Action_PARAMETER_PREFIX + "dt");


		DisplayScreenAppearanceUpdateAction::DisplayScreenAppearanceUpdateAction()
			: util::FactorableTemplate<Action, DisplayScreenAppearanceUpdateAction>()
		{
		}



		ParametersMap DisplayScreenAppearanceUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if (_screen.get() != NULL) map.insert(PARAMETER_DISPLAY_SCREEN, _screen->getKey());
			return map;
		}



		void DisplayScreenAppearanceUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				setScreenId(map.get<RegistryKeyType>(PARAMETER_DISPLAY_SCREEN));
				_title = map.get<string>(PARAMETER_TITLE);
				_blinkingDelay = map.get<int>(PARAMETER_BLINKING_DELAY);
				_displayPlatform = map.getDefault<bool>(PARAMETER_DISPLAY_PLATFORM, true);
				_displayServiceNumber = map.getDefault<bool>(PARAMETER_DISPLAY_SERVICE_NUMBER, false);
				_displayTeam = map.getDefault<bool>(PARAMETER_DISPLAY_TEAM, true);
				_displayClock = map.getDefault<bool>(PARAMETER_DISPLAY_CLOCK, false);
			}
			catch(ParametersMap::MissingParameterException& e)
			{
				throw ActionException(e, *this);
			}
		}



		void DisplayScreenAppearanceUpdateAction::run(Request& request)
		{
			// Log
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Délai de clignotement", _screen->get<BlinkingDelay>(), _blinkingDelay);
			DBLogModule::appendToLogIfChange(log, "Affichage du numéro de quai", _screen->get<TrackNumberDisplay>(), _displayPlatform);
			DBLogModule::appendToLogIfChange(log, "Affichage du numéro de service", _screen->get<ServiceNumberDisplay>(), _displayServiceNumber);
			DBLogModule::appendToLogIfChange(log, "Affichage du numéro d'équipe", _screen->get<DisplayTeam>(), _displayTeam);
			DBLogModule::appendToLogIfChange(log, "Affichage de l'horloge", _screen->get<DisplayClock>(), _displayClock);
			DBLogModule::appendToLogIfChange(log, "Titre", _screen->get<Title>(), _title);

			// Preparation
			_screen->set<Title>(_title);
			_screen->set<DisplayTeam>(_displayTeam);
			_screen->set<BlinkingDelay>(_blinkingDelay);
			_screen->set<TrackNumberDisplay>(_displayPlatform);
			_screen->set<ServiceNumberDisplay>(_displayServiceNumber);
			_screen->set<DisplayClock>(_displayClock);

			// The action
			DisplayScreenTableSync::Save(_screen.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(*_screen, log.str(), *request.getUser());
		}



		void DisplayScreenAppearanceUpdateAction::setScreenId(
			util::RegistryKeyType id
		){
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(id, *_env);
			}
			catch (ObjectNotFoundException<DisplayScreen>& e)
			{
				throw ActionException("Specified display screen "+ lexical_cast<string>(id) +" not found in "+ FACTORY_KEY + e.getMessage());
			}
		}



		bool DisplayScreenAppearanceUpdateAction::isAuthorized(const Session* session
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
	}
}
