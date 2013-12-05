
/** AlarmTemplate class implementation.
	@file AlarmTemplate.cpp

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

#include "AlarmTemplate.h"

#include "AlarmTableSync.h"
#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"
#include "MessagesSectionTableSync.hpp"
#include "SentAlarm.h"
#include "ScenarioTableSync.h"
#include "ScenarioTemplate.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace impex;

	namespace messages
	{
		AlarmTemplate::~AlarmTemplate()
		{}



		AlarmTemplate::AlarmTemplate(
			util::RegistryKeyType key,
			const ScenarioTemplate* scenario
		):	Registrable(key),
			Alarm(key, scenario)
		{}



		AlarmTemplate::AlarmTemplate(
			const AlarmTemplate& source
		):	Registrable(0),
			Alarm(source)
		{}



		AlarmTemplate::AlarmTemplate(
			const ScenarioTemplate& scenario,
			const Alarm& source
		):	Registrable(0),
			Alarm(source, &scenario, NULL)
		{}



		const ScenarioTemplate* AlarmTemplate::getScenario() const
		{
			return static_cast<const ScenarioTemplate*>(Alarm::getScenario());
		}

		void AlarmTemplate::toParametersMap( util::ParametersMap& pm, bool withScenario, boost::logic::tribool withRecipients /*= false */, std::string prefix /*= std::string()*/ ) const
		{
			// Inter synthese package
			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + AlarmTableSync::COL_IS_TEMPLATE,
				true
			);
			pm.insert(
				prefix + AlarmTableSync::COL_LEVEL,
				getLevel()
			);
			pm.insert(
				prefix + AlarmTableSync::COL_SHORT_MESSAGE,
				getShortMessage()
			);
			pm.insert(
				prefix + AlarmTableSync::COL_LONG_MESSAGE,
				getLongMessage()
			);
			pm.insert(
				prefix + AlarmTableSync::COL_SCENARIO_ID,
				getScenario() ? getScenario()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + AlarmTableSync::COL_TEMPLATE_ID,
				RegistryKeyType(0)
			);
			pm.insert(
				prefix + AlarmTableSync::COL_RAW_EDITOR,
				getRawEditor()
			);
			pm.insert(
				prefix + AlarmTableSync::COL_DONE,
				getDone()
			);
			pm.insert(
				prefix + AlarmTableSync::COL_MESSAGES_SECTION_ID,
				getSection()
			);
			pm.insert(
				prefix + AlarmTableSync::COL_CALENDAR_ID,
				getCalendar() ? getCalendar()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + AlarmTableSync::COL_DATASOURCE_LINKS,
				synthese::DataSourceLinks::Serialize(getDataSourceLinks())
			);

			// CMS output
			Alarm::toParametersMapAlarm(pm, withScenario, withRecipients, prefix);
		}

		bool AlarmTemplate::loadFromRecord(
			const Record& record,
			util::Env& env
		){
			bool result(false);

			// Level
			if(record.isDefined(AlarmTableSync::COL_LEVEL))
			{
				AlarmLevel value(
					static_cast<AlarmLevel>(record.get<int>(AlarmTableSync::COL_LEVEL))
				);
				if(value != getLevel())
				{
					setLevel(value);
					result = true;
				}
			}

			 // Short message
			if(record.isDefined(AlarmTableSync::COL_SHORT_MESSAGE))
			{
				string value(
					record.get<string>(AlarmTableSync::COL_SHORT_MESSAGE)
				);
				if(value != getShortMessage())
				{
					setShortMessage(value);
					result = true;
				}
			}

			// Long message
			if(record.isDefined(AlarmTableSync::COL_LONG_MESSAGE))
			{
				string value(
					record.get<string>(AlarmTableSync::COL_LONG_MESSAGE)
				);
				if(value != getLongMessage())
				{
					setLongMessage(value);
					result = true;
				}
			}

			// Raw editor
			if(record.isDefined(AlarmTableSync::COL_RAW_EDITOR))
			{
				bool value(
					record.get<bool>(AlarmTableSync::COL_RAW_EDITOR)
				);
				if(value != getRawEditor())
				{
					setRawEditor(value);
					result = true;
				}
			}

			// Done
			if(record.isDefined(AlarmTableSync::COL_DONE))
			{
				bool value(
					record.get<bool>(AlarmTableSync::COL_DONE)
				);
				if(value != getDone())
				{
					setDone(value);
					result = true;
				}
			}

			// Section
			if(record.isDefined(AlarmTableSync::COL_MESSAGES_SECTION_ID))
			{
				MessagesSection* value(NULL);
				try
				{
					RegistryKeyType sectionId(
						record.getDefault<RegistryKeyType>(
							AlarmTableSync::COL_MESSAGES_SECTION_ID,
							0
					)	);
					if(decodeTableId(sectionId) == MessagesSectionTableSync::TABLE.ID)
					{
						value = MessagesSectionTableSync::GetEditable(sectionId, env).get();
					}
				}
				catch(ObjectNotFoundException<MessagesSection>&)
				{
					Log::GetInstance().warn("No such message section in alarm "+ lexical_cast<string>(getKey()));
				}

				if(value != getSection())
				{
					if(value)
					{
						setSection(value);
					}
					else
					{
						setSection(NULL);
					}
					result = true;
				}
			}

			// Scenario
			if(record.isDefined(AlarmTableSync::COL_SCENARIO_ID))
			{
				const ScenarioTemplate* value(NULL);
				try
				{
					RegistryKeyType parentId(
						record.getDefault<RegistryKeyType>(
							AlarmTableSync::COL_SCENARIO_ID,
							0
					)	);
					if(decodeTableId(parentId) == ScenarioTableSync::TABLE.ID)
					{
						value = ScenarioTableSync::GetCast<ScenarioTemplate>(parentId, env).get();
					}
				}
				catch(ObjectNotFoundException<ScenarioTemplate>&)
				{
					Log::GetInstance().warn("No such scenario template in alarm "+ lexical_cast<string>(getKey()));
				}

				if(value != getScenario())
				{
					if(value)
					{
						setScenario(value);
						getScenario()->addMessage(*this);
					}
					else
					{
						setScenario(NULL);
					}
					result = true;
				}
			}

			// Data source links (at the end of the load to avoid registration of objects which are removed later by an exception)
			if(record.isDefined(AlarmTableSync::COL_DATASOURCE_LINKS))
			{
				Importable::DataSourceLinks value(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						record.get<string>(AlarmTableSync::COL_DATASOURCE_LINKS),
						env
				)	);
				if(value != getDataSourceLinks())
				{
					if(&env == &Env::GetOfficialEnv())
					{
						setDataSourceLinksWithRegistration(value);
					}
					else
					{
						setDataSourceLinksWithoutRegistration(value);
					}
					result = true;
				}
			}

			return result;
		}
}	}
