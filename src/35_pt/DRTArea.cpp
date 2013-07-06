
/** DRTArea class implementation.
	@file DRTArea.cpp

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

#include "DRTArea.hpp"
#include "PTModule.h"

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const std::string Registry<pt::DRTArea>::KEY("DRTArea");
	}

	namespace pt
	{
		DRTArea::DRTArea(
			const util::RegistryKeyType id,
			std::string name,
			Stops stops
		):	Registrable(id),
			Vertex(NULL, boost::shared_ptr<geos::geom::Point>()),
			_stops(stops)
		{}



		graph::GraphIdType DRTArea::getGraphType() const
		{
			return PTModule::GRAPH_ID;
		}



		std::string DRTArea::getRuleUserName() const
		{
			return "Zone de TAD";
		}



		bool DRTArea::contains( const StopArea& stopArea ) const
		{
			return _stops.find(&const_cast<StopArea&>(stopArea)) != _stops.end();
		}
}	}
