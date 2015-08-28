
/** BroadcastPoint class header.
	@file BroadcastPoint.cpp

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

#ifndef SYNTHESE_messages_BroadcastPoint_cpp__
#define SYNTHESE_messages_BroadcastPoint_cpp__

#include "BroadcastPoint.hpp"

#include "Factory.h"
#include "Scenario.h"

#include <boost/foreach.hpp>

using namespace boost;

namespace synthese
{
	using namespace util;

	namespace messages
	{
		const std::string BroadcastPoint::VAR_BROADCAST_POINT_TYPE = "broadcast_point_type";
		const std::string BroadcastPoint::VAR_MESSAGE_ID = "message_id";



		BroadcastPoint::BroadcastPoints BroadcastPoint::GetBroadcastPoints()
		{
			BroadcastPoints r;
			BOOST_FOREACH(boost::shared_ptr<BroadcastPoint> item, Factory<BroadcastPoint>::GetNewCollection())
			{
				item->getBroadcastPoints(r);
			}
			return r;
		}



		bool BroadcastPoint::displaysScenario( const Scenario& scenario ) const
		{
			ParametersMap fakeParameters;
			BOOST_FOREACH(const Alarm* it, scenario.getMessages())
			{
				if(it->isOnBroadcastPoint(*this, fakeParameters))
				{
					return true;
				}
			}
			return false;
		}
}	}

#endif // SYNTHESE_messages_BroadcastPoint_cpp__

