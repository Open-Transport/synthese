
/** MessageAlternative class implementation.
	@file MessageAlternative.cpp

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

#include "MessageAlternative.hpp"

#include "AlarmTableSync.h"
#include "MessageTypeTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace messages;
	using namespace util;
	using namespace db;

	CLASS_DEFINITION(MessageAlternative, "t102_message_types", 102)
	FIELD_DEFINITION_OF_OBJECT(MessageAlternative, "message_alternative_id", "message_alternative_ids")

	FIELD_DEFINITION_OF_TYPE(Content, "content", SQL_TEXT)

	namespace messages
	{
		MessageAlternative::MessageAlternative(
			util::RegistryKeyType id /*= 0 */
		):	Registrable(id),
			Object<MessageAlternative, MessageAlternativeRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Alarm),
					FIELD_DEFAULT_CONSTRUCTOR(MessageType),
					FIELD_DEFAULT_CONSTRUCTOR(Content)
			)	)
		{}




		void MessageAlternative::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(	get<Alarm>() &&
				get<MessageType>()
			){
				Alarm::MessageAlternatives v(get<Alarm>()->getMessageAlternatives());
				v.insert(make_pair(&(*get<MessageType>()), this));
				get<Alarm>()->setMessageAlternatives(v);
			}
		}



		void MessageAlternative::unlink()
		{
			if(	get<Alarm>() &&
				get<MessageType>()
			){
				Alarm::MessageAlternatives v(get<Alarm>()->getMessageAlternatives());
				v.erase(&(*get<MessageType>()));
				get<Alarm>()->setMessageAlternatives(v);
			}
		}

		void MessageAlternative::toParametersMap( util::ParametersMap& pm, bool withScenario, boost::logic::tribool withRecipients /*= false */, std::string prefix /*= std::string()*/ ) const
		{
			// Inter synthese package
			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + SimpleObjectFieldDefinition<Alarm>::FIELD.name,
				get<Alarm>() ? get<Alarm>()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + SimpleObjectFieldDefinition<MessageType>::FIELD.name,
				get<MessageType>() ? get<MessageType>()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + SimpleObjectFieldDefinition<Content>::FIELD.name,
				get<Content>()
			);
		}

		bool MessageAlternative::loadFromRecord(
			const Record& record,
			util::Env& env
		){
			bool result(false);

			// Alarm
			if(record.isDefined(SimpleObjectFieldDefinition<Alarm>::FIELD.name))
			{
				Alarm* value(NULL);
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						SimpleObjectFieldDefinition<Alarm>::FIELD.name,
						0
				)	);
				if(id > 0)
				{
					try
					{
						value = AlarmTableSync::GetEditable(id, env).get();
					}
					catch(ObjectNotFoundException<Alarm>&)
					{
						Log::GetInstance().warn("No such alarm in message alternative "+ lexical_cast<string>(getKey()));
					}
				}
				if(value != get<Alarm>().get_ptr())
				{
					set<Alarm>(*value);
					result = true;
				}
			}

			// Message type
			if(record.isDefined(SimpleObjectFieldDefinition<MessageType>::FIELD.name))
			{
				MessageType* value(NULL);
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						SimpleObjectFieldDefinition<MessageType>::FIELD.name,
						0
				)	);
				if(id > 0)
				{
					try
					{
						value = MessageTypeTableSync::GetEditable(id, env).get();
					}
					catch(ObjectNotFoundException<MessageType>&)
					{
						Log::GetInstance().warn("No such message type in message alternative "+ lexical_cast<string>(getKey()));
					}
				}
				if(value != get<MessageType>().get_ptr())
				{
					set<MessageType>(*value);
					result = true;
				}
			}

			// Content
			if(record.isDefined(SimpleObjectFieldDefinition<Content>::FIELD.name))
			{
				string value(
					record.get<string>(SimpleObjectFieldDefinition<Content>::FIELD.name)
				);
				if(value != get<Content>())
				{
					set<Content>(value);
					result = true;
				}
			}

			return result;
		}
}	}
