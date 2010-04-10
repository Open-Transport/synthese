
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
#include "RoadPlace.h"
#include "GeoPoint.h"
#include "Vertex.h"
#include "Projection.h"
#include "Hub.h"
#include "NamedPlace.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace messages;
	using namespace util;
	using namespace geography;
	using namespace road;
	using namespace graph;
	

	template<> const string util::FactorableTemplate<InterfacePage,routeplanner::JourneyBoardJunctionCellInterfacePage>::FACTORY_KEY("journey_board_junction_cell");
	
	namespace routeplanner
	{
		const string JourneyBoardJunctionCellInterfacePage::DATA_REACHED_PLACE_NAME("reached_place_name");
		const string JourneyBoardJunctionCellInterfacePage::DATA_ODD_ROW("is_odd_row");
		const string JourneyBoardJunctionCellInterfacePage::DATA_ROAD_NAME("road_name");
		const string JourneyBoardJunctionCellInterfacePage::DATA_LENGTH("length");

		void JourneyBoardJunctionCellInterfacePage::display( 
		    ostream& stream
		    , const Vertex& vertex
		    , const SentAlarm* alarm
		    , bool color
			, const Road* road
			, double distance
		    , const server::Request* request /*= NULL */
		) const	{

			GeoPoint point(WGS84FromLambert(vertex));

			ParametersVector pv;
			pv.push_back(lexical_cast<string>(point.getLongitude()));
			pv.push_back(lexical_cast<string>(point.getLatitude()));
			pv.push_back(lexical_cast<string>(dynamic_cast<const NamedPlace*>(vertex.getHub()) != NULL));
			pv.push_back(lexical_cast<string>(color));
			pv.push_back((road && road->getRoadPlace()) ? road->getRoadPlace()->getName() : string());
			pv.push_back(lexical_cast<string>(distance));

			VariablesMap vm;

			InterfacePage::_display(stream, pv, vm, NULL, request);

		}



		JourneyBoardJunctionCellInterfacePage::JourneyBoardJunctionCellInterfacePage()
			: Registrable(0)
		{

		}
	}
}
