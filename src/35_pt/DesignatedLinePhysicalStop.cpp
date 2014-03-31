
/** DesignatedLinePhysicalStop class implementation.
	@file DesignatedLinePhysicalStop.cpp

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

#include "DesignatedLinePhysicalStop.hpp"

#include "LineStop.h"
#include "StopPoint.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// Constructor.
		/// @param lineStop object of the database generating this object
		/// @pre lineStop.get<LineNode>() is a StopPoint
		DesignatedLinePhysicalStop::DesignatedLinePhysicalStop(
			LineStop& lineStop
		):	Registrable(0),
			LinePhysicalStop(
				lineStop.get<Line>() ? &*lineStop.get<Line>() : NULL,
				lineStop.get<RankInPath>(),
				lineStop.get<MetricOffsetField>(),
				lineStop.get<LineNode>() ? dynamic_cast<StopPoint*>(&*lineStop.get<LineNode>()) : NULL,
				&lineStop
			)
		{}



		DesignatedLinePhysicalStop::~DesignatedLinePhysicalStop()
		{
			unlink();
		}



		bool DesignatedLinePhysicalStop::isDepartureAllowed() const
		{
			return _lineStop->get<IsDeparture>();
		}



		bool DesignatedLinePhysicalStop::isArrivalAllowed() const
		{
			return _lineStop->get<IsArrival>();
		}



		boost::shared_ptr<geos::geom::LineString> DesignatedLinePhysicalStop::getRealGeometry() const
		{
			boost::shared_ptr<geos::geom::LineString> tmpGeom(
				_lineStop->get<LineStringGeometry>()
			);

			if(tmpGeom && !tmpGeom->isEmpty())
			{
				return tmpGeom;
			}
			else
			{
				return graph::Edge::getRealGeometry();
			}
		}
}	}
