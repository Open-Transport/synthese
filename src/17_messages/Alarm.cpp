
/** Alarm class implementation.
	@file Alarm.cpp

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

#include "Alarm.h"

#include "AlarmObjectLink.h"
#include "AlarmRecipient.h"
#include "DBConstants.h"
#include "Factory.h"
#include "MessageAlternative.hpp"
#include "Scenario.h"
#include "AlarmTemplate.h"
#include "ParametersMap.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;

	template<> const Field SimpleObjectFieldDefinition<messages::Alarm>::FIELD = Field("alarm_id", SQL_INTEGER);
	template<> const std::string util::Registry<messages::Alarm>::KEY("Alarm");

	namespace messages
	{
		const string Alarm::DATA_MESSAGE_ID("message_id");
		const string Alarm::DATA_CONTENT("content");
		const string Alarm::DATA_PRIORITY("priority");
		const string Alarm::DATA_SCENARIO_ID("scenario_id");
		const string Alarm::DATA_SCENARIO_NAME("scenario_name");
		const string Alarm::DATA_TITLE("title");
		const string Alarm::DATA_DONE = "done";

		const string Alarm::TAG_MESSAGE_ALTERNATIVE = "message_alternative";
		const string Alarm::TAG_RECIPIENTS = "recipients";

		const string Alarm::ATTR_LINK_ID = "link_id";
		const string Alarm::ATTR_LINK_PARAMETER = "link_parameter";


		Alarm::Alarm(
			util::RegistryKeyType key,
			const Scenario* scenario
		):	Registrable(key),
			_level(ALARM_LEVEL_INFO),
			_scenario(scenario),
			_rawEditor(false),
			_done(true)
		{}



		Alarm::Alarm(
			const Alarm& source
		):	Registrable(0),
			_level(source._level),
			_shortMessage(source._shortMessage),
			_longMessage(source._longMessage),
			_scenario(source._scenario),
			_rawEditor(source._rawEditor),
			_done(source._done)
		{}



		Alarm::Alarm(
			const Alarm& source,
			const Scenario* scenario
		):	Registrable(0),
			_level(source._level),
			_shortMessage(source._shortMessage),
			_longMessage(source._longMessage),
			_scenario(scenario),
			_rawEditor(source._rawEditor),
			_done(source._done)
		{}



		Alarm::~Alarm()
		{}



		void Alarm::toParametersMap(
			util::ParametersMap& pm,
			bool withScenario,
			std::string prefix /*= std::string() */,
			bool withRecipients
		) const	{
			pm.insert(prefix + "roid", getKey()); // Backward compatibility, deprecated
			pm.insert(prefix + DATA_MESSAGE_ID, getKey());
			pm.insert(prefix + DATA_TITLE, getShortMessage());
			pm.insert(prefix + DATA_CONTENT, getLongMessage());
			pm.insert(prefix + DATA_DONE, getDone());
			pm.insert(prefix + DATA_PRIORITY, static_cast<int>(getLevel()));
			if(withScenario && getScenario())
			{
				pm.insert(prefix + DATA_SCENARIO_ID, getScenario()->getKey());
				pm.insert(prefix + DATA_SCENARIO_NAME, getScenario()->getName());
			}

			// Message alternatives
			BOOST_FOREACH(const MessageAlternatives::value_type& it, _messageAlternatives)
			{
				shared_ptr<ParametersMap> altPM(new ParametersMap);
				it.second->toParametersMap(*altPM);
				pm.insert(TAG_MESSAGE_ALTERNATIVE, altPM);
			}

			if(withRecipients)
			{
				shared_ptr<ParametersMap> recipientsPM(new ParametersMap);
				BOOST_FOREACH(const LinkedObjects::value_type& ar, _linkedObjects)
				{
					BOOST_FOREACH(const LinkedObjects::mapped_type::value_type& it, ar.second)
					{
						shared_ptr<ParametersMap> arPM(new ParametersMap);
						if(it->getObject())
						{
							it->getObject()->toParametersMap(*arPM);
						}
						else
						{
							arPM->insert(TABLE_COL_ID, 0);
						}
						arPM->insert(ATTR_LINK_ID, it->getKey());
						arPM->insert(ATTR_LINK_PARAMETER, it->getParameter());
						recipientsPM->insert(ar.first, arPM);
				}	}
				pm.insert(TAG_RECIPIENTS, recipientsPM);
			}
		}



		void Alarm::toParametersMap( util::ParametersMap& pm ) const
		{
			toParametersMap(pm, true);
		}
}	}
