
/** MessagesModule class implementation.
	@file MessagesModule.cpp

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

#include "17_messages/MessagesModule.h"

using namespace std;

namespace synthese
{
	namespace messages
	{
		Alarm::Registry MessagesModule::_alarms;
		Scenario::Registry MessagesModule::_scenarii;

		Alarm::Registry& 
			MessagesModule::getAlarms ()
		{
			return _alarms;
		}

		Scenario::Registry& MessagesModule::getScenarii()
		{
			return _scenarii;
		}

		std::map<uid, std::string> MessagesModule::getScenariiLabels( bool withAll /*= false*/ )
		{
			map<uid,string> m;
			if (withAll)
				m.insert(make_pair(0, "(tous)"));
			for(Scenario::Registry::const_iterator it = _scenarii.begin(); it != _scenarii.end(); ++it)
				m.insert(make_pair(it->first, it->second->getName()));
			return m;

		}

		void initialize()
		{

		}
	}
}
