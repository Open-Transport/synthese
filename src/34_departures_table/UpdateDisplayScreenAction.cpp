
/** UpdateDisplayScreenAction class implementation.
	@file UpdateDisplayScreenAction.cpp

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

#include "Conversion.h"

#include "PublicTransportStopZoneConnectionPlace.h"
#include "PhysicalStop.h"

#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

#include "UpdateDisplayScreenAction.h"
#include "DisplayScreenTableSync.h"
#include "ArrivalDepartureTableLog.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "DisplayScreen.h"

#include "DBLogModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace db;
	using namespace dblog;

	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departurestable::UpdateDisplayScreenAction>::FACTORY_KEY("updatedisplayscreen");
	}

	namespace departurestable
	{
		const string UpdateDisplayScreenAction::PARAMETER_NAME(Action_PARAMETER_PREFIX + "name");
		const string UpdateDisplayScreenAction::PARAMETER_WIRING_CODE = Action_PARAMETER_PREFIX + "wc";
		const string UpdateDisplayScreenAction::PARAMETER_BLINKING_DELAY = Action_PARAMETER_PREFIX + "bd";
		const string UpdateDisplayScreenAction::PARAMETER_CLEANING_DELAY = Action_PARAMETER_PREFIX + "cd";
		const string UpdateDisplayScreenAction::PARAMETER_DISPLAY_PLATFORM = Action_PARAMETER_PREFIX + "dp";
		const string UpdateDisplayScreenAction::PARAMETER_DISPLAY_SERVICE_NUMBER = Action_PARAMETER_PREFIX + "ds";
		const string UpdateDisplayScreenAction::PARAMETER_DISPLAY_DEPARTURE_ARRIVAL = Action_PARAMETER_PREFIX + "da";
		const string UpdateDisplayScreenAction::PARAMETER_DISPLAY_END_FILTER = Action_PARAMETER_PREFIX + "ef";
		const string UpdateDisplayScreenAction::PARAMETER_DISPLAY_MAX_DELAY = Action_PARAMETER_PREFIX + "md";
		const string UpdateDisplayScreenAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "ty";
		const string UpdateDisplayScreenAction::PARAMETER_TITLE = Action_PARAMETER_PREFIX + "tt";
		const string UpdateDisplayScreenAction::PARAMETER_DISPLAY_TEAM(Action_PARAMETER_PREFIX + "dt");


		ParametersMap UpdateDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void UpdateDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				// The screen
				_screen = DisplayScreenTableSync::GetEditable(_request->getObjectId());

				// Properties
				_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
				_title = map.getString(PARAMETER_TITLE, true, FACTORY_KEY);
				_wiringCode = map.getInt(PARAMETER_WIRING_CODE, true, FACTORY_KEY);
				_blinkingDelay = map.getInt(PARAMETER_BLINKING_DELAY, true, FACTORY_KEY);
				_cleaningDelay = map.getInt(PARAMETER_CLEANING_DELAY, true, FACTORY_KEY);
				_maxDelay = map.getInt(PARAMETER_DISPLAY_MAX_DELAY, true, FACTORY_KEY);
				_displayPlatform = map.getBool(PARAMETER_DISPLAY_PLATFORM, true, true, FACTORY_KEY);
				_displayServiceNumber = map.getBool(PARAMETER_DISPLAY_SERVICE_NUMBER, true, true, FACTORY_KEY);
				_displayTeam = map.getBool(PARAMETER_DISPLAY_TEAM, true, true, FACTORY_KEY);
				_direction = static_cast<DeparturesTableDirection>(map.getInt(PARAMETER_DISPLAY_DEPARTURE_ARRIVAL, true, FACTORY_KEY));
				_endFilter = static_cast<EndFilter>(map.getInt(PARAMETER_DISPLAY_END_FILTER, true, FACTORY_KEY));

				// Type
				uid id(map.getUid(PARAMETER_TYPE, true, FACTORY_KEY));
				_type = DisplayTypeTableSync::Get(id);

			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Display screen not specified or specified display screen not found");
			}
			catch (ObjectNotFoundException<DisplayType>&)
			{
				throw ActionException("Specified display type not found");
			}
			catch(...)
			{
				throw ActionException("Unknown error at display screen update");
			}
		}

		void UpdateDisplayScreenAction::run()
		{
			// Comparison for log text generation
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Nom", _screen->getLocalizationComment(), _name);
			DBLogModule::appendToLogIfChange(log, "Code de branchement", _screen->getWiringCode(), _wiringCode);
			DBLogModule::appendToLogIfChange(log, "Délai de clignotement", _screen->getBlinkingDelay(), _blinkingDelay);
			DBLogModule::appendToLogIfChange(log, "Affichage du numéro de quai", _screen->getTrackNumberDisplay(), _displayPlatform);
			DBLogModule::appendToLogIfChange(log, "Affichage du numéro de service", _screen->getServiceNumberDisplay(), _displayServiceNumber);
			DBLogModule::appendToLogIfChange(log, "Affichage du numéro d'équipe", _screen->getDisplayTeam(), _displayTeam);
			DBLogModule::appendToLogIfChange(log, "Type de tableau", _screen->getDirection(), _direction);
			DBLogModule::appendToLogIfChange(log, "Affichage des terminus seulement", _screen->getEndFilter(), _endFilter);
			DBLogModule::appendToLogIfChange(log, "Délai d'effacement", _screen->getClearingDelay(), _cleaningDelay);
			DBLogModule::appendToLogIfChange(log, "Délai d'apparition", _screen->getMaxDelay(), _maxDelay);
			if (_screen->getType())
				DBLogModule::appendToLogIfChange(log, "Type de panneau", _screen->getType()->getName(), _type->getName());
			DBLogModule::appendToLogIfChange(log, "Titre", _screen->getTitle(), _title);

			// Preparation of the action
			_screen->setLocalizationComment(_name);
			_screen->setWiringCode(_wiringCode);
			_screen->setBlinkingDelay(_blinkingDelay);
			_screen->setTrackNumberDisplay(_displayPlatform);
			_screen->setServiceNumberDisplay(_displayServiceNumber);
			_screen->setDirection(_direction);
			_screen->setOriginsOnly(_endFilter);
			_screen->setClearingDelay(_cleaningDelay);
			_screen->setMaxDelay(_maxDelay);
			_screen->setType(_type.get());
			_screen->setTitle(_title);
			_screen->setDisplayTeam(_displayTeam);

			// The action
			DisplayScreenTableSync::Save(_screen.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(_screen.get(), log.str(), _request->getUser().get());
		}
	}
}
