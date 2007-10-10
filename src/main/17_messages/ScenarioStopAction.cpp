
/** ScenarioStopAction class implementation.
	@file ScenarioStopAction.cpp

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

#include "ScenarioStopAction.h"

#include "17_messages/ScenarioTableSync.h"
#include "17_messages/SentScenario.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesLog.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace time;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, messages::ScenarioStopAction>::FACTORY_KEY("scenariostop");
	
	namespace messages
	{
		ParametersMap ScenarioStopAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void ScenarioStopAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_scenario = ScenarioTableSync::getSent(_request->getObjectId());
			}
			catch (...) {
				throw ActionException("Invalid scenario");
			}
		}

		void ScenarioStopAction::run()
		{
			// Action
			_scenario->setPeriodEnd(_stopDateTime);
			_scenario->setIsEnabled(false);
			ScenarioTableSync::save(_scenario.get());

			// Log
			MessagesLog::addUpdateEntry(_scenario.get(), "Diffusion arrêtée le " + _stopDateTime.toString(), _request->getUser().get());
		}

		ScenarioStopAction::ScenarioStopAction()
			: FactorableTemplate<Action, ScenarioStopAction>(), _stopDateTime(TIME_CURRENT)
		{
	
		}
	}
}
