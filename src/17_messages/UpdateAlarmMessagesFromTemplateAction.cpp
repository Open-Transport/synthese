////////////////////////////////////////////////////////////////////////////////
/// UpdateAlarmMessagesFromTemplateAction class implementation.
///	@file UpdateAlarmMessagesFromTemplateAction.cpp
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

#include "UpdateAlarmMessagesFromTemplateAction.h"
#include "ActionException.h"
#include "ObjectNotFoundException.h"
#include "RequestMissingParameterException.h"
#include "MessagesRight.h"
#include "MessagesLibraryRight.h"
#include "Alarm.h"
#include "AlarmTableSync.h"
#include "TextTemplate.h"
#include "TextTemplateTableSync.h"
#include "MessagesModule.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "AlarmTemplate.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;
	using namespace security;
	

	template<> const string util::FactorableTemplate<Action, messages::UpdateAlarmMessagesFromTemplateAction>::FACTORY_KEY("uaft");
	
	namespace messages
	{
		const string UpdateAlarmMessagesFromTemplateAction::PARAMETER_ALARM_ID(Action_PARAMETER_PREFIX + "ai");
		const string UpdateAlarmMessagesFromTemplateAction::PARAMETER_TEMPLATE_ID(Action_PARAMETER_PREFIX + "ti");


		ParametersMap UpdateAlarmMessagesFromTemplateAction::getParametersMap() const
		{
			ParametersMap map;
			if (_message.get() != NULL) map.insert(PARAMETER_ALARM_ID, _message.get());
			return map;
		}

		void UpdateAlarmMessagesFromTemplateAction::_setFromParametersMap(const ParametersMap& map
		) throw(ActionException)
		{
			try
			{
				setAlarmId(map.getUid(PARAMETER_ALARM_ID, true, FACTORY_KEY));
				uid id = map.getUid(PARAMETER_TEMPLATE_ID, true, FACTORY_KEY);
				_template = TextTemplateTableSync::Get(id, _env);
			}
			catch(ObjectNotFoundException<TextTemplate>& e)
			{
				throw ActionException("template", FACTORY_KEY, e);
			}
			catch(RequestMissingParameterException& e)
			{
				throw ActionException(e.getMessage());
			}
		}

		void UpdateAlarmMessagesFromTemplateAction::run() throw(ActionException)
		{
			_message->setShortMessage(_template->getShortMessage());
			_message->setLongMessage(_template->getLongMessage());
			AlarmTableSync::Save(_message.get());
		}



		void UpdateAlarmMessagesFromTemplateAction::setAlarmId(
			util::RegistryKeyType id
		) throw(server::ActionException) {

			try
			{
				_message = AlarmTableSync::GetEditable(id, _env);
			}
			catch (ObjectNotFoundException<Alarm>& e)
			{
				throw ActionException("edited message", id, FACTORY_KEY, e);
			}
			
		}



		bool UpdateAlarmMessagesFromTemplateAction::_isAuthorized(
		) const {
			if (dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_message).get() != NULL)
			{
				return _request->isAuthorized<MessagesLibraryRight>(WRITE);
			}
			else
			{
				return _request->isAuthorized<MessagesRight>(WRITE);
			}
		}
	}
}
