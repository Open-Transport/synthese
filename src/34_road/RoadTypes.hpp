
/** RoadTypes class header.
	@file RoadTypes.hpp

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

#ifndef SYNTHESE_road_RoadTypes_hpp__
#define SYNTHESE_road_RoadTypes_hpp__

#include <boost/optional.hpp>
#include <map>

namespace synthese
{
	namespace road
	{
		typedef unsigned int HouseNumber;

		typedef enum {
			ROAD_TYPE_UNKNOWN,
			ROAD_TYPE_MOTORWAY, /* autoroute */
			ROAD_TYPE_MEDIANSTRIPPEDROAD,  /* route a chaussees separees (terre plein) */
			ROAD_TYPE_PRINCIPLEAXIS, /* axe principal (au sens rue) */
			ROAD_TYPE_SECONDARYAXIS, /* axe principal (au sens rue) */
			ROAD_TYPE_BRIDGE, /* pont */
			ROAD_TYPE_STREET, /* rue */
			ROAD_TYPE_PEDESTRIANSTREET, /* rue pietonne */
			ROAD_TYPE_ACCESSROAD, /* bretelle */
			ROAD_TYPE_PRIVATEWAY, /* voie privee */
			ROAD_TYPE_PEDESTRIANPATH, /* chemin pieton */
			ROAD_TYPE_TUNNEL, /* tunnel */
			ROAD_TYPE_HIGHWAY, /* route secondaire */
			ROAD_TYPE_STEPS, /* steps */
			ROAD_TYPE_SERVICE
		} RoadType;

		typedef enum {
			ODD_NUMBERS = 'O',
			EVEN_NUMBERS = 'E',
			ALL_NUMBERS = 'A'
		} HouseNumberingPolicy;

		typedef boost::optional<std::pair<HouseNumber, HouseNumber> > HouseNumberBounds;

		struct CarSpeedFactors {
			typedef std::map<RoadType, double> SpeedFactors;
			SpeedFactors _factors;

			CarSpeedFactors() : _factors()
			{
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_UNKNOWN, 0.7));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_MOTORWAY, 0.9));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_MEDIANSTRIPPEDROAD, 0.85));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_PRINCIPLEAXIS, 0.8));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_SECONDARYAXIS, 0.6));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_BRIDGE, 0.5));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_STREET, 0.5));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_PEDESTRIANSTREET, 0.5));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_ACCESSROAD, 0.5));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_PRIVATEWAY, 0.2));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_PEDESTRIANPATH, 0.2));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_TUNNEL, 0.5));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_HIGHWAY, 0.8));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_STEPS, 0.5));
				_factors.insert(SpeedFactors::value_type(ROAD_TYPE_SERVICE, 0.2));
			}
		};
}	}

#endif // SYNTHESE_road_RoadTypes_hpp__

