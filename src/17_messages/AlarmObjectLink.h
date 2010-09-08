
/** AlarmObjectLink class header.
	@file AlarmObjectLink.h

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

#ifndef SYNTHESE_AlarmObjectLink_h__
#define SYNTHESE_AlarmObjectLink_h__

#include <string>

#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace messages
	{
		class Alarm;

		/** Link between an alarm and an object.
			@ingroup m17

			@note This class is used only to permit the remove hook
		*/
		class AlarmObjectLink
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<AlarmObjectLink>	Registry;

		private:
			util::RegistryKeyType	_objectId;
			Alarm*			_alarm;
			std::string		_recipientKey;

		public:
			AlarmObjectLink(util::RegistryKeyType key = 0);

			util::RegistryKeyType getObjectId() const { return _objectId; }
			Alarm* getAlarm() const;
			std::string getRecipientKey() const;

			void setObjectId(util::RegistryKeyType key) { _objectId = key; }
			void setAlarm(Alarm* value);
			void setRecipientKey(const std::string& key);
		};
	}
}

#endif // SYNTHESE_AlarmObjectLink_h__
