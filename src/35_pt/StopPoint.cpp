
/** StopPoint class implementation.
	@file StopPoint.cpp

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

#include "StopPoint.hpp"
#include "Registry.h"
#include "PTModule.h"
#include "StopArea.hpp"
#include "ReverseRoadChunk.hpp"

#include <boost/date_time/time_duration.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace pt;
	using namespace graph;
	using namespace impex;

	namespace util
	{
		template<> const string Registry<StopPoint>::KEY("StopPoint");
	}

	namespace pt
	{
		StopPoint::StopPoint(
			RegistryKeyType id
			, string name
			, const StopArea* place,
			shared_ptr<Point> geometry
		):	Registrable(id),
			Vertex(place, geometry),
			Importable(),
			Named(name)
		{
		}



		StopPoint::~StopPoint()
		{

		}



		const StopArea* StopPoint::getConnectionPlace() const
		{
			return static_cast<const StopArea*>(Vertex::getHub());
		}



		graph::GraphIdType StopPoint::getGraphType() const
		{
			return PTModule::GRAPH_ID;
		}



		graph::VertexAccess StopPoint::getVertexAccess( const road::Crossing& crossing ) const
		{
			if(_projectedPoint.getRoadChunk())
			{
				if(_projectedPoint.getRoadChunk()->getFromCrossing() == &crossing)
				{
					return VertexAccess(minutes(_projectedPoint.getMetricOffset() / 50), _projectedPoint.getMetricOffset());
				}
				if(	_projectedPoint.getRoadChunk()->getReverseRoadChunk() &&
					_projectedPoint.getRoadChunk()->getReverseRoadChunk()->getFromCrossing() == &crossing
				){
					return VertexAccess(
						minutes((_projectedPoint.getRoadChunk()->getEndMetricOffset() - _projectedPoint.getRoadChunk()->getMetricOffset() - _projectedPoint.getMetricOffset()) / 50),
						_projectedPoint.getRoadChunk()->getEndMetricOffset() - _projectedPoint.getRoadChunk()->getMetricOffset() - _projectedPoint.getMetricOffset()
					);
				}
			}
			return VertexAccess();
		}
	}
}

