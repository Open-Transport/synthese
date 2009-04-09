
/** TemporaryPlace class implementation.
	@file TemporaryPlace.cpp

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

#include "TemporaryPlace.h"

#include "Address.h"
#include "Road.h"
#include "RoadPlace.h"
#include "VertexAccessMap.h"
#include "AccessParameters.h"

namespace synthese
{
	using namespace graph;
	using namespace road;
	
	namespace env
	{
		TemporaryPlace::TemporaryPlace(
			const Road* road,
			double metricOffset
		):	Place ("", road->getRoadPlace()->getCity()),
			_road (road),
			_metricOffset (metricOffset)
		{
		}
		
		
		
		TemporaryPlace::~TemporaryPlace ()
		{
		}
		
		
		const Road* 
		TemporaryPlace::getRoad () const
		{
			return _road;
		}
		
		
		
		
		double 
		TemporaryPlace::getMetricOffset () const
		{
			return _metricOffset;
		}
		
		
		
		
		
		
		
		VertexAccess 
		TemporaryPlace::getVertexAccess (const AccessDirection& accessDirection,
						const AccessParameters& accessParameters,
						const Vertex* destination,
						const Vertex* origin) const
		{
			VertexAccess access(accessDirection);
			//access.approachDistance = _metricOffset - ((Address*) destination)->getMetricOffset ();
			access.approachTime = access.approachDistance / accessParameters.getApproachSpeed();
			return access;
		}
			
		
		
		
		
		
		
		
		void
		TemporaryPlace::getImmediateVertices (VertexAccessMap& result, 
							const AccessDirection& accessDirection,
							const AccessParameters& accessParameters,
							const Vertex* origin,
							bool returnAddresses,
							bool returnPhysicalStops) const
		{
			// Find closest addresses on both sides and run search from here.
			const Address* closestBefore = _road->findClosestAddressBefore (_metricOffset);
			const Address* closestAfter = _road->findClosestAddressAfter (_metricOffset);
		
			if (closestBefore != 0)
			{
//				VertexAccess access(accessDirection);
	//			access.approachDistance = _metricOffset - closestBefore->getMetricOffset ();
//				access.approachTime = access.approachDistance / accessParameters.getApproachSpeed();
				
//				result.insert (closestBefore, access);
			}
		
			if ( (closestAfter != 0) && (closestAfter != closestBefore) )
			{
	//			VertexAccess access(accessDirection);
		//		access.approachDistance = _metricOffset - closestAfter->getMetricOffset ();
			//	access.approachTime = access.approachDistance / accessParameters.getApproachSpeed();
				
		//		result.insert (closestAfter, access);
			}
		}
		
	

		uid TemporaryPlace::getId() const
		{
			return static_cast<uid>(UNKNOWN_VALUE);
		}
	}
}

