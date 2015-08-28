////////////////////////////////////////////////////////////////////////////////
/// ScenarioTemplate class implementation.
///	@file ScenarioTemplate.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ScenarioTemplate.h"

#include "Alarm.h"
#include "MessagesSection.hpp"
#include "ParametersMap.h"
#include "Registry.h"
#include "Request.h"
#include "ScenarioFolder.h"
#include "ScenarioCalendar.hpp"

#include <sstream>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;

	CLASS_DEFINITION(messages::ScenarioTemplate, "t028_scenario_templates", 28)
	FIELD_DEFINITION_OF_TYPE(messages::Folder, "folder_id", SQL_INTEGER)

	namespace messages
	{
		const std::string ScenarioTemplate::DATA_SCENARIO_ID = "scenario_id";
		const std::string ScenarioTemplate::DATA_NAME = "name";
		const std::string ScenarioTemplate::DATA_FOLDER_ID = "folder_id";
		const std::string ScenarioTemplate::DATA_FOLDER_NAME = "folder_name";
		const std::string ScenarioTemplate::DATA_IS_TEMPLATE = "is_template";

		const std::string ScenarioTemplate::TAG_VARIABLE = "variable";
		const std::string ScenarioTemplate::TAG_MESSAGE = "message";
		const std::string ScenarioTemplate::TAG_SECTION = "section";
		const std::string ScenarioTemplate::TAG_CALENDAR = "calendar";

		ScenarioTemplate::ScenarioTemplate(
			util::RegistryKeyType key
		) : Scenario(key),
			Object<ScenarioTemplate, ScenarioTemplateRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(Folder),
					FIELD_DEFAULT_CONSTRUCTOR(Sections)
			)	)
		{}


		ScenarioTemplate::~ScenarioTemplate()
		{}



		ScenarioFolder* ScenarioTemplate::getFolder() const
		{
			return get<Folder>().get_ptr();
		}


		void ScenarioTemplate::setFolder(ScenarioFolder* value )
		{
			set<Folder>(value
				? boost::optional<ScenarioFolder&>(*value)
				: boost::none);
		}


		void ScenarioTemplate::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const	{
			// roid
			pm.insert(DATA_SCENARIO_ID, getKey());
			pm.insert(Request::PARAMETER_OBJECT_ID, getKey()); // Deprecated

			// name
			pm.insert(DATA_NAME, getName());

			// is template
			pm.insert(DATA_IS_TEMPLATE, true);

			// Calendars
			bool oneMessageWithoutCalendar(false);
			BOOST_FOREACH(const ScenarioCalendar* calendar, getCalendars())
			{
				// Calendar export
				boost::shared_ptr<ParametersMap> calendarPM(new ParametersMap);
				calendar->toParametersMap(*calendarPM);
				pm.insert(TAG_CALENDAR, calendarPM);

				// Messages loop
				BOOST_FOREACH(const Alarm* alarm, getMessages())
				{
					// Mark message without calendar and avoit it
					if(!alarm->getCalendar())
					{
						oneMessageWithoutCalendar = true;
						continue;
					}

					// Jump over messages of other calendars
					if(alarm->getCalendar() != calendar)
					{
						continue;
					}

					// Message export
					boost::shared_ptr<ParametersMap> messagePM(new ParametersMap);
					alarm->toParametersMap(*messagePM, false, string(), true);
					calendarPM->insert(TAG_MESSAGE, messagePM);
				}
			}

			// Fake calendar for old style messages
			if( oneMessageWithoutCalendar ||
				(getCalendars().empty() && !getMessages().empty())
			){
				// Fake calendar export
				boost::shared_ptr<ParametersMap> calendarPM(new ParametersMap);
				calendarPM->insert(Key::FIELD.name, 0);
				calendarPM->insert(Name::FIELD.name, string());
				calendarPM->insert(ScenarioPointer::FIELD.name, getKey());
				pm.insert(TAG_CALENDAR, calendarPM);
				// Old style messages loop
				BOOST_FOREACH(const Alarm* alarm, getMessages())
				{
					// Jump over new style messages
					if(alarm->getCalendar())
					{
						continue;
					}

					boost::shared_ptr<ParametersMap> messagePM(new ParametersMap);
					alarm->toParametersMap(*messagePM, false, string(), true);
					calendarPM->insert(TAG_MESSAGE, messagePM);
				}
			}


			BOOST_FOREACH(const MessagesSection* section, getSections())
			{
				boost::shared_ptr<ParametersMap> sectionPM(new ParametersMap);
				section->toParametersMap(*sectionPM, true);
				pm.insert(TAG_SECTION, sectionPM);
			}

			// Folder
			if(getFolder())
			{
				// folder_id
				pm.insert(DATA_FOLDER_ID, getFolder()->getKey());

				// folder_name
				pm.insert(DATA_FOLDER_NAME, getFolder()->getName());
			}
		}

}	}
