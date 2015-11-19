
/** OSMParser class header.
	@file OSMParser.hpp

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

#ifndef SYNTHESE_dataexchange_OSMEntityHandler_hpp__
#define SYNTHESE_dataexchange_OSMEntityHandler_hpp__

#include <string>


namespace geos
{
	namespace geom
	{
		class Geometry;
	}
}


namespace synthese
{
namespace data_exchange
{

typedef enum {
	TWO_WAYS,
	ONE_WAY,
	REVERSED_ONE_WAY
} TrafficDirection;


class OSMEntityHandler
{
protected:

	virtual ~OSMEntityHandler() {}

public:

	virtual void handleCity(const std::string& cityName, 
		                    const std::string& cityCode, 
		                    geos::geom::Geometry* boundary) = 0;

	virtual void handleRoad(const std::string& name,
							TrafficDirection trafficDirection,
							double maxSpeed,
							bool isDrivable,
							bool isBikable,
							bool isWalkable,
							geos::geom::Geometry* path) = 0;

};

}
}

#endif // SYNTHESE_dataexchange_OSMEntityHandler_hpp__
