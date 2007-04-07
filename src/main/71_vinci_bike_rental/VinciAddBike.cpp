
/** VinciAddBike class implementation.
	@file VinciAddBike.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#include "30_server/Request.h"

#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciAddBike.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace vinci
	{
		const string VinciAddBike::PARAMETER_NUMBER = Action_PARAMETER_PREFIX + "nu";
		const string VinciAddBike::PARAMETER_MARKED_NUMBER = Action_PARAMETER_PREFIX + "mn";


		ParametersMap VinciAddBike::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_NUMBER, _number));
			map.insert(make_pair(PARAMETER_MARKED_NUMBER, _marked_number));
			return map;
		}

		void VinciAddBike::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			it = map.find(PARAMETER_NUMBER);
			if (it != map.end())
			{
				_number = it->second;
			}

			it = map.find(PARAMETER_MARKED_NUMBER);
			if (it != map.end())
			{
				_marked_number = it->second;
			}
		}

		void VinciAddBike::run()
		{
			VinciBike* bike = new VinciBike;
			bike->setNumber(_number);
			bike->setMarkedNumber(_marked_number);
			VinciBikeTableSync::save(bike);
			_request->setObjectId(bike->getKey());
		}
	}
}
