
//////////////////////////////////////////////////////////////////////////////////////////
///	MessageTypesService class implementation.
///	@file MessageTypesService.cpp
///	@author hromain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "MessageTypesService.hpp"

#include "Alarm.h"
#include "BroadcastPoint.hpp"
#include "MessageType.hpp"
#include "RequestException.h"
#include "Request.h"
#include "Scenario.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,messages::MessageTypesService>::FACTORY_KEY = "message_types";


	
	namespace messages
	{
		const string MessageTypesService::PARAMETER_SCENARIO_ID = "scenario_id";
		const string MessageTypesService::PARAMETER_MESSAGE_ID = "message_id";


		const string MessageTypesService::TAG_TYPE = "type";
		


		ParametersMap MessageTypesService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void MessageTypesService::_setFromParametersMap(const ParametersMap& map)
		{
			// Message type
			RegistryKeyType typeId(
				map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0)
			);
			if(typeId > 0)
			{
				try
				{
					_messageType = Env::GetOfficialEnv().get<MessageType>(typeId).get();
				}
				catch (ObjectNotFoundException<MessageType>&)
				{
					throw RequestException("No such message type");
				}
			}
			else
			{
				// Scenario
				RegistryKeyType scenarioId(
					map.getDefault<RegistryKeyType>(PARAMETER_SCENARIO_ID, 0)
				);
				if(scenarioId > 0)
				{
					try
					{
						_scenario = Env::GetOfficialEnv().get<Scenario>(scenarioId).get();
					}
					catch (ObjectNotFoundException<Scenario>&)
					{
						throw RequestException("No such scenario");
					}
				}

				// Message
				RegistryKeyType messageId(
					map.getDefault<RegistryKeyType>(PARAMETER_MESSAGE_ID, 0)
				);
				if(messageId > 0)
				{
					try
					{
						_message = Env::GetOfficialEnv().get<Alarm>(messageId).get();
					}
					catch (ObjectNotFoundException<Alarm>&)
					{
						throw RequestException("No such message");
					}
			}	}
		}



		ParametersMap MessageTypesService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap pm;

			if(_messageType)
			{
				boost::shared_ptr<ParametersMap> typePM(new ParametersMap);
				_messageType->toParametersMap(*typePM);
				pm.insert(TAG_TYPE, typePM);
			}
			else if(_scenario || _message) // Messages types needed by a scenario
			{
				set<MessageType*> messageTypes;

				// Broadcast points loop
				ParametersMap fakeParameters;
				BOOST_FOREACH(BroadcastPoint* broadcastPoint, BroadcastPoint::GetBroadcastPoints())
				{
					BOOST_FOREACH(MessageType* messageType, broadcastPoint->getMessageTypes())
					{
						// Jump over null or already selected message type 
						if (messageType == NULL ||
							messageTypes.find(messageType) != messageTypes.end()
							){
							continue;
						}

						// Search if a message of the scenario have to be displayed on the broadcast point
						if ((_scenario && broadcastPoint->displaysScenario(*_scenario)) ||
							(_message && _message->isOnBroadcastPoint(*broadcastPoint, fakeParameters))
							){
							messageTypes.insert(messageType);
						}
					}
				}

				// Loop on selected message types
				BOOST_FOREACH(MessageType* messageType, messageTypes)
				{
					boost::shared_ptr<ParametersMap> typePM(new ParametersMap);
					messageType->toParametersMap(*typePM);
					pm.insert(TAG_TYPE, typePM);
				}
			}
			else // Entire registry
			{
				BOOST_FOREACH(const MessageType::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<MessageType>())
				{
					boost::shared_ptr<ParametersMap> typePM(new ParametersMap);
					it.second->toParametersMap(*typePM);
					pm.insert(TAG_TYPE, typePM);
				}
			}

			return pm;
		}
		
		
		
		bool MessageTypesService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string MessageTypesService::getOutputMimeType() const
		{
			return "text/html";
		}



		MessageTypesService::MessageTypesService():
			_messageType(NULL),
			_scenario(NULL),
			_message(NULL)
		{

		}
}	}
