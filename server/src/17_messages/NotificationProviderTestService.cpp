/** NotificationProviderTestService class implementation.
	@file NotificationTestService.cpp
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

#include <Env.h>
#include <HTMLModule.h>
#include <NotificationProvider.hpp>
#include <NotificationProviderTestService.hpp>
#include <ObjectNotFoundException.h>
#include <Request.h>
#include <RequestException.h>
#include <Scenario.h>
#include <UtilTypes.h>

#include <boost/foreach.hpp>
#include <boost/optional/optional.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <iostream>
#include <map>
#include <set>
#include <utility>

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;

	template<>
	const string FactorableTemplate<Function, messages::NotificationProviderTestService>::FACTORY_KEY = "notification_provider_test";

	namespace messages
	{
		const string NotificationProviderTestService::PARAMETER_NOTIFICATION_PROVIDER_ID = "notification_provider_id";
		const string NotificationProviderTestService::PARAMETER_TEST_SCENARIO_ID = "test_scenario_id";
		const string NotificationProviderTestService::PARAMETER_TEST_NOTIFICATION_TYPE = "test_notification_type";
		const string NotificationProviderTestService::PARAMETER_TEST_PARAMETERS = "test_parameters";
		const string NotificationProviderTestService::TAG_TEST_FIELD = "test_field";
		const string NotificationProviderTestService::TAG_FIELD_NAME = "field_name";
		const string NotificationProviderTestService::TAG_FIELD_VALUE = "field_value";



		ParametersMap NotificationProviderTestService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void NotificationProviderTestService::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				// Reference to notification provider
				_notificationProvider = Env::GetOfficialEnv().getEditable<NotificationProvider>(
					map.get<RegistryKeyType>(PARAMETER_NOTIFICATION_PROVIDER_ID)
				).get();

				// Reference to scenario
				_testScenario = Env::GetOfficialEnv().getEditable<Scenario>(
					map.getDefault<RegistryKeyType>(PARAMETER_TEST_SCENARIO_ID)
				).get();

			}
			catch(ObjectNotFoundException<NotificationProvider>&)
			{
				_notificationProvider = NULL;
			}
			catch(ObjectNotFoundException<Scenario>&)
			{
				_testScenario = NULL;
			}
		}



		ParametersMap NotificationProviderTestService::run(
			std::ostream& stream,
			const Request& request
		) const {
			// Result
			ParametersMap map;

			if (!_notificationProvider || !_testScenario)
			{
				throw RequestException("Invalid parameters");
			}

			const ParametersMap testParameters(
				request.getParametersMap().get<string>(PARAMETER_TEST_PARAMETERS)
			);

			boost::optional<ParametersMap> fields;
			const int type = request.getParametersMap().getDefault<int>(PARAMETER_TEST_NOTIFICATION_TYPE, 1);
			Scenario::Messages testMessages = _testScenario->getMessages();
			if (!testMessages.empty())
			{
				// Temporarily replace parameters from request
				// Will be discarded if provider is not saved
				ParametersMap savedParameters = _notificationProvider->get<Parameters>();
				if (!testParameters.empty())
				{
					_notificationProvider->get<Parameters>().clear();
					_notificationProvider->get<Parameters>().merge(testParameters);
				}

				fields = _notificationProvider->generateScriptFields(*testMessages.begin(), type);

				// Restore parameters
				if (!testParameters.empty())
				{
					_notificationProvider->get<Parameters>().clear();
					_notificationProvider->get<Parameters>().merge(savedParameters);
				}
			}
			if (fields)
			{
				BOOST_FOREACH(const ParametersMap::Map::value_type& it, fields.get().getMap())
				{
					boost::shared_ptr<ParametersMap> aField(new ParametersMap);
					aField->insert(TAG_FIELD_NAME, it.first);
					aField->insert(TAG_FIELD_VALUE, html::HTMLModule::HTMLEncode(it.second));
					map.insert(TAG_TEST_FIELD, aField);
				}
			}

			return map;
		}



		bool NotificationProviderTestService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string NotificationProviderTestService::getOutputMimeType() const
		{
			return "text/html";
		}



		NotificationProviderTestService::NotificationProviderTestService():
			_notificationProvider(NULL),
			_testScenario(NULL)
		{ }

	} /* namespace messages */
} /* namespace synthese */
