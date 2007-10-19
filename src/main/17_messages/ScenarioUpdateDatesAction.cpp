
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

#include "ScenarioUpdateDatesAction.h"

#include "17_messages/MessagesModule.h"
#include "17_messages/SentScenario.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/MessagesLog.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

#include "04_time/TimeParseException.h"

#include "01_util/Conversion.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace time;
	using namespace util;

	template<> const string util::FactorableTemplate<Action,messages::ScenarioUpdateDatesAction>::FACTORY_KEY("messscenarioud");
		
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

				_enabled = map.getBool(PARAMETER_ENABLED, true, false, FACTORY_KEY);

				_startDate = map.getDateTime(PARAMETER_START_DATE, true, FACTORY_KEY);

				_endDate = map.getDateTime(PARAMETER_END_DATE, true, FACTORY_KEY);
			}
			catch (ObjectNotFoundException<uid,Scenario>& e)
			{
				throw ActionException(e.getMessage());
			}
			catch(TimeParseException& e)
			{
				throw ActionException("Une date ou une heure est mal formée");
			}

		}

		ScenarioUpdateDatesAction::ScenarioUpdateDatesAction()
			: FactorableTemplate<Action, ScenarioUpdateDatesAction>()
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
			MessagesLog::addUpdateEntry(_scenario.get(), text.str(), _request->getUser().get());
		}
	}
}
