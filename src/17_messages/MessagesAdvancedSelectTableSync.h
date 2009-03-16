////////////////////////////////////////////////////////////////////////////////
///	MessagesAdvancedSelectTableSync class header.
///	@file MessagesAdvancedSelectTableSync.h
///	@author Hugues Romain (RCS)
///	@date ven jan 30 2009
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

#ifndef SYNTHESE_MessagesAdvancedSelectTableSync_h__
#define SYNTHESE_MessagesAdvancedSelectTableSync_h__

#include "DateTime.h"
#include "Registry.h"
#include "17_messages/Types.h"

#include <vector>
#include <boost/logic/tribool.hpp>

namespace synthese
{
	namespace messages
	{
		class SentScenario;
		class SingleSentAlarm;
		
		struct SentMessage
		{
			util::RegistryKeyType id;
			time::DateTime startTime;
			time::DateTime endTime;
			bool enabled;
			std::string name;
			AlarmLevel level;
			AlarmConflict conflict;
			
			SentMessage()
			:	startTime(time::TIME_UNKNOWN),
				endTime(time::TIME_UNKNOWN)
			{}
		};
		
		typedef std::vector<SentMessage> SentMessages;
		
		typedef enum
		{
			BROADCAST_OVER,
			BROADCAST_RUNNING,
			BROADCAST_RUNNING_WITH_END,
			BROADCAST_RUNNING_WITHOUT_END,
			FUTURE_BROADCAST,
			BROADCAST_DRAFT
		} StatusSearch;

		/** Messages selector.
			Typical uses :
				- BROADCAST_OVER : endDate in past
				- BROADCAST_RUNNING : startDate in past or unknown + activated,
				- BROADCAST_RUNNING_WITH_END : startDate in past or unknown + endTime in future + activated
				- BROADCAST_RUNNING_WITHOUT_END : startDate in past or unknown + endTime unknown + activated,
				- FUTURE_BROADCAST : startDate in future + activated,
				- BROADCAST_DRAFT : endDate in future or unknown + not activated 
		*/
		SentMessages GetSentMessages(
			const time::Date& date,
			StatusSearch status,
			AlarmConflict conflict,
			AlarmLevel level,
			util::RegistryKeyType scenarioId = UNKNOWN_VALUE,
			int first = 0,
			int number = 0,
			bool orderByDate = true,
			bool orderByLevel = false,
			bool orderByStatus = false,
			bool orderByConflict = false,
			bool raisingOrder = false
		);
	}
}

#endif // SYNTHESE_MessagesAdvancedSelectTableSync_h__
