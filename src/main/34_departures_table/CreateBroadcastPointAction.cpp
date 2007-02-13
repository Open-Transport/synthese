
/** CreateBroadcastPointAction class implementation.
	@file CreateBroadcastPointAction.cpp

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

#include "15_env/ConnectionPlace.h"
#include "15_env/PhysicalStop.h"
#include "15_env/EnvModule.h"

#include "30_server/ActionException.h"

#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/BroadcastPointTableSync.h"
#include "34_departures_table/CreateBroadcastPointAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace env;
	
	namespace departurestable
	{
		const string CreateBroadcastPointAction::PARAMETER_NAME = Action::PARAMETER_PREFIX + "nam";
		const string CreateBroadcastPointAction::PARAMETER_PHYSICAL_ID = Action::PARAMETER_PREFIX + "pid";
		const string CreateBroadcastPointAction::PARAMETER_PLACE_ID = Action::PARAMETER_PREFIX + "lid";


		Request::ParametersMap CreateBroadcastPointAction::getParametersMap() const
		{
			Request::ParametersMap map;
			return map;
		}

		void CreateBroadcastPointAction::setFromParametersMap(Request::ParametersMap& map)
		{
			try
			{
				Request::ParametersMap::iterator it;

				it = map.find(PARAMETER_PLACE_ID);
				if (it == map.end())
					throw ActionException("Place not specified");
				_place = EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(it->second));

				it = map.find(PARAMETER_PHYSICAL_ID);
				if (it != map.end())
				{
					_physicalStop = EnvModule::getPhysicalStops().get(Conversion::ToLongLong(it->second));
					if (_physicalStop->getConnectionPlace() != _place)
						throw ActionException("Bad physical stop : does not belong to the specified place");
				}

				it = map.find(PARAMETER_NAME);
				if (it != map.end())
				{
					_name = it->second;
					map.erase(it);
				}

				if (_name.size() == 0 && _physicalStop == NULL)
					throw ActionException("Physical stop or broadcast point name not specified");

			}
			catch (ConnectionPlace::RegistryKeyException e)
			{
				throw ActionException("Specified place not found");
			}
			catch (PhysicalStop::RegistryKeyException e)
			{
				throw ActionException("Specified physical stop not found");
			}

		}

		void CreateBroadcastPointAction::run()
		{
			BroadcastPoint* bp = new BroadcastPoint;
			bp->setConnectionPlace(_place);
			bp->setPhysicalStop(_physicalStop);
			bp->setName(_name);
			BroadcastPointTableSync::save(bp);
			delete bp;
		}

		CreateBroadcastPointAction::CreateBroadcastPointAction()
			: Action()
			, _physicalStop(NULL)
			, _place(NULL)
		{
			
		}
	}
}