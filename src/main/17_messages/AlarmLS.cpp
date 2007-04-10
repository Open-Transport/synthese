
/** AlarmLS class implementation.
	@file AlarmLS.cpp

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

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/XmlToolkit.h"

#include "17_messages/Alarm.h"
#include "17_messages/AlarmLS.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/Types.h"

#include "04_time/DateTime.h"

using namespace boost;

namespace synthese
{
	using namespace util::XmlToolkit;
	using namespace time;

	namespace messages
	{
		const std::string AlarmLS::ALARM_TAG ("alarm");
		const std::string AlarmLS::ALARM_ID_ATTR ("id");
		const std::string AlarmLS::ALARM_MESSAGE_ATTR ("message");
		const std::string AlarmLS::ALARM_PERIODSTART_ATTR ("periodStart");
		const std::string AlarmLS::ALARM_PERIODEND_ATTR ("periodEnd");

		const std::string AlarmLS::ALARM_LEVEL_ATTR ("level");
		const std::string AlarmLS::ALARM_LEVEL_ATTR_INFO ("info");
		const std::string AlarmLS::ALARM_LEVEL_ATTR_WARNING ("warning");





		void 
		AlarmLS::Load (XMLNode& node)
		{
			// assert (ALARM_TAG == node.getName ());
			uid id (GetLongLongAttr (node, ALARM_ID_ATTR));

			if (MessagesModule::getAlarms().contains (id)) return;

			std::string message (GetStringAttr (node, ALARM_MESSAGE_ATTR));
			DateTime periodStart (DateTime::FromString (GetStringAttr (node, ALARM_PERIODSTART_ATTR)));
			DateTime periodEnd (DateTime::FromString (GetStringAttr (node, ALARM_PERIODEND_ATTR)));

			std::string levelStr (GetStringAttr (node, ALARM_LEVEL_ATTR));
			AlarmLevel level (ALARM_LEVEL_INFO);
			if (levelStr == ALARM_LEVEL_ATTR_INFO) level = ALARM_LEVEL_INFO;
			else if (levelStr == ALARM_LEVEL_ATTR_WARNING) level = ALARM_LEVEL_WARNING;

			shared_ptr<Alarm> alarm(new Alarm);
			alarm->setKey(id);
			alarm->setShortMessage(message);
			alarm->setLongMessage(message);
			alarm->setPeriodStart(periodStart);
			alarm->setPeriodEnd(periodEnd);
			alarm->setLevel(level);


			MessagesModule::getAlarms ().add (alarm);
		    
		}




		XMLNode* 
		AlarmLS::Save (const Alarm* alarm)
		{
			// ...
			return 0;
		}
	}
}
