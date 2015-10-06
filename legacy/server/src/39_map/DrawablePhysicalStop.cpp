
/** DrawablePhysicalStop class implementation.
	@file DrawablePhysicalStop.cpp

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

#include "DrawablePhysicalStop.h"

#include "StopPoint.hpp"
#include "StopArea.hpp"
#include "RGBColor.h"

#include "Map.h"
#include "PostscriptCanvas.h"

using namespace geos::geom;

namespace synthese
{
	using namespace pt;

	namespace map
	{
		DrawablePhysicalStop::DrawablePhysicalStop(
			const StopPoint* physicalStop
		): _physicalStopId(physicalStop->getKey())
			, _name(physicalStop->getConnectionPlace()->getName ())
			, _point (*physicalStop->getGeometry()->getCoordinate())
		{
		}


		DrawablePhysicalStop::~DrawablePhysicalStop ()
		{
		}



		int
		DrawablePhysicalStop::getPhysicalStopId () const
		{
			return _physicalStopId;
		}


		const std::string&
		DrawablePhysicalStop::getName () const
		{
			return _name;
		}


		const Coordinate&
		DrawablePhysicalStop::getPoint () const
		{
			return _point;
		}



		void
		DrawablePhysicalStop::prepare (Map& map)
		{
			_point = map.toOutputFrame (_point);
		}
	}
}
