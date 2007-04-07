
/** DeleteBroadcastPointAction class implementation.
	@file DeleteBroadcastPointAction.cpp

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

#include "34_departures_table/DeleteBroadcastPointAction.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/BroadcastPointTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace departurestable
	{
		const string DeleteBroadcastPointAction::PARAMETER_BROADCAST_ID = Action_PARAMETER_PREFIX + "bid";


		ParametersMap DeleteBroadcastPointAction::getParametersMap() const
		{
			ParametersMap map;
			if (_broadcastPoint)
				map.insert(make_pair(PARAMETER_BROADCAST_ID, Conversion::ToString(_broadcastPoint->getKey())));
			return map;
		}

		void DeleteBroadcastPointAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_BROADCAST_ID);
				if (it == map.end())
					throw ActionException("Broadcast point not specified");

				_broadcastPoint = DeparturesTableModule::getBroadcastPoints().get(Conversion::ToLongLong(it->second));

				 /// @todo Control of existence of a display screen. In this case throw an exception.
			}
			catch (BroadcastPoint::RegistryKeyException e)
			{
				throw ActionException("Specified broadcast point not found");
			}
		}

		DeleteBroadcastPointAction::DeleteBroadcastPointAction()
			: Action()
			, _broadcastPoint(NULL)
		{}

		void DeleteBroadcastPointAction::run()
		{
			BroadcastPointTableSync::remove(_broadcastPoint->getKey());
		}
	}
}