
//////////////////////////////////////////////////////////////////////////
/// SimpleMessageCreationAction class implementation.
/// @file SimpleMessageCreationAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "SimpleMessageCreationAction.hpp"
#include "MessagesRight.h"
#include "Request.h"
#include "MessagesLog.h"
#include "AlarmObjectLinkTableSync.h"
#include "DBTransaction.hpp"
#include "SentScenario.h"
#include "ScenarioTableSync.h"
#include "SentAlarm.h"
#include "AlarmTableSync.h"

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, messages::SimpleMessageCreationAction>::FACTORY_KEY("SimpleMessageCreationAction");
	}

	namespace messages
	{
		const string SimpleMessageCreationAction::PARAMETER_CONTENT = Action_PARAMETER_PREFIX + "c";
		const string SimpleMessageCreationAction::PARAMETER_LEVEL = Action_PARAMETER_PREFIX + "l";
		const string SimpleMessageCreationAction::PARAMETER_RECIPIENT_ID = Action_PARAMETER_PREFIX + "r";



		ParametersMap SimpleMessageCreationAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_CONTENT, _content);
			map.insert(PARAMETER_LEVEL, static_cast<int>(_level));
			map.insert(PARAMETER_RECIPIENT_ID, _recipientId);
			return map;
		}



		void SimpleMessageCreationAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Content
			_content = map.get<string>(PARAMETER_CONTENT);
			if(_content.empty())
			{
				throw ActionException("The message cannot be empty");
			}

			// Level
			_level = static_cast<AlarmLevel>(map.get<int>(PARAMETER_LEVEL));

			// Recipient
			_recipientId = map.get<RegistryKeyType>(PARAMETER_RECIPIENT_ID);
		}



		void SimpleMessageCreationAction::run(
			Request& request
		){
			DBTransaction transaction;
			ptime time(second_clock::local_time());

			SentScenario scenario;
			scenario.setIsEnabled(true);
			scenario.setName("Short message");
			scenario.setTemplate(NULL);
			scenario.setPeriodStart(time);
			ScenarioTableSync::Save(&scenario, transaction);

			SentAlarm alarm;
			alarm.setLevel(_level);
			alarm.setShortMessage("Short message");
			alarm.setLongMessage(_content);
			alarm.setScenario(&scenario);
			AlarmTableSync::Save(&alarm, transaction);

			AlarmObjectLink link;
			link.setObjectId(_recipientId);
			link.setAlarm(&alarm);
			AlarmObjectLinkTableSync::Save(&link, transaction);

			transaction.run();

			MessagesLog::AddNewSentScenarioEntry(scenario, *request.getUser());
			request.setActionCreatedId(link.getKey());
		}



		bool SimpleMessageCreationAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(WRITE);
		}



		SimpleMessageCreationAction::SimpleMessageCreationAction():
		_recipientId(0),
		_level(ALARM_LEVEL_INFO)
		{

		}
}	}
