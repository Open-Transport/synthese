
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
		class AlarmRecipient;

		class AlarmObjectLink:
			public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<AlarmObjectLink>	Registry;

		private:
			util::RegistryKeyType	_objectId;
			Alarm*			_alarm;
			std::auto_ptr<AlarmRecipient> _recipient;
			std::string		_parameter;

		public:
			AlarmObjectLink(util::RegistryKeyType key = 0);

			util::RegistryKeyType getObjectId() const { return _objectId; }
			Alarm* getAlarm() const { return _alarm; }
			const AlarmRecipient* getRecipient() const { return _recipient.get(); }
			const std::string& getParameter() const { return _parameter; }

			void setObjectId(util::RegistryKeyType value){ _objectId = value; }
			void setAlarm(Alarm* value){ _alarm = value; }
			void setRecipient(const std::string& key);
			void setParameter(const std::string& value){ _parameter = value; }

			void toParametersMap(
				util::ParametersMap& pm,
				bool withAdditionalParameters,
				boost::logic::tribool withFiles,
				std::string prefix
			) const;

			bool loadFromRecord(
				const Record& record,
				util::Env& env
			);
		};
}	}

#endif // SYNTHESE_AlarmObjectLink_h__
