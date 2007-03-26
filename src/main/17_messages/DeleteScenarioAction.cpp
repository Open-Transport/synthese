
/** DeleteScenarioAction class implementation.
	@file DeleteScenarioAction.cpp

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

#include "DeleteScenarioAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace messages
	{
		/// @todo Parameters constants definition
		// const string DeleteScenarioAction::PARAMETER_xxx = Action_PARAMETER_PREFIX + "xxx";


		Request::ParametersMap DeleteScenarioAction::getParametersMap() const
		{
			Request::ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void DeleteScenarioAction::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;

			// it = map.find(PARAMETER_xxx);
			// if (it == map.end())
			//	throw ActionException("Parameter xxx not found");
			//
			// _xxx = it->second;
			// map.erase(it);
			// if (_xxx <= 0)
			//	throw ActionException("Bad value for xxx parameter ");	
			// 
		}

		DeleteScenarioAction::DeleteScenarioAction()
			: Action()
			/// @todo Put here other parameters initialization
		{}

		void DeleteScenarioAction::run()
		{
		}
	}
}
