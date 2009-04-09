
/** Road place class implementation.
	@file RoadPlace.cpp

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

#include <boost/foreach.hpp>

#include "RoadPlace.h"
#include "Road.h"
#include "RoadChunk.h"
#include "Registry.h"
#include "Address.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace env;

	namespace util
	{
		template<> const string Registry<road::RoadPlace>::KEY("RoadPlace");
	}

	namespace road
	{
		RoadPlace::RoadPlace(
			RegistryKeyType key,
			string name,
			const City* city
		):	Registrable(key),
			AddressablePlace (name, city)
		{
		}
		
		
		
		const RoadPlace::Roads& RoadPlace::getRoads(
		) const {
			return _roads;
		}
		
		
		
		void RoadPlace::addRoad(
			const Road* road
		){
			_roads.insert(road);
		}
		
		
		
		void RoadPlace::removeRoad(
			const Road* road
		){
			Roads::iterator it(_roads.find(road));
			if(it != _roads.end()) _roads.erase(it);
		}
		
		
		
		void RoadPlace::getImmediateVertices(
			VertexAccessMap& result,
			const AccessDirection& accessDirection,
			const AccessParameters& accessParameters,
			SearchAddresses returnAddresses,
			SearchPhysicalStops returnPhysicalStops,
			const Vertex* origin /*= 0 */
		) const {
			BOOST_FOREACH(const Road* road, _roads)
			{
				BOOST_FOREACH(const Edge* edge, road->getEdges())
				{
					const Address* address(static_cast<const RoadChunk*>(edge)->getFromAddress());
					address->getAddressablePlace()->getImmediateVertices(
						result, accessDirection, accessParameters,
						returnAddresses, returnPhysicalStops, origin
					);
				}
			}
		}
 
	}
}
