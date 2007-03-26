
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
#include "17_messages/Alarm.h"
#include "17_messages/Types.h"
#include "17_messages/Scenario.h"

/** @defgroup m17 17 Messages
@{	*/
namespace synthese
{
	namespace messages
	{
		
		class MessagesModule : public util::ModuleClass
		{
		public:
//			typedef messages::AlarmBroadcastList<env::Line>				LineBroadcastList;
//			typedef messages::AlarmBroadcastList<departurestable::BroadcastPoint>	BroadcastPointBroadcastList;

		private:
			static Alarm::Registry		_alarms;
			static Scenario::Registry	_scenarii;

		public:
			static Alarm::Registry&		getAlarms();
			static Scenario::Registry&	getScenarii();
			
			void initialize();

			static std::map<uid, std::string>			getScenariiLabels(bool withAll = false);
			static std::map<AlarmLevel, std::string>	getLevelLabels(bool withAll = false);
			static std::map<AlarmConflict, std::string>	getConflictLabels(bool withAll = false);
			static std::map<uid, std::string>			getTextTemplateLabels(const AlarmLevel& level);

			static std::string							getLevelLabel(const AlarmLevel& level);
		};
	}
}

/** @} */

#endif // SYNTHESE_MessagesModule_H__
