/** BroadcastPoint class header.
	@file BroadcastPoint.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_messages_BroadcastPoint_hpp__
#define SYNTHESE_messages_BroadcastPoint_hpp__

#include "FactoryBase.h"

#include "Alarm.h"

#include <vector>

namespace synthese
{
	namespace messages
	{
		class Alarm;
		class MessageType;
		class Scenario;

		//////////////////////////////////////////////////////////////////////////
		/** BroadcastPoint class.
			@ingroup m17
		*/
		class BroadcastPoint:
			public util::FactoryBase<BroadcastPoint>, 
			public virtual util::Registrable
		{
		public:
			static const std::string VAR_BROADCAST_POINT_TYPE;
			static const std::string VAR_MESSAGE_ID;

			BroadcastPoint() {}

			virtual ~BroadcastPoint() {}

			virtual std::set<MessageType*> getMessageTypes() const = 0;

			//////////////////////////////////////////////////////////////////////////
			/// Checks if the broadcast point displays the specified scenario.
			///
			/// @param scenario the scenario to test
			/// @param the display parameters
			/// @return true if at least one message of the specified scenario is currently displayed on the broadcast point.
			/// Note : if the scenario is a template the check is a simulation
			virtual bool displaysScenario(const Scenario& scenario) const;

			//////////////////////////////////////////////////////////////////////////
			/// Checks if the broadcast point displays the specified message.
			/// @param linkedObjects the objects linked to the message
			/// @param the display parameters
			/// @return true if the specified message is currently displayed on the broadcast point.
			/// Note : if the message is a template the check is a simulation
			virtual bool displaysMessage(
				const Alarm::LinkedObjects& linkedObjects,
				const util::ParametersMap& parameters
			) const = 0;

			typedef std::vector<BroadcastPoint*> BroadcastPoints;

			//////////////////////////////////////////////////////////////////////////
			/// Feed broadcast point instances for each derived class
			/// @param result vector of BroadcastPoint pointers
			virtual void getBroadcastPoints(BroadcastPoints& result) const = 0;

			/// Complete list of broadcast point instances for all derived classes
			/// @return vector of BroadcastPoint pointers
			static BroadcastPoints GetBroadcastPoints();

			//////////////////////////////////////////////////////////////////////////
			/// Function to run when the message display begins.
			/// Default implementation does nothing.
			/// @param message the xAlarm to display
			virtual void onDisplayStart(
				const Alarm& message
			) const {}

			//////////////////////////////////////////////////////////////////////////
			/// Function to run when the message display ends.
			/// Default implementation does nothing.
			/// @param message the Alarm to hide
			virtual void onDisplayEnd(
				const Alarm& message
			) const {}

			//////////////////////////////////////////////////////////////////////////
			/// Function to run before an applicable message is updated.
			/// Default implementation does nothing.
			/// @param message the Alarm to hide
			virtual void beforeMessageUpdate(
				const Alarm& message
			) const {}

			//////////////////////////////////////////////////////////////////////////
			/// Function to run after an applicable message is updated.
			/// Default implementation does nothing.
			/// @param message the Alarm to hide
			virtual void afterMessageUpdate(
				const Alarm& message
			) const {}

		};
}	}

#endif // SYNTHESE_messages_BroadcastPoint_hpp__
