
/** AlarmObjectLink class implementation.
	@file AlarmObjectLink.cpp

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

#include "AlarmObjectLink.h"

namespace synthese
{

	namespace util
	{
		template<> typename Registrable<uid,messages::AlarmObjectLink>::Registry Registrable<uid,messages::AlarmObjectLink>::_registry;
	}

	namespace messages
	{


		uid AlarmObjectLink::getObjectId() const
		{
			return _objectId;
		}

		uid AlarmObjectLink::getAlarmId() const
		{
			return _alarmId;
		}

		std::string AlarmObjectLink::getRecipientKey() const
		{
			return _recipientKey;
		}

		void AlarmObjectLink::setObjectId( uid key )
		{
			_objectId = key;
		}

		void AlarmObjectLink::setAlarmId( uid key )
		{
			_alarmId = key;
		}

		void AlarmObjectLink::setRecipientKey( const std::string& key )
		{
			_recipientKey = key;
		}
	}
}
