
/** JourneyBoardStopCellInterfacePage class implementation.
	@file JourneyBoardStopCellInterfacePage.cpp

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

#include "33_route_planner/JourneyBoardStopCellInterfacePage.h"

#include "17_messages/SentAlarm.h"

namespace synthese
{
	using namespace messages;
	using namespace interfaces;

	namespace routeplanner
	{
		void JourneyBoardStopCellInterfacePage::display(
			std::ostream& stream
			, bool isItArrival
			, const SentAlarm* alarm
			, bool isItTerminus, const std::string& stopName, bool color
			, const synthese::time::Hour& firstTime, const synthese::time::Hour& lastTime, const server::Request* request /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back( synthese::util::Conversion::ToString( isItArrival ));
			pv.push_back( alarm->getLongMessage() );
			pv.push_back( synthese::util::Conversion::ToString( alarm->getLevel() ));
			pv.push_back( synthese::util::Conversion::ToString( isItTerminus ));
			pv.push_back( stopName );
			pv.push_back( synthese::util::Conversion::ToString( color ));
			pv.push_back( firstTime.isUnknown() ? "" : firstTime.toInternalString() );
			pv.push_back( lastTime.isUnknown() ? "" : lastTime.toInternalString() );

			InterfacePage::display( stream, pv, VariablesMap(), NULL, request );
		}
	}
}
