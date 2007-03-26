
/** AlarmRemoveLinkAction class implementation.
	@file AlarmRemoveLinkAction.cpp

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

#include "01_util/Conversion.h"

#include "30_server/ActionException.h"

#include "17_messages/AlarmRemoveLinkAction.h"
#include "17_messages/AlarmObjectLinkTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	
	namespace messages
	{
		const string AlarmRemoveLinkAction::PARAMETER_LINK_ID = Action_PARAMETER_PREFIX + "araid";
		

		Request::ParametersMap AlarmRemoveLinkAction::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_LINK_ID, Conversion::ToString(_id)));
			return map;
		}

		void AlarmRemoveLinkAction::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;

			it = map.find(PARAMETER_LINK_ID);
			if (it == map.end())
				throw ActionException("Link ID not found");
			_id = Conversion::ToLongLong(it->second);
		}

		AlarmRemoveLinkAction::AlarmRemoveLinkAction()
			: Action()
		{}

		void AlarmRemoveLinkAction::run()
		{
			AlarmObjectLinkTableSync::remove(_id);
		}
	}
}
