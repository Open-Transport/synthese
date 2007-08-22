
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

#include "01_util/Conversion.h"

#include "02_db/DBEmptyResultException.h"

#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/PhysicalStop.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "34_departures_table/UpdateDisplayScreenAction.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"
#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayScreen.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace db;
	
	namespace departurestable
	{
		const string UpdateDisplayScreenAction::PARAMETER_NAME(Action_PARAMETER_PREFIX + "name");
		const std::string UpdateDisplayScreenAction::PARAMETER_WIRING_CODE = Action_PARAMETER_PREFIX + "wc";
		const std::string UpdateDisplayScreenAction::PARAMETER_BLINKING_DELAY = Action_PARAMETER_PREFIX + "bd";
		const std::string UpdateDisplayScreenAction::PARAMETER_CLEANING_DELAY = Action_PARAMETER_PREFIX + "cd";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_PLATFORM = Action_PARAMETER_PREFIX + "dp";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_SERVICE_NUMBER = Action_PARAMETER_PREFIX + "ds";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_DEPARTURE_ARRIVAL = Action_PARAMETER_PREFIX + "da";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_END_FILTER = Action_PARAMETER_PREFIX + "ef";
		const std::string UpdateDisplayScreenAction::PARAMETER_DISPLAY_MAX_DELAY = Action_PARAMETER_PREFIX + "md";
		const std::string UpdateDisplayScreenAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "ty";
		const std::string UpdateDisplayScreenAction::PARAMETER_TITLE = Action_PARAMETER_PREFIX + "tt";



		ParametersMap UpdateDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::get(_request->getObjectId());

				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_NAME);
				if (it == map.end())
					throw ActionException("Name not specified");
				_name = it->second;

				it= map.find(PARAMETER_TITLE);
				if (it == map.end())
					throw ActionException("Title not specified");
				_title = it->second;

				it = map.find(PARAMETER_WIRING_CODE);
				if (it != map.end())
					_wiringCode = Conversion::ToInt(it->second);

				it = map.find(PARAMETER_BLINKING_DELAY);
				if (it != map.end())
					_blinkingDelay = Conversion::ToInt(it->second);

				it = map.find(PARAMETER_CLEANING_DELAY);
				if (it != map.end())
					_cleaningDelay = Conversion::ToInt(it->second);

				it = map.find(PARAMETER_DISPLAY_MAX_DELAY);
				if (it != map.end())
					_maxDelay = Conversion::ToInt(it->second);

				it = map.find(PARAMETER_DISPLAY_PLATFORM);
				if (it != map.end())
					_displayPlatform = Conversion::ToBool(it->second);

				it = map.find(PARAMETER_DISPLAY_SERVICE_NUMBER);
				if (it != map.end())
					_displayServiceNumber = Conversion::ToBool(it->second);

				it = map.find(PARAMETER_DISPLAY_DEPARTURE_ARRIVAL);
				if (it != map.end())
					_direction = (DeparturesTableDirection) Conversion::ToInt(it->second);

				it = map.find(PARAMETER_DISPLAY_END_FILTER);
				if (it != map.end())
					_endFilter = (EndFilter) Conversion::ToInt(it->second);

				it = map.find(PARAMETER_TYPE);
				if (it != map.end())
					_type = DisplayType::Get(Conversion::ToLongLong(it->second));

			}
			catch (DBEmptyResultException<DisplayScreen>&)
			{
				throw ActionException("Display screen not specified or specified display screen not found");
			}
			catch (DisplayType::RegistryKeyException&)
			{
				throw ActionException("Specified display type not found");
			}
		}

		void UpdateDisplayScreenAction::run()
		{
			// Comparison for log text generation
			stringstream log;
			if (_screen->getLocalizationComment() != _name)
				log << " - Nom : " << _screen->getLocalizationComment() << " => " << _name;
			if (_screen->getWiringCode() != _wiringCode)
				log << " - Code de branchement : " << _screen->getWiringCode() << " => " << _wiringCode;
			if (_screen->getBlinkingDelay() != _blinkingDelay)
				log << " - Délai de clignotement : " << _screen->getBlinkingDelay() << " => " << _blinkingDelay;
			if (_screen->getTrackNumberDisplay() != _displayPlatform)
				log << " - Affichage du numéro de quai : " << _screen->getTrackNumberDisplay() << " => " << _displayPlatform;
			if (_screen->getServiceNumberDisplay() != _displayServiceNumber)
				log << " - Affichage du numéro de service : " << _screen->getServiceNumberDisplay() << " => " << _displayServiceNumber;
			if (_screen->getDirection() != _direction)
				log << " - Type de tableau : " << _screen->getDirection() << " => " << _direction;
			if (_screen->getDirection() != _endFilter)
				log << " - Affichage des terminus seulement : " << _screen->getDirection() << " => " << _endFilter;
			if (_screen->getClearingDelay() != _cleaningDelay)
				log << " - Délai d'effacement : " << _screen->getClearingDelay() << " => " << _cleaningDelay;
			if (_screen->getMaxDelay() != _maxDelay)
				log << " - Délai d'apparition : " << _screen->getMaxDelay() << " => " << _maxDelay;
			if (_screen->getType() && _screen->getType() != _type.get())
				log << " - Type de panneau : " << _screen->getType()->getName() << " => " << _type->getName();
			if (_screen->getTitle() != _title)
				log << " - Titre : " << _screen->getTitle() << " => " << _title;

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

			// The action
			DisplayScreenTableSync::save(_screen.get());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(_screen, log.str(), _request->getUser());
		}
	}
}
