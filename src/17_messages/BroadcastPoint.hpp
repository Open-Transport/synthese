
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

#include <vector>

namespace synthese
{
	namespace messages
	{
		class MessageType;
		class Scenario;

		/** BroadcastPoint class.
			@ingroup m17
		*/
		class BroadcastPoint:
			public util::FactoryBase<BroadcastPoint>
		{
		public:
			BroadcastPoint() {}

			virtual MessageType* getMessageType() const = 0;

			virtual bool displaysMessage(const Scenario& message) const = 0;

			typedef std::vector<BroadcastPoint*> BroadcastPoints;
			
			virtual void getBrodcastPoints(BroadcastPoints& result) const = 0;

			static BroadcastPoints GetBroadcastPoints();
		};
}	}

#endif // SYNTHESE_messages_BroadcastPoint_hpp__

