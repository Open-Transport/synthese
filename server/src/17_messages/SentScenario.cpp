
/** SentScenario class implementation.
	@file SentScenario.cpp

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

#include "SentScenario.h"

#include "MessagesSection.hpp"
#include "ScenarioCalendar.hpp"
#include "ScenarioTemplate.h"
#include "Alarm.h"
#include "ParametersMap.h"
#include "Registry.h"
#include "Request.h"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace messages;

	CLASS_DEFINITION(SentScenario, "t039_scenarios", 39)
	FIELD_DEFINITION_OF_TYPE(Enabled, "is_enabled", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(PeriodStart, "period_start", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(PeriodEnd, "period_end", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(Template, "template_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(EventStart, "event_start", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(EventEnd, "event_end", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(Archived, "archived", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(ManualOverride, "manual_override", SQL_BOOLEAN)
	
	namespace messages
	{
		const std::string SentScenario::DATA_ID = "id";
		const std::string SentScenario::DATA_NAME = "name";
		const std::string SentScenario::DATA_START_DATE = "start_date";
		const std::string SentScenario::DATA_END_DATE = "end_date";
		const std::string SentScenario::DATA_EVENT_START_DATE = "event_start_date";
		const std::string SentScenario::DATA_EVENT_END_DATE = "event_end_date";
		const std::string SentScenario::DATA_ACTIVE = "active";
		const std::string SentScenario::DATA_SCENARIO_ID = "scenario_id";
		const std::string SentScenario::DATA_IS_TEMPLATE = "is_template";
		const std::string SentScenario::DATA_ARCHIVED = "archived";
		const std::string SentScenario::DATA_CODE = "code";
		const std::string SentScenario::DATA_VALUE = "value";
		const std::string SentScenario::DATA_MANUAL_OVERRIDE = "manual_override";

		const std::string SentScenario::TAG_VARIABLE = "variable";
		const std::string SentScenario::TAG_MESSAGE = "message";
		const std::string SentScenario::TAG_TEMPLATE_SCENARIO = "template_scenario";
		const std::string SentScenario::TAG_SECTION = "section";
		const std::string SentScenario::TAG_CALENDAR = "calendar";



		SentScenario::SentScenario(
			util::RegistryKeyType key
		):	Scenario(key),
			Object<SentScenario, SentScenarioRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_VALUE_CONSTRUCTOR(Enabled, false),
					FIELD_VALUE_CONSTRUCTOR(PeriodStart, second_clock::local_time()),
					FIELD_VALUE_CONSTRUCTOR(PeriodEnd, not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(Template),
					FIELD_DEFAULT_CONSTRUCTOR(DataSourceLinksWithoutUnderscore),
					FIELD_DEFAULT_CONSTRUCTOR(Sections),
					FIELD_DEFAULT_CONSTRUCTOR(EventStart),
					FIELD_DEFAULT_CONSTRUCTOR(EventEnd),
					FIELD_VALUE_CONSTRUCTOR(Archived, false),
					FIELD_VALUE_CONSTRUCTOR(ManualOverride, false)
					))
		{}


		SentScenario::~SentScenario()
		{}


		SentScenario::SentScenario(const SentScenario& source)
			: Registrable(0)
			, Scenario(0)
			, Object<SentScenario, SentScenarioRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, 0),
					FIELD_VALUE_CONSTRUCTOR(Name, source.get<Name>()),
					FIELD_VALUE_CONSTRUCTOR(Enabled, false),
					FIELD_VALUE_CONSTRUCTOR(PeriodStart, second_clock::local_time()),
					FIELD_VALUE_CONSTRUCTOR(PeriodEnd, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(Template, source.get<Template>()),
					FIELD_DEFAULT_CONSTRUCTOR(DataSourceLinksWithoutUnderscore),
					FIELD_DEFAULT_CONSTRUCTOR(Sections),
					FIELD_DEFAULT_CONSTRUCTOR(EventStart),
					FIELD_DEFAULT_CONSTRUCTOR(EventEnd),
					FIELD_VALUE_CONSTRUCTOR(Archived, false),
					FIELD_VALUE_CONSTRUCTOR(ManualOverride, false)
					))
		{
		}

		
		bool SentScenario::isApplicable( const ptime& start, const ptime& end ) const
		{
			// Archived event is never applicable
			if(getArchived())
			{
				return false;
			}

			// Disabled alarm is never applicable
			if (!getIsEnabled())
			{
				return false;
			}

			// Start date control
			if (!getPeriodStart().is_not_a_date_time() && end < getPeriodStart())
			{
				return false;
			}

			// End date control
			if (!getPeriodEnd().is_not_a_date_time() && start >= getPeriodEnd())
			{
				return false;
			}

			return true;
		}



		bool SentScenario::isApplicable( const ptime& date ) const
		{
			return isApplicable(date, date);
		}

		ScenarioTemplate* SentScenario::getTemplate() const
		{
			return get<Template>().get_ptr();
		}


		void SentScenario::setTemplate(const ScenarioTemplate* value )
		{
			set<Template>(value
						  ? boost::optional<ScenarioTemplate&>(*const_cast<ScenarioTemplate*>(value))
						  : boost::none);
		}



		void SentScenario::toParametersMap( util::ParametersMap& pm ) const
		{
			// roid
			pm.insert(DATA_ID, getKey());
			pm.insert(DATA_SCENARIO_ID, getKey()); // Deprecated
			pm.insert(Request::PARAMETER_OBJECT_ID, getKey()); // Deprecated

			// name
			pm.insert(DATA_NAME, getName());

			// is template
			pm.insert(DATA_IS_TEMPLATE, false);

			// is template
			pm.insert(DATA_ARCHIVED, getArchived());

			if(	getTemplate()
			){
				// Template scenario
				boost::shared_ptr<ParametersMap> templatePM(new ParametersMap);
				getTemplate()->toParametersMap(*templatePM, true);
				pm.insert(TAG_TEMPLATE_SCENARIO, templatePM);
			}

			// Calendars
			bool oneMessageWithoutCalendar(false);
			BOOST_FOREACH(const ScenarioCalendar* calendar, getCalendars())
			{
				// Calendar export
				boost::shared_ptr<ParametersMap> calendarPM(new ParametersMap);
				calendar->toParametersMap(*calendarPM, true);
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
			if(	oneMessageWithoutCalendar ||
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

					// Message export
					boost::shared_ptr<ParametersMap> messagePM(new ParametersMap);
					alarm->toParametersMap(*messagePM, false, string(), true);
					calendarPM->insert(TAG_MESSAGE, messagePM);
				}
			}

			// Sections
			BOOST_FOREACH(const MessagesSection* section, get<Sections>())
			{
				boost::shared_ptr<ParametersMap> sectionPM(new ParametersMap);
				section->toParametersMap(*sectionPM, true);
				pm.insert(TAG_SECTION, sectionPM);
			}

			// start date
			if(!getPeriodStart().is_not_a_date_time())
			{
				pm.insert(DATA_START_DATE, getPeriodStart());
			}

			// end date
			if(!getPeriodEnd().is_not_a_date_time())
			{
				pm.insert(DATA_END_DATE, getPeriodEnd());
			}

			// start date
			if(!getEventStart().is_not_a_date_time())
			{
				pm.insert(DATA_EVENT_START_DATE, getEventStart());
			}

			// end date
			if(!getEventEnd().is_not_a_date_time())
			{
				pm.insert(DATA_EVENT_END_DATE, getEventEnd());
			}

			// active
			pm.insert(DATA_ACTIVE, getIsEnabled());

			// active
			pm.insert(DATA_MANUAL_OVERRIDE, getManualOverride());

		}

		void SentScenario::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{

		}


		void SentScenario::unlink()
		{
			cleanDataSourceLinks(true);
		}

		bool SentScenario::belongsToAnAutomaticSection() const
		{
			BOOST_FOREACH(const MessagesSection* section, get<Sections>())
			{
				if (section->get<AutoActivation>())
				{
					return true;
				}
			}
			return false;
		}


		bool SentScenario::shouldBeEnabled(const boost::posix_time::ptime& time) const
		{
			BOOST_FOREACH(const ScenarioCalendar* calendar, getCalendars())
			{
				if (calendar->isInside(time))
				{
					return true;
				}
			}
			return false;
		}


		bool SentScenario::shouldBeArchived(const boost::posix_time::ptime& time) const
		{
			if (getCalendars().empty())
			{
				return false;
			}
			BOOST_FOREACH(const ScenarioCalendar* calendar, getCalendars())
			{
				if (!calendar->isAfter(time))
				{
					return false;
				}
			}
			return true;
		}
		
}	}
