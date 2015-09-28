
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
#include "Alarm.h"

using namespace std;

namespace synthese
{
	using namespace messages;
	using namespace util;

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
					FIELD_DEFAULT_CONSTRUCTOR(Content),
					FIELD_VALUE_CONSTRUCTOR(LastUpdate, boost::posix_time::second_clock::local_time())
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



		void MessageAlternative::setContent(const std::string& content)
		{
			set<Content>(content);
			set<LastUpdate>(boost::posix_time::second_clock::local_time());
		}

}	}
