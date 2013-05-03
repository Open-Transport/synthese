
/** AlarmObjectLink class implementation.
	@file AlarmObjectLink.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
		AlarmObjectLink::AlarmObjectLink(
			RegistryKeyType key
		):	Registrable(key),
			_objectId(0),
			_alarm(NULL)
		{}



		void AlarmObjectLink::setRecipient( const std::string& key )
		{
			_recipient.reset(
				Factory<AlarmRecipient>::create(key)
			);
		}
}	}

/** @class AlarmObjectLink
	@ingroup m17

	Link between an alarm and an object.
*/
