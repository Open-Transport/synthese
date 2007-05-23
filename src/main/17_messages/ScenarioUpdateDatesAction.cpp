
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

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace time;
	
	namespace messages
	{
		const string ScenarioUpdateDatesAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "nam";
		const string ScenarioUpdateDatesAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sda";
		const string ScenarioUpdateDatesAction::PARAMETER_START_HOUR = Action_PARAMETER_PREFIX + "sho";
		const string ScenarioUpdateDatesAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "eda";
		const string ScenarioUpdateDatesAction::PARAMETER_END_HOUR = Action_PARAMETER_PREFIX + "eho";


		ParametersMap ScenarioUpdateDatesAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_NAME, _name));
			/// @todo Finish the implementation
			return map;
		}

		void ScenarioUpdateDatesAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_scenario = ScenarioTableSync::getSent(_request->getObjectId());

				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_NAME);
				if (it == map.end())
					throw ActionException("Name not specified");
				_name = it->second;

				it = map.find(PARAMETER_START_DATE);
				if (it == map.end())
					throw ActionException("Start date not specified");
				if (!it->second.empty())
					_startDate.getDate().FromString(it->second);

				it = map.find(PARAMETER_START_HOUR);
				if (it == map.end())
					throw ActionException("Start hour not specified");
				if (!it->second.empty())
					_startDate.getHour().FromString(it->second);

				it = map.find(PARAMETER_END_DATE);
				if (it == map.end())
					throw ActionException("End date not specified");
				if (!it->second.empty())
					_endDate.getDate().FromString(it->second);

				it = map.find(PARAMETER_END_HOUR);
				if (it == map.end())
					throw ActionException("End hour not specified");
				if (!it->second.empty())
					_endDate.getHour().FromString(it->second);
			}
			catch (DBEmptyResultException<Scenario>)
			{
				throw ActionException("Scenario not found");
			}
		}

		ScenarioUpdateDatesAction::ScenarioUpdateDatesAction()
			: Action()
			, _startDate(TIME_UNKNOWN)
			, _endDate(TIME_UNKNOWN)
		{}

		void ScenarioUpdateDatesAction::run()
		{
			_scenario->setName(_name);
			_scenario->setPeriodStart(_startDate);
			_scenario->setPeriodEnd(_endDate);
			ScenarioTableSync::save(_scenario.get());
		}
	}
}
