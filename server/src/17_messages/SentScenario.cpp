
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
#include "SentAlarm.h"
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

		const std::string SentScenario::TAG_VARIABLE = "variable";
		const std::string SentScenario::TAG_MESSAGE = "message";
		const std::string SentScenario::TAG_TEMPLATE_SCENARIO = "template_scenario";
		const std::string SentScenario::TAG_SECTION = "section";
		const std::string SentScenario::TAG_CALENDAR = "calendar";



		SentScenario::SentScenario(
			util::RegistryKeyType key
		):	Registrable(key),
			Scenario(),
			_isEnabled(false),
			_periodStart(second_clock::local_time()),
			_periodEnd(not_a_date_time),
			_template(NULL),
			_archived(false)
		{}



		SentScenario::SentScenario(
			const ScenarioTemplate& source
		):	Registrable(0),
			Scenario(source.getName()),
			_isEnabled(false),
			_periodStart(second_clock::local_time()),
			_periodEnd(not_a_date_time),
			_template(&source),
			_archived(false)
		{}



		SentScenario::SentScenario(
			const SentScenario& source
		):	Registrable(0),
			Scenario(source._template ? source._template->getName() : source.getName()),
			_isEnabled(false),
			_periodStart(second_clock::local_time()),
			_periodEnd(not_a_date_time),
			_template(source._template),
			_variables(source._variables),
			_archived(false)
		{
		}



		void SentScenario::setPeriodStart( const ptime& periodStart )
		{
			_periodStart = periodStart;
		}



		void SentScenario::setPeriodEnd( const ptime& periodEnd )
		{
			_periodEnd = periodEnd;
		}



		SentScenario::~SentScenario()
		{}



		bool SentScenario::isApplicable( const ptime& start, const ptime& end ) const
		{
			// Archived event is never applicable
			if(_archived)
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


		bool SentScenario::belongsToAnAutomaticSection() const
		{
			BOOST_FOREACH(const Scenario::Sections::value_type& section, getSections())
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


		void SentScenario::setTemplate(
			const ScenarioTemplate* value
		){
			_template = value;
		}



		void SentScenario::setVariables(
			const VariablesMap& value
		){
			_variables = value;
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

				// Variables
				const ScenarioTemplate::VariablesMap& variables(
					getTemplate()->getVariables()
				);
				BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
				{
					boost::shared_ptr<ParametersMap> variablePM(new ParametersMap);
					string value;
					const SentScenario::VariablesMap& values(getVariables());
					SentScenario::VariablesMap::const_iterator it(values.find(variable.first));
					if(it != values.end())
					{
						value = it->second;
					}
					
					// code
					variablePM->insert(DATA_CODE, variable.first);

					// value
					variablePM->insert(DATA_VALUE, value);

					pm.insert(TAG_VARIABLE, variablePM);
				}
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
			BOOST_FOREACH(const MessagesSection* section, getSections())
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
		}
}	}
