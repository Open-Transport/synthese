
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

#include "DataSourceLinksField.hpp"
#include "DBConstants.h"
#include "ImportableTableSync.hpp"
#include "MessagesSection.hpp"
#include "MessagesSectionTableSync.hpp"
#include "ScenarioCalendar.hpp"
#include "ScenarioTableSync.h"
#include "ScenarioTemplate.h"
#include "SentAlarm.h"
#include "ParametersMap.h"
#include "Registry.h"
#include "Request.h"

#include <sstream>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace util;
	using namespace impex;

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
				calendar->toParametersMap(*calendarPM, true, false, string());
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
					alarm->toParametersMap(*messagePM, false, true, string());
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
					alarm->toParametersMap(*messagePM, false, true, string());
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

		void SentScenario::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix /*= std::string() */
		) const	{
			// Inter synthese package
			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + ScenarioTableSync::COL_IS_TEMPLATE,
				false
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_ENABLED,
				getIsEnabled()
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_NAME,
				getName()
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_PERIODSTART,
				getPeriodStart()
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_PERIODEND,
				getPeriodEnd()
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_FOLDER_ID,
				RegistryKeyType(0)
			);
			stringstream vars;
			const SentScenario::VariablesMap& variables(getVariables());
			bool firstVar(true);
			BOOST_FOREACH(const SentScenario::VariablesMap::value_type& variable, variables)
			{
				if(!firstVar)
				{
					vars << ScenarioTableSync::VARIABLES_SEPARATOR;
				}
				else
				{
					firstVar = false;
				}
				vars << variable.first << ScenarioTableSync::VARIABLES_OPERATOR << variable.second;
			}
			pm.insert(
				prefix + ScenarioTableSync::COL_VARIABLES,
				vars.str()
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_TEMPLATE,
				getTemplate() ? getTemplate()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_DATASOURCE_LINKS,
				synthese::DataSourceLinks::Serialize(getDataSourceLinks())
			);
			bool first(true);
			stringstream sectionsStr;
			BOOST_FOREACH(const MessagesSection* section, this->getSections())
			{
				if(first)
				{
					first = false;
				}
				else
				{
					sectionsStr << ScenarioTableSync::SECTIONS_SEPARATOR;
				}
				sectionsStr << section->getKey();
			}
			pm.insert(
				prefix + ScenarioTableSync::COL_SECTIONS,
				sectionsStr.str()
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_EVENT_START,
				getEventStart()
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_EVENT_END,
				getEventEnd()
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_ARCHIVED,
				getArchived()
			);
		}

		bool SentScenario::loadFromRecord(
			const Record& record,
			util::Env& env
		){
			bool result(false);

			// Enabled
			if(record.isDefined(ScenarioTableSync::COL_ENABLED))
			{
				bool value(
					record.get<bool>(ScenarioTableSync::COL_ENABLED)
				);
				if(value != getIsEnabled())
				{
					setIsEnabled(value);
					result = true;
				}
			}

			// Name
			if(record.isDefined(ScenarioTableSync::COL_NAME))
			{
				string value(
					record.get<string>(ScenarioTableSync::COL_NAME)
				);
				if(value != getName())
				{
					setName(value);
					result = true;
				}
			}

			// Period start
			if(record.isDefined(ScenarioTableSync::COL_PERIODSTART))
			{
				string strValue(
					record.get<string>(ScenarioTableSync::COL_PERIODSTART)
				);
				if (!strValue.empty())
				{
					ptime value = time_from_string(strValue);
					if(value != getPeriodStart())
					{
						setPeriodStart(value);
						result = true;
					}
				}
			}

			// Period end
			if(record.isDefined(ScenarioTableSync::COL_PERIODEND))
			{
				string strValue(
					record.get<string>(ScenarioTableSync::COL_PERIODEND)
				);
				if (!strValue.empty())
				{
					ptime value = time_from_string(strValue);
					if(value != getPeriodEnd())
					{
						setPeriodEnd(value);
						result = true;
					}
				}
			}

			// Variables
			const string txtVariables(record.get<string>(ScenarioTableSync::COL_VARIABLES));
			SentScenario::VariablesMap variables;
			vector<string> tokens;
			split(tokens, txtVariables, is_any_of(ScenarioTableSync::VARIABLES_SEPARATOR));
			BOOST_FOREACH(const string& token, tokens)
			{
				if(token.empty()) continue;

				typedef split_iterator<string::const_iterator> string_split_iterator;
				string_split_iterator it(
					make_split_iterator(
						token,
						first_finder(ScenarioTableSync::VARIABLES_OPERATOR, is_iequal())
				)	);
				string code = copy_range<string>(*it);
				++it;
				if (it == string_split_iterator())
				{
					Log::GetInstance().warn("Bad value for variable definition on scenario table");
					continue;
				}
				variables.insert(make_pair(code, copy_range<string>(*it)));
			}
			setVariables(variables);

			//Template
			if(record.isDefined(ScenarioTableSync::COL_TEMPLATE))
			{
				ScenarioTemplate* value(NULL);
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						ScenarioTableSync::COL_TEMPLATE,
						0
				)	);
				if(id > 0)
				{
					try
					{
						value = static_cast<ScenarioTemplate*>(ScenarioTableSync::GetEditable(id, env).get());
					}
					catch(ObjectNotFoundException<ScenarioTemplate>&)
					{
						Log::GetInstance().warn("No such scenario template in sent scenario "+ lexical_cast<string>(getKey()));
					}
				}
				if(value != getTemplate())
				{
					setTemplate(value);
					result = true;
				}
			}

			// Sections
			const string txtSections(record.get<string>(ScenarioTableSync::COL_SECTIONS));
			Scenario::Sections sections;
			if(!txtSections.empty())
			{
				vector<string> tokens;
				split(tokens, txtSections, is_any_of(ScenarioTableSync::SECTIONS_SEPARATOR));
				BOOST_FOREACH(const string& token, tokens)
				{
					try
					{
						sections.insert(
							MessagesSectionTableSync::Get(
								lexical_cast<RegistryKeyType>(token),
								env
							).get()
						);
					}
					catch (bad_lexical_cast&)
					{
					}
					catch(ObjectNotFoundException<MessagesSection>&)
					{
					}
				}
			}
			setSections(sections);

			// Event start
			if(record.isDefined(ScenarioTableSync::COL_EVENT_START))
			{
				string strValue(
					record.get<string>(ScenarioTableSync::COL_EVENT_START)
				);
				if (!strValue.empty())
				{
					ptime value = time_from_string(strValue);
					if(value != getEventStart())
					{
						setEventStart(value);
						result = true;
					}
				}
			}

			// Event end
			if(record.isDefined(ScenarioTableSync::COL_EVENT_END))
			{
				string strValue(
					record.get<string>(ScenarioTableSync::COL_EVENT_END)
				);
				if (!strValue.empty())
				{
					ptime value = time_from_string(strValue);
					if(value != getEventEnd())
					{
						setEventEnd(value);
						result = true;
					}
				}
			}

			// Archived
			if(record.isDefined(ScenarioTableSync::COL_ARCHIVED))
			{
				bool value(
					record.get<bool>(ScenarioTableSync::COL_ARCHIVED)
				);
				if(value != getArchived())
				{
					setArchived(value);
					result = true;
				}
			}

			// Data source links (at the end of the load to avoid registration of objects which are removed later by an exception)
			if(record.isDefined(ScenarioTableSync::COL_DATASOURCE_LINKS))
			{
				Importable::DataSourceLinks value(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						record.get<string>(ScenarioTableSync::COL_DATASOURCE_LINKS),
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
