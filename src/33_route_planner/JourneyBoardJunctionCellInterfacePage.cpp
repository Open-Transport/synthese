
/** JourneyBoardJunctionCellInterfacePage class implementation.
	@file JourneyBoardJunctionCellInterfacePage.cpp

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

#include "JourneyBoardJunctionCellInterfacePage.h"
#include "SentAlarm.h"
#include "Place.h"
#include "Road.h"
#include "Conversion.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace messages;
	using namespace env;
	using namespace util;
	using namespace road;

	template<> const string util::FactorableTemplate<InterfacePage,routeplanner::JourneyBoardJunctionCellInterfacePage>::FACTORY_KEY("journey_board_junction_cell");
	
	namespace routeplanner
	{
		void JourneyBoardJunctionCellInterfacePage::display( 
		    ostream& stream
		    , const Place* place
		    , const SentAlarm* alarm
		    , bool color
			, const Road* road
			, int distance
		    , const server::Request* request /*= NULL */
		) const	{
			ParametersVector pv;
			pv.push_back(Conversion::ToString(place->getKey()));
			pv.push_back(alarm == NULL ? "" : alarm->getLongMessage());
			pv.push_back(alarm == NULL ? "" : alarm->getLongMessage());
			pv.push_back(Conversion::ToString(color));
			pv.push_back(road ? road->getName() : string());
			pv.push_back(Conversion::ToString(distance));

			VariablesMap vm;

			InterfacePage::_display(stream, pv, vm, NULL, request);

		}



		JourneyBoardJunctionCellInterfacePage::JourneyBoardJunctionCellInterfacePage()
			: Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
