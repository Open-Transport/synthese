
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

#include "Object.hpp"

#include "StringField.hpp"

namespace synthese
{
	namespace messages
	{
		class Alarm;
		class AlarmRecipient;

		FIELD_STRING(RecipientKey)
		FIELD_ID(ObjectId)
		FIELD_POINTER(LinkedAlarm, Alarm)
		FIELD_STRING(Parameter)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(RecipientKey),
			FIELD(ObjectId),
			FIELD(LinkedAlarm),
			FIELD(Parameter)
		> AlarmObjectLinkSchema;

		class AlarmObjectLink:
			public virtual Object<AlarmObjectLink, AlarmObjectLinkSchema>
		{
		private:
			std::auto_ptr<AlarmRecipient> _recipient;

		public:
			AlarmObjectLink(util::RegistryKeyType key = 0);

			util::RegistryKeyType getObjectId() const { return get<ObjectId>(); }
			Alarm* getAlarm() const;
			const AlarmRecipient* getRecipient() const { return _recipient.get(); }
			const std::string& getParameter() const { return get<Parameter>(); }

			void setObjectId(util::RegistryKeyType value){ set<ObjectId>(value); }
			void setAlarm(Alarm* value);
			void setRecipient(const std::string& key);
			void setParameter(const std::string& value){ set<Parameter>(value); }

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_AlarmObjectLink_h__
