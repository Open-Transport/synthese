
/** ScenarioUpdateDatesAction class implementation.
	@file ScenarioUpdateDatesAction.cpp

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

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "17_messages/ScenarioUpdateDatesAction.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/SentScenario.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/MessagesLog.h"

#include "04_time/TimeParseException.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace time;
	
	namespace messages
	{
		const string ScenarioUpdateDatesAction::PARAMETER_ENABLED(Action_PARAMETER_PREFIX + "ena");
		const string ScenarioUpdateDatesAction::PARAMETER_START_DATE(Action_PARAMETER_PREFIX + "sda");
		const string ScenarioUpdateDatesAction::PARAMETER_END_DATE(Action_PARAMETER_PREFIX + "eda");


		ParametersMap ScenarioUpdateDatesAction::getParametersMap() const
		{
			ParametersMap map;
			/// @todo Finish the implementation
			return map;
		}

		void ScenarioUpdateDatesAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_scenario = ScenarioTableSync::getSent(_request->getObjectId());

				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_ENABLED);
				if (it == map.end())
					throw ActionException("Enabled status not specified");
				_enabled = Conversion::ToBool(it->second);

				it = map.find(PARAMETER_START_DATE);
				if (it == map.end())
					throw ActionException("Start date not specified");
				if (!it->second.empty())
					_startDate = DateTime::FromString(it->second);

				it = map.find(PARAMETER_END_DATE);
				if (it == map.end())
					throw ActionException("End date not specified");
				if (!it->second.empty())
					_endDate = DateTime::FromString(it->second);
			}
			catch (DBEmptyResultException<Scenario>)
			{
				throw ActionException("Scenario not found");
			}
			catch(TimeParseException)
			{
				throw ActionException("Une date ou une heure est mal formée");
			}

		}

		ScenarioUpdateDatesAction::ScenarioUpdateDatesAction()
			: Action()
			, _startDate(TIME_UNKNOWN)
			, _endDate(TIME_UNKNOWN)
		{}

		void ScenarioUpdateDatesAction::run()
		{
			// Log message
			stringstream text;
			if (_scenario->getIsEnabled() != _enabled)
				text << " - " << (_enabled ? "Affichage activé" : "Affichage désactivé");
			if (_scenario->getPeriodStart() != _startDate)
				text << " - Date de début : " << _scenario->getPeriodStart().toString() << " => " << _startDate.toString();
			if (_scenario->getPeriodEnd() != _endDate)
				text << " - Date de fin : " << _scenario->getPeriodEnd().toString() << " => " << _endDate.toString();

			// Action
			_scenario->setIsEnabled(_enabled);
			_scenario->setPeriodStart(_startDate);
			_scenario->setPeriodEnd(_endDate);
			ScenarioTableSync::save(_scenario.get());

			// Log
			MessagesLog::addUpdateEntry(static_pointer_cast<const SentScenario, SentScenario>(_scenario), text.str(), _request->getUser());
		}
	}
}
