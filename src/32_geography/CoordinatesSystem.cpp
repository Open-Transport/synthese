
/** CoordinatesSystem class implementation.
	@file CoordinatesSystem.cpp

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

#include "CoordinatesSystem.hpp"

using namespace std;

namespace synthese
{
	namespace geography
	{
		CoordinatesSystem::Map CoordinatesSystem::_coordinates_systems;



		void CoordinatesSystem::AddCoordinatesSystem(
			const string& code,
			const string& name,
			const string& tridentKey,
			const string& projSequence
		){
			_coordinates_systems[code] = CoordinatesSystem(code, name, tridentKey, projSequence);
		}



		const CoordinatesSystem& CoordinatesSystem::GetCoordinatesSystem( const std::string& key )
		{
			Map::const_iterator it(_coordinates_systems.find(key));
			if(it == _coordinates_systems.end())
			{
				throw NotFoundException(key);
			}
			return it->second;
		}



		void CoordinatesSystem::AddCoordinatesSystems()
		{
			AddCoordinatesSystem(
				"EPSG:2975",
				"RGR92 / UTM zone 40S (Ile de la Réunion)",
				"RGR92 UTM40s",
				"+proj=tmerc +towgs84=0.0000,0.0000,0.0000 +a=6378137.0000 +rf=298.2572221010000 +lat_0=0.000000000 +lon_0=57.000000000 +k_0=0.99960000 +x_0=500000.000 +y_0=10000000.000 +units=m +no_defs <>"
			);

			AddCoordinatesSystem(
				"EPSG:4326",
				"WGS84",
				"WGS84",
				"+proj=longlat +towgs84=0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.000000 +a=6378137.0000 +rf=298.2572221010000 +units=m +no_defs <>"
			);

			/// CH1903 / LV03 :
			/// http://www.swisstopo.admin.ch/internet/swisstopo/en/home/products/software/products/skripts.html
			AddCoordinatesSystem(
				"EPSG:21781",
				"CH1903 / LV03 (Suisse)",
				"CH1903LV03",
				"+proj=somerc +lat_0=46.95240555555556 +lon_0=7.439583333333333 +k_0=1 +x_0=600000 +y_0=200000 +ellps=bessel +towgs84=674.374,15.056,405.346,0,0,0,0 +units=m +no_defs <>"
			);

			/// Lambert II étendu :
			/// http://www.ign.fr/telechargement/MPro/geodesie/CIRCE/transfo.pdf
			/// http://www.ign.fr/telechargement/MPro/geodesie/CIRCE/NTG_80.pdf
			AddCoordinatesSystem(
				"EPSG:27572",
				"Lambert II etendu (France)",
				"LambertIIe",
				"+proj=lcc +towgs84=-168.0000,-60.0000,320.0000 +a=6378249.2000 +rf=293.4660210000000 +pm=2.337229167 +lat_0=46.800000000 +lon_0=0.000000000 +k_0=0.99987742 +lat_1=46.800000000 +x_0=600000.000 +y_0=2200000.000 +units=m +no_defs <>"
			);
		}
}	}
