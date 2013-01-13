
/** AlarmObjectLink class header.
	@file AlarmObjectLink.h

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
		class AlarmObjectLink:
			public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<AlarmObjectLink>	Registry;

		private:
			util::Registrable*	_object;
			Alarm*			_alarm;
			std::string		_recipientKey;
			std::string		_parameter;

		public:
			AlarmObjectLink(util::RegistryKeyType key = 0);

			util::Registrable* getObject() const { return _object; }
			Alarm* getAlarm() const { return _alarm; }
			const std::string& getRecipientKey() const { return _recipientKey; }
			const std::string& getParameter() const { return _parameter; }

			void setObject(util::Registrable* value){ _object = value; }
			void setAlarm(Alarm* value){ _alarm = value; }
			void setRecipientKey(const std::string& value){ _recipientKey = value; }
			void setParameter(const std::string& value){ _parameter = value; }
		};
}	}

#endif // SYNTHESE_AlarmObjectLink_h__
