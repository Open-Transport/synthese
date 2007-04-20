
/** MessagesModule class header.
	@file MessagesModule.h

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

#ifndef SYNTHESE_MessagesModule_H__
#define SYNTHESE_MessagesModule_H__

#include "01_util/ModuleClass.h"

#include "17_messages/AlarmBroadcastList.h"
#include "17_messages/Types.h"
#include "17_messages/SentAlarm.h"
#include "17_messages/SentScenario.h"
#include "17_messages/AlarmObjectLink.h"

namespace synthese
{
	/** @defgroup m17Actions Actions
		@ingroup m17

		@defgroup m17 17 Messages
	@{	*/

	/** 17 Messages module namespace.
	*/
	namespace messages
	{
		/** 17 Messages module class.
		*/
		class MessagesModule : public util::ModuleClass
		{
		public:
//			typedef messages::AlarmBroadcastList<env::Line>				LineBroadcastList;
//			typedef messages::AlarmBroadcastList<departurestable::BroadcastPoint>	BroadcastPointBroadcastList;

		private:
			static SentAlarm::Registry			_alarms;
			static SentScenario::Registry		_scenarii;
			static AlarmObjectLink::Registry	_alarmLinks;

		public:
			static SentAlarm::Registry&			getAlarms();
			static SentScenario::Registry&		getScenarii();
			static AlarmObjectLink::Registry&	getAlarmLinks();
			
			void initialize();

			static std::vector<std::pair<uid, std::string> >			getScenarioTemplatesLabels(bool withAll = false);
			static std::vector<std::pair<AlarmLevel, std::string> >		getLevelLabels(bool withAll = false);
			static std::vector<std::pair<AlarmConflict, std::string> >	getConflictLabels(bool withAll = false);
			static std::vector<std::pair<uid, std::string> >			getTextTemplateLabels(const AlarmLevel& level);

			static std::string							getLevelLabel(const AlarmLevel& level);
		};
	}
	/** @} */
}

#endif // SYNTHESE_MessagesModule_H__
