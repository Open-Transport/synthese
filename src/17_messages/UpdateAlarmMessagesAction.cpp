////////////////////////////////////////////////////////////////////////////////
/// UpdateAlarmMessagesAction class implementation.
///	@file UpdateAlarmMessagesAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ActionException.h"
#include "UpdateAlarmMessagesAction.h"
#include "ObjectNotFoundException.h"
#include "MessagesModule.h"
#include "Alarm.h"
#include "AlarmTemplate.h"
#include "SentAlarm.h"
#include "AlarmTableSync.h"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "DBLogModule.h"
#include "MessagesLibraryLog.h"
#include "MessagesLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace security;
	using namespace dblog;
	using namespace util;
	
	template<> const string util::FactorableTemplate<Action, messages::UpdateAlarmMessagesAction>::FACTORY_KEY("muama");

	namespace messages
	{
		const string UpdateAlarmMessagesAction::PARAMETER_SHORT_MESSAGE = Action_PARAMETER_PREFIX + "sme";
		const string UpdateAlarmMessagesAction::PARAMETER_LONG_MESSAGE = Action_PARAMETER_PREFIX + "lme";
		const string UpdateAlarmMessagesAction::PARAMETER_ALARM_ID(Action_PARAMETER_PREFIX + "id");
		const string UpdateAlarmMessagesAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "ty";


		ParametersMap UpdateAlarmMessagesAction::getParametersMap() const
		{
			ParametersMap map;
			if (_alarm.get() != NULL) map.insert(PARAMETER_ALARM_ID, _alarm->getKey());
			return map;
		}

		void UpdateAlarmMessagesAction::_setFromParametersMap(const ParametersMap& map) throw(ActionException)
		{
			try
			{
				setAlarmId(map.get<RegistryKeyType>(PARAMETER_ALARM_ID));
				_type = static_cast<AlarmLevel>(map.get<int>(PARAMETER_TYPE));
				_shortMessage = map.get<string>(PARAMETER_SHORT_MESSAGE);
				_longMessage = map.get<string>(PARAMETER_LONG_MESSAGE);
			}
			catch (ParametersMap::MissingParameterException& e)
			{
				throw ActionException(e, *this);
			}
		}



		void UpdateAlarmMessagesAction::run(
			Request& request
		) throw(ActionException) {
			stringstream s;
			DBLogModule::appendToLogIfChange(s,
				"type",
				MessagesModule::getLevelLabel(_alarm->getLevel()),
				MessagesModule::getLevelLabel(_type)
			);
			DBLogModule::appendToLogIfChange(s, "message court", _alarm->getShortMessage(), _shortMessage);
			DBLogModule::appendToLogIfChange(s, "message long", _alarm->getLongMessage(), _longMessage);

			_alarm->setLevel(_type);
			_alarm->setShortMessage(_shortMessage);
			_alarm->setLongMessage(_longMessage);
			AlarmTableSync::Save(_alarm.get());

			// Log
			if (dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm).get())
			{
				shared_ptr<const AlarmTemplate> alarmTemplate = dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm);
				MessagesLibraryLog::addUpdateEntry(alarmTemplate.get(), s.str(), request.getUser().get());
			}
			else
			{
				shared_ptr<const SentAlarm> scenarioSentAlarm = dynamic_pointer_cast<const SentAlarm, const Alarm>(_alarm);
				MessagesLog::addUpdateEntry(scenarioSentAlarm.get(), s.str(), request.getUser().get());
			}
		}



		bool UpdateAlarmMessagesAction::isAuthorized(const Session* session
		) const {
			if (dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm).get() != NULL)
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE);
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(WRITE);
			}
		}



		void UpdateAlarmMessagesAction::setAlarmId(
			util::RegistryKeyType id
		) throw(server::ActionException) {
			try
			{
				_alarm = AlarmTableSync::GetEditable(id, *_env);
			}
			catch (ObjectNotFoundException<Alarm>& e)
			{
				throw ActionException("message", e, *this);
			}
		}
	}
}
