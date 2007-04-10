
/** VinciUpdateBikeAction class implementation.
	@file VinciUpdateBikeAction.cpp

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

#include "30_server/ActionException.h"

#include "VinciUpdateBikeAction.h"
#include "VinciBikeTableSync.h"
#include "VinciBike.h"

#include "02_db/DBEmptyResultException.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	using namespace db;
	
	namespace vinci
	{
		const string VinciUpdateBikeAction::PARAMETER_BIKE_ID = Action_PARAMETER_PREFIX + "bid";
		const string VinciUpdateBikeAction::PARAMETER_NUMBER = Action_PARAMETER_PREFIX + "nu";
		const string VinciUpdateBikeAction::PARAMETER_MARKED_NUMBER = Action_PARAMETER_PREFIX + "mnu";


		ParametersMap VinciUpdateBikeAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_BIKE_ID, Conversion::ToString(_bike.get() ? _bike->getKey() : uid())));
			map.insert(make_pair(PARAMETER_NUMBER, _number));
			map.insert(make_pair(PARAMETER_MARKED_NUMBER, _markedNumber));
			return map;
		}

		void VinciUpdateBikeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_BIKE_ID);
				if (it == map.end())
					throw ActionException("Parameter bike not found");
				_bike = VinciBikeTableSync::get(Conversion::ToLongLong(it->second));
				
				it = map.find(PARAMETER_NUMBER);
				if (it == map.end())
					throw ActionException("Parameter number not found");
				_number = it->second;

				it = map.find(PARAMETER_MARKED_NUMBER);
				if (it == map.end())
					throw ActionException("Parameter number not found");
				_markedNumber = it->second;
			}
			catch(DBEmptyResultException<VinciBike> e)
			{
				throw ActionException("Le vélo spécifié est introuvable");
			}
		}

		void VinciUpdateBikeAction::run()
		{
			_bike->setNumber(_number);
			_bike->setMarkedNumber(_markedNumber);
			VinciBikeTableSync::save(_bike.get());
		}
	}
}
