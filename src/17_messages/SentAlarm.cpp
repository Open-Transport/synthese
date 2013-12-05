////////////////////////////////////////////////////////////////////////////////
/// SentAlarm class implementation.
///	@file SentAlarm.cpp
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

#include "SentAlarm.h"

#include "AlarmRecipient.h"
#include "AlarmTableSync.h"
#include "AlarmTemplate.h"
#include "DataSourceLinksField.hpp"
#include "Factory.h"
#include "ImportableTableSync.hpp"
#include "MessagesSectionTableSync.hpp"
#include "Registry.h"
#include "ScenarioCalendar.hpp"
#include "ScenarioCalendarTableSync.hpp"
#include "ScenarioTableSync.h"
#include "SentScenario.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace impex;

	namespace messages
	{
		SentAlarm::SentAlarm(
			util::RegistryKeyType key ,
			const SentScenario* scenario /*= NULL */
		):	Registrable(key),
			Alarm(key, scenario),
			_template(NULL)
		{}



		SentAlarm::SentAlarm(
			const SentScenario& scenario,
			const AlarmTemplate& source
		):	Registrable(0),
			Alarm(source, &scenario, NULL),
			_template(&source)
		{}



		SentAlarm::SentAlarm(
			const SentScenario& scenario,
			const SentAlarm& source
		):	Registrable(0),
			Alarm(source, &scenario, NULL),
			_template(source._template)
		{}



		SentAlarm::SentAlarm(
			const SentScenario& scenario,
			const Alarm& source
		):	Registrable(0),
			Alarm(source, &scenario, NULL),
			_template(
				dynamic_cast<const AlarmTemplate*>(&source) ?
				static_cast<const AlarmTemplate*>(&source) :
				static_cast<const SentAlarm&>(source)._template
			)
		{}



		SentAlarm::~SentAlarm()
		{}



		const SentScenario* SentAlarm::getScenario() const
		{
			return static_cast<const SentScenario*>(Alarm::getScenario());
		}



		const AlarmTemplate* SentAlarm::getTemplate() const
		{
			return _template;
		}



		void SentAlarm::setTemplate( const AlarmTemplate* value )
		{
			_template = value;
		}



		void SentAlarm::toParametersMap( util::ParametersMap& pm, bool withScenario, boost::logic::tribool withRecipients /*= false */, std::string prefix /*= std::string()*/ ) const
		{
			// Inter synthese package
			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + AlarmTableSync::COL_IS_TEMPLATE,
				false
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
				getTemplate() ? getTemplate()->getKey() : RegistryKeyType(0)
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
				getSection() ? getSection()->getKey() : RegistryKeyType(0)
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



		//////////////////////////////////////////////////////////////////////////
		/// Checks if the current message is active at the specified time.
		/// @param when the time to check
		/// @return true if the message must be displayed at the specified time
		bool SentAlarm::isApplicable( boost::posix_time::ptime& when ) const
		{
			// Check if the event is active first
			if( !getScenario() ||
				!getScenario()->getIsEnabled()
			){
				return false;
			}

			// Then check if specific application periods are defined for the current message
			if(_calendar)
			{
				// Search for an application period including the checked date
				BOOST_FOREACH(
					const ScenarioCalendar::ApplicationPeriods::value_type& period,
					_calendar->getApplicationPeriods()
				){
					if(period->getValue(when))
					{
						return true;
					}
				}

				// No period was found : the message is inactive
				return false;
			}
			else
			{
				// Then refer to the simple start/end date of the scenario
				return
					(getScenario()->getPeriodStart().is_not_a_date_time() || getScenario()->getPeriodStart() <= when) &&
					(getScenario()->getPeriodEnd().is_not_a_date_time() || getScenario()->getPeriodEnd() >= when)
				;
			}
		}



		void SentAlarm::clearBroadcastPointsCache() const
		{
			_broadcastPointsCache.clear();
		}



		bool SentAlarm::isOnBroadcastPoint(
			const BroadcastPoint& point,
			const util::ParametersMap& parameters
		) const	{

			BroadcastPointsCache::key_type pp(
				make_pair(&point, parameters)
			);
			BroadcastPointsCache::const_iterator it(
				_broadcastPointsCache.find(pp)
			);
			if(it == _broadcastPointsCache.end())
			{
				it = _broadcastPointsCache.insert(
					make_pair(
						pp,
						_isOnBroadcastPoint(point, parameters)
				)	).first;
			}
			return it->second;
		}

		bool SentAlarm::loadFromRecord(
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
				const SentScenario* value(NULL);
				try
				{
					RegistryKeyType parentId(
						record.getDefault<RegistryKeyType>(
							AlarmTableSync::COL_SCENARIO_ID,
							0
					)	);
					if(decodeTableId(parentId) == ScenarioTableSync::TABLE.ID)
					{
						value = ScenarioTableSync::GetCast<SentScenario>(parentId, env).get();
					}
				}
				catch(ObjectNotFoundException<SentScenario>&)
				{
					Log::GetInstance().warn("No such sent scenario in alarm "+ lexical_cast<string>(getKey()));
				}

				if(value != getScenario())
				{
					if(value)
					{
						setScenario(value);
						value->addMessage(*this);
					}
					else
					{
						setScenario(NULL);
					}
					result = true;
				}
			}

			//Template
			if(record.isDefined(AlarmTableSync::COL_TEMPLATE_ID))
			{
				AlarmTemplate* value(NULL);
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						AlarmTableSync::COL_TEMPLATE_ID,
						0
				)	);
				if(id > 0)
				{
					try
					{
						value = static_cast<AlarmTemplate*>(AlarmTableSync::GetEditable(id, env).get());
					}
					catch(ObjectNotFoundException<AlarmTemplate>&)
					{
						Log::GetInstance().warn("No such alarm template in sent alarm "+ lexical_cast<string>(getKey()));
					}
				}
				if(value != getTemplate())
				{
					setTemplate(value);
					result = true;
				}
			}

			//Calendar
			if(record.isDefined(AlarmTableSync::COL_CALENDAR_ID))
			{
				ScenarioCalendar* value(NULL);
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						AlarmTableSync::COL_CALENDAR_ID,
						0
				)	);
				if(id > 0)
				{
					try
					{
						value = ScenarioCalendarTableSync::GetEditable(id, env).get();
					}
					catch(ObjectNotFoundException<ScenarioCalendar>&)
					{
						Log::GetInstance().warn("No such scenario calendar in sent alarm "+ lexical_cast<string>(getKey()));
					}
				}
				if(value != getCalendar())
				{
					setCalendar(value);
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
