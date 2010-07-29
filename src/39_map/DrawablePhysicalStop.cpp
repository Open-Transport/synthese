
/** DrawablePhysicalStop class implementation.
	@file DrawablePhysicalStop.cpp

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

#include "DrawablePhysicalStop.h"

#include "PhysicalStop.h"
#include "AddressablePlace.h"
#include "StopArea.hpp"
#include "RGBColor.h"

#include "Map.h"
#include "PostscriptCanvas.h"

using synthese::pt::PhysicalStop;

namespace synthese
{
	using namespace geometry;
	using namespace pt;
	
	namespace map
	{
		DrawablePhysicalStop::DrawablePhysicalStop(
			const PhysicalStop* physicalStop
		): _physicalStopId(physicalStop->getKey())
			, _name(physicalStop->getConnectionPlace()->getName ())
			, _point (*physicalStop)
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
		
		
		const Point2D& 
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

