
/** AlarmRemoveLinkAction class implementation.
	@file AlarmRemoveLinkAction.cpp

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

#include "01_util/Conversion.h"

#include "30_server/ActionException.h"

#include "17_messages/AlarmRemoveLinkAction.h"
#include "17_messages/AlarmObjectLinkTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	
	namespace messages
	{
		const string AlarmRemoveLinkAction::PARAMETER_ALARM_ID = Action_PARAMETER_PREFIX + "aid";
		const string AlarmRemoveLinkAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "oid";
		

		ParametersMap AlarmRemoveLinkAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_ALARM_ID, Conversion::ToString(_alarmId)));
			map.insert(make_pair(PARAMETER_OBJECT_ID, Conversion::ToString(_objectId)));
			return map;
		}

		void AlarmRemoveLinkAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			it = map.find(PARAMETER_OBJECT_ID);
			if (it == map.end())
				throw ActionException("Object ID not specified");
			_objectId = Conversion::ToLongLong(it->second);

			it = map.find(PARAMETER_ALARM_ID);
			if (it == map.end())
				throw ActionException("Alarm ID not specified");
			_alarmId = Conversion::ToLongLong(it->second);
		}

		void AlarmRemoveLinkAction::run()
		{
			AlarmObjectLinkTableSync::remove(_alarmId, _objectId);
		}

		void AlarmRemoveLinkAction::setAlarmId( uid id )
		{
			_alarmId = id;
		}

		void AlarmRemoveLinkAction::setObjectId( uid id )
		{
			_objectId = id;
		}
	}
}
