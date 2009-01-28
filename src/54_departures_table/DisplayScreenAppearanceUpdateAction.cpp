
/** DisplayScreenAppearanceUpdateAction class implementation.
	@file DisplayScreenAppearanceUpdateAction.cpp
	@author Hugues Romain
	@date 2008

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

// Server
#include "ActionException.h"
#include "ParametersMap.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "DisplayScreenAppearanceUpdateAction.h"
#include "ArrivalDepartureTableRight.h"
#include "Request.h"
#include "RequestMissingParameterException.h"
#include "DBLogModule.h"
#include "ObjectNotFoundException.h"
#include "ArrivalDepartureTableLog.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace dblog;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, departurestable::DisplayScreenAppearanceUpdateAction>::FACTORY_KEY("DisplayScreenAppearanceUpdateAction");
	}

	namespace departurestable
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
				setScreenId(map.getUid(PARAMETER_DISPLAY_SCREEN, true, FACTORY_KEY));
				_title = map.getString(PARAMETER_TITLE, true, FACTORY_KEY);
				_blinkingDelay = map.getInt(PARAMETER_BLINKING_DELAY, true, FACTORY_KEY);
				_displayPlatform = map.getBool(PARAMETER_DISPLAY_PLATFORM, false, true, FACTORY_KEY);
				_displayServiceNumber = map.getBool(PARAMETER_DISPLAY_SERVICE_NUMBER, false, false, FACTORY_KEY);
				_displayTeam = map.getBool(PARAMETER_DISPLAY_TEAM, false, true, FACTORY_KEY);
				_displayClock = map.getBool(PARAMETER_DISPLAY_CLOCK, false, false, FACTORY_KEY);
			}
			catch(RequestMissingParameterException& e)
			{
				throw ActionException(e.getMessage());
			}
		}
		
		
		
		void DisplayScreenAppearanceUpdateAction::run()
		{
			// Log
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Délai de clignotement", _screen->getBlinkingDelay(), _blinkingDelay);
			DBLogModule::appendToLogIfChange(log, "Affichage du numéro de quai", _screen->getTrackNumberDisplay(), _displayPlatform);
			DBLogModule::appendToLogIfChange(log, "Affichage du numéro de service", _screen->getServiceNumberDisplay(), _displayServiceNumber);
			DBLogModule::appendToLogIfChange(log, "Affichage du numéro d'équipe", _screen->getDisplayTeam(), _displayTeam);
			DBLogModule::appendToLogIfChange(log, "Affichage de l'horloge", _screen->getDisplayClock(), _displayClock);
			DBLogModule::appendToLogIfChange(log, "Titre", _screen->getTitle(), _title);

			// Preparation
			_screen->setTitle(_title);
			_screen->setDisplayTeam(_displayTeam);
			_screen->setBlinkingDelay(_blinkingDelay);
			_screen->setTrackNumberDisplay(_displayPlatform);
			_screen->setServiceNumberDisplay(_displayServiceNumber);
			_screen->setDisplayClock(_displayClock);

			// The action
			DisplayScreenTableSync::Save(_screen.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(_screen.get(), log.str(), _request->getUser().get());
		}



		void DisplayScreenAppearanceUpdateAction::setScreenId(
			util::RegistryKeyType id
		){
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(id, _env);
			}
			catch (ObjectNotFoundException<DisplayScreen>& e)
			{
				throw ActionException("Specified display screen "+ Conversion::ToString(id) +" not found in "+ FACTORY_KEY + e.getMessage());
			}
		}



		bool DisplayScreenAppearanceUpdateAction::_isAuthorized(
		) const {
			assert(_screen.get() != NULL);
			return _request->isAuthorized<ArrivalDepartureTableRight>(
				WRITE,
				UNKNOWN_RIGHT_LEVEL,
				Conversion::ToString(_screen->getKey())
			);
		}
	}
}
