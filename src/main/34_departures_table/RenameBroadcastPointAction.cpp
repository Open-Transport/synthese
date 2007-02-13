
/** RenameBroadcastPointAction class implementation.
	@file RenameBroadcastPointAction.cpp

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

#include "34_departures_table/RenameBroadcastPointAction.h"
#include "34_departures_table/BroadcastPointTableSync.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/BroadcastPoint.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace departurestable
	{
		const string RenameBroadcastPointAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "nam";
		const string RenameBroadcastPointAction::PARAMETER_BROADCAST_ID = Action_PARAMETER_PREFIX + "bid";


		Request::ParametersMap RenameBroadcastPointAction::getParametersMap() const
		{
			Request::ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void RenameBroadcastPointAction::setFromParametersMap(Request::ParametersMap& map)
		{
			try
			{
				Request::ParametersMap::iterator it;

				it = map.find(PARAMETER_BROADCAST_ID);
				if (it == map.end())
					throw ActionException("Broadcast point not specified");

				_broadcastPoint = DeparturesTableModule::getBroadcastPoints().get(Conversion::ToLongLong(it->second));

				it = map.find(PARAMETER_NAME);
				if (it == map.end())
					throw ActionException("Name not specified");

				_name = it->second;

				if (_name.size() == 0)
					throw ActionException("Name must be non empty");
			}
			catch (BroadcastPoint::RegistryKeyException e)
			{
				throw ActionException("Specified broadcast point not found");
			}
		}

		RenameBroadcastPointAction::RenameBroadcastPointAction()
			: Action()
			, _broadcastPoint(NULL)
		{}

		void RenameBroadcastPointAction::run()
		{
			_broadcastPoint->setName(_name);
			BroadcastPointTableSync::save(_broadcastPoint);
		}
	}
}