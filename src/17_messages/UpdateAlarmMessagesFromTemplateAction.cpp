
/** UpdateAlarmMessagesFromTemplateAction class implementation.
	@file UpdateAlarmMessagesFromTemplateAction.cpp

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

#include "UpdateAlarmMessagesFromTemplateAction.h"

#include "17_messages/Alarm.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/TextTemplate.h"
#include "17_messages/TextTemplateTableSync.h"
#include "17_messages/MessagesModule.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, messages::UpdateAlarmMessagesFromTemplateAction>::FACTORY_KEY("uaft");
	
	namespace messages
	{
		const string UpdateAlarmMessagesFromTemplateAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "tid";


		ParametersMap UpdateAlarmMessagesFromTemplateAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateAlarmMessagesFromTemplateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_message = AlarmTableSync::GetEditable(_request->getObjectId(), _env);

				uid id = map.getUid(PARAMETER_TEMPLATE_ID, true, FACTORY_KEY);
				_template = TextTemplateTableSync::Get(id, _env);
			}
			catch (ObjectNotFoundException<Alarm>& e)
			{
				throw ActionException("Specified message not found" + e.getMessage());
			}
			catch(ObjectNotFoundException<TextTemplate>& e)
			{
				throw ActionException("Specified template not found" + e.getMessage());
			}
		}

		void UpdateAlarmMessagesFromTemplateAction::run()
		{
			_message->setShortMessage(_template->getShortMessage());
			_message->setLongMessage(_template->getLongMessage());
			AlarmTableSync::Save(_message.get());
		}
	}
}
