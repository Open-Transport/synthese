/** NotificationEventsService class implementation.
	@file NotificationEventsService.cpp
	@author yves.martin
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include <Alarm.h>
#include <Env.h>
#include <NotificationEvent.hpp>
#include <NotificationEventsService.hpp>
#include <NotificationEventTableSync.hpp>
#include <ObjectNotFoundException.h>
#include <Registrable.h>
#include <RequestException.h>
#include <Scenario.h>
#include <UtilTypes.h>

#include <iostream>
#include <map>
#include <set>

#include <boost/foreach.hpp>
#include <boost/optional/optional.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;

	template<>
	const string FactorableTemplate<Function, messages::NotificationEventsService>::FACTORY_KEY = "notification_events";

	namespace messages
	{
	const string NotificationEventsService::PARAMETER_SCENARIO_ID = "scenario_id";
		const string NotificationEventsService::PARAMETER_UNHOLD_EVENTS = "unhold_events";
		const string NotificationEventsService::TAG_HOLD_COUNT = "hold_count";
		const string NotificationEventsService::TAG_FAILED_COUNT = "failed_count";


		ParametersMap NotificationEventsService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void NotificationEventsService::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				// Reference to scenario
				_scenario = Env::GetOfficialEnv().getEditable<Scenario>(
					map.getDefault<RegistryKeyType>(PARAMETER_SCENARIO_ID)
				).get();

			}
			catch(ObjectNotFoundException<Scenario>&)
			{
				_scenario = NULL;
			}

			_optionUnhold = map.getDefault<bool>(PARAMETER_UNHOLD_EVENTS, false);
		}



		ParametersMap NotificationEventsService::run(
			std::ostream& stream,
			const Request& request
		) const {
			// Result
			ParametersMap map;

			if (!_scenario)
			{
				throw RequestException("Invalid parameters");
			}

			Scenario::Messages alarms = _scenario->getMessages();

			Env env = Env::GetOfficialEnv();
			size_t hold_count = 0;
			size_t failed_count = 0;

			BOOST_FOREACH(const Alarm* alarm, alarms)
			{
				NotificationEventTableSync::SearchResult resultEvents(
					NotificationEventTableSync::Search(
						env,
						alarm->getKey()
				)	);

				BOOST_FOREACH(const shared_ptr<NotificationEvent> event, resultEvents)
				{
					if (event->get<Status>() == HOLD)
					{
						if (_optionUnhold)
						{
							event->set<Status>(READY);
							NotificationEventTableSync::Save(event.get());
						}
						else
						{
							hold_count++;
						}
					}
					// Only report attempts older than 2 seconds
					posix_time::ptime justBefore =
							posix_time::second_clock::local_time()
							- posix_time::seconds(2);

					if (event->get<Status>() == FAILED
						|| (event->get<Status>() == IN_PROGRESS
							&& !event->get<LastAttempt>().is_not_a_date_time()
							&& event->get<LastAttempt>() < justBefore))
					{
						failed_count++;
					}
				}
			}
			map.insert(TAG_HOLD_COUNT, hold_count);
			map.insert(TAG_FAILED_COUNT, failed_count);
			return map;
		}



		bool NotificationEventsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string NotificationEventsService::getOutputMimeType() const
		{
			return "text/html";
		}



		NotificationEventsService::NotificationEventsService():
			_scenario(NULL),
			_optionUnhold(false)
		{ }

	} /* namespace messages */
} /* namespace synthese */
