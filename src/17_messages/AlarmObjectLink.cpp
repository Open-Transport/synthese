
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
#include "Registry.h"
#include "Factory.h"
#include "AlarmRecipient.h"

#include <boost/foreach.hpp>

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const std::string Registry<messages::AlarmObjectLink>::KEY("AlarmObjectLink");
	}

	namespace messages
	{
		Alarm* AlarmObjectLink::getAlarm() const
		{
			return _alarm;
		}

		std::string AlarmObjectLink::getRecipientKey() const
		{
			return _recipientKey;
		}



		void AlarmObjectLink::setAlarm(Alarm* alarm)
		{
			_alarm = alarm;
		}

		void AlarmObjectLink::setRecipientKey( const std::string& key )
		{
			_recipientKey = key;
		}



		AlarmObjectLink::AlarmObjectLink(
			RegistryKeyType key 
		):	Registrable(key),
			_alarm(NULL)
		{
		
		}



		void AlarmObjectLink::setObjectId( util::RegistryKeyType key )
		{
			_objectId = key;

			RegistryTableType tableId(decodeTableId(_objectId));
			BOOST_FOREACH(const Factory<AlarmRecipient>::ObjectsCollection::value_type& obj, Factory<AlarmRecipient>::GetNewCollection())
			{
				if(obj->getTableId() == tableId)
				{
					_recipientKey = obj->getFactoryKey();
					return;
				}
			}
			assert(false);
		}
	}
}
