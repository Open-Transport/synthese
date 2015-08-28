
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
#include "MessageApplicationPeriod.hpp"
#include "MessagesSection.hpp"
#include "MessagesModule.h"
#include "BroadcastPoint.hpp"
#include "Scenario.h"
#include "SentScenario.h"
#include "ScenarioCalendar.hpp"
#include "ScenarioTemplate.h"
#include "ParametersMap.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace impex;
	using namespace messages;

	CLASS_DEFINITION(Alarm, "t003_alarms", 3)
	FIELD_DEFINITION_OF_OBJECT(messages::Alarm, "alarm_id", "alarm_ids")

	
	FIELD_DEFINITION_OF_TYPE(Level, "level", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ShortMessage, "short_message", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(LongMessage, "long_message", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ParentScenario, "scenario_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(RawEditor, "raw_editor", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(Done, "done", SQL_BOOLEAN)

	FIELD_DEFINITION_OF_TYPE(Calendar, "calendar_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DisplayDuration, "display_duration", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DigitizedVersion, "digitized_version", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(RepeatInterval, "repeat_interval", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(WithAck, "with_ack", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(MultipleStops, "multiple_stops", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(PlayTts, "play_tts", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(Light, "light", SQL_BOOLEAN)
	
	FIELD_DEFINITION_OF_TYPE(DirectionSignCode, "direction_sign_code", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(StartStopPoint, "start_stop_point", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(EndStopPoint, "end_stop_point", SQL_INTEGER)


	
	namespace messages
	{
		const string Alarm::DATA_MESSAGE_ID("message_id");
		const string Alarm::DATA_CONTENT("content");
		const string Alarm::DATA_PRIORITY("priority");
		const string Alarm::DATA_SCENARIO_ID("scenario_id");
		const string Alarm::DATA_SCENARIO_NAME("scenario_name");
		const string Alarm::DATA_TITLE("title");
		const string Alarm::DATA_DONE = "done";
		const string Alarm::DATA_DISPLAY_DURATION("display_duration");
		const string Alarm::DATA_DIGITIZED_VERSION("digitized_version");
		const string Alarm::DATA_TAGS("tags");
		const string Alarm::DATA_REPEAT_INTERVAL("repeat_interval");
		const string Alarm::DATA_WITH_ACK("with_ack");
		const string Alarm::DATA_MULTIPLE_STOPS("multiple_stops");
		const string Alarm::DATA_PLAY_TTS("play_tts");
		const string Alarm::DATA_LIGHT("light");
		const string Alarm::DATA_DIRECTION_SIGN_CODE("direction_sign_code");
		const string Alarm::DATA_START_STOP_POINT("start_stop_point");
		const string Alarm::DATA_END_STOP_POINT("end_stop_point");
		const string Alarm::DATA_REPEAT_INTERVAL("repeat_interval");
		const string Alarm::DATA_WITH_ACK("with_ack");
		const string Alarm::DATA_MULTIPLE_STOPS("multiple_stops");
		const string Alarm::DATA_PLAY_TTS("play_tts");
		const string Alarm::DATA_LIGHT("light");
		const string Alarm::DATA_DIRECTION_SIGN_CODE("direction_sign_code");
		const string Alarm::DATA_START_STOP_POINT("start_stop_point");
		const string Alarm::DATA_END_STOP_POINT("end_stop_point");

		const string Alarm::TAG_MESSAGE_ALTERNATIVE = "message_alternative";
		const string Alarm::TAG_RECIPIENTS = "recipients";
		const string Alarm::TAG_SECTION = "section";

		const string Alarm::ATTR_LINK_ID = "link_id";
		const string Alarm::ATTR_LINK_PARAMETER = "link_parameter";
		const string Alarm::ATTR_CALENDAR_ID = "calendar_id";


		Alarm::Alarm(
			util::RegistryKeyType key
			):	Registrable(key),
				Object<Alarm, AlarmRecord>(
					Schema(
						FIELD_VALUE_CONSTRUCTOR(Key, key),
						FIELD_DEFAULT_CONSTRUCTOR(Name),
						FIELD_VALUE_CONSTRUCTOR(Level, ALARM_LEVEL_INFO),
						FIELD_DEFAULT_CONSTRUCTOR(ShortMessage),
						FIELD_DEFAULT_CONSTRUCTOR(LongMessage),
						FIELD_DEFAULT_CONSTRUCTOR(ParentScenario),
						FIELD_VALUE_CONSTRUCTOR(RawEditor, false),
						FIELD_VALUE_CONSTRUCTOR(Done, true),
						FIELD_DEFAULT_CONSTRUCTOR(messages::MessagesSection),
						FIELD_DEFAULT_CONSTRUCTOR(Calendar),
						FIELD_DEFAULT_CONSTRUCTOR(DataSourceLinksWithoutUnderscore),
						FIELD_DEFAULT_CONSTRUCTOR(DisplayDuration),
						FIELD_DEFAULT_CONSTRUCTOR(DigitizedVersion),
						FIELD_DEFAULT_CONSTRUCTOR(RepeatInterval),
						FIELD_DEFAULT_CONSTRUCTOR(WithAck),
						FIELD_DEFAULT_CONSTRUCTOR(MultipleStops),
						FIELD_DEFAULT_CONSTRUCTOR(PlayTts),
						FIELD_DEFAULT_CONSTRUCTOR(Light),
						FIELD_DEFAULT_CONSTRUCTOR(DirectionSignCode),
						FIELD_DEFAULT_CONSTRUCTOR(StartStopPoint),
						FIELD_DEFAULT_CONSTRUCTOR(EndStopPoint)
						)	)
		{}



		Alarm::Alarm(
			const Alarm& source
			):	Registrable(0),
				Object<Alarm, AlarmRecord>(
					Schema(
						FIELD_VALUE_CONSTRUCTOR(Key, 0),
						FIELD_DEFAULT_CONSTRUCTOR(Name),
						FIELD_VALUE_CONSTRUCTOR(Level, source.getLevel()),
						FIELD_VALUE_CONSTRUCTOR(ShortMessage, source.getShortMessage()),
						FIELD_VALUE_CONSTRUCTOR(LongMessage, source.getLongMessage()),
						FIELD_VALUE_CONSTRUCTOR(ParentScenario, source.get<ParentScenario>()),
						FIELD_VALUE_CONSTRUCTOR(RawEditor, source.getRawEditor()),
						FIELD_VALUE_CONSTRUCTOR(Done, source.getDone()),
						FIELD_VALUE_CONSTRUCTOR(messages::MessagesSection, source.get<messages::MessagesSection>()),
						FIELD_VALUE_CONSTRUCTOR(Calendar, source.get<Calendar>()),
						FIELD_DEFAULT_CONSTRUCTOR(DataSourceLinksWithoutUnderscore),
						FIELD_VALUE_CONSTRUCTOR(DisplayDuration, source.getDisplayDuration()),
						FIELD_VALUE_CONSTRUCTOR(DigitizedVersion, source.getDigitizedVersion()),
						FIELD_VALUE_CONSTRUCTOR(Tags, source.getTags()),
						FIELD_VALUE_CONSTRUCTOR(RepeatInterval, source.getRepeatInterval()),
						FIELD_VALUE_CONSTRUCTOR(WithAck, source.getWithAck()),
						FIELD_VALUE_CONSTRUCTOR(MultipleStops, source.getMultipleStops()),
						FIELD_VALUE_CONSTRUCTOR(PlayTts, source.getPlayTts()),
						FIELD_VALUE_CONSTRUCTOR(Light, source.getLight()),
						FIELD_VALUE_CONSTRUCTOR(DirectionSignCode, source.getDirectionSignCode()),
						FIELD_VALUE_CONSTRUCTOR(StartStopPoint, source.getStartStopPoint()),
						FIELD_VALUE_CONSTRUCTOR(EndStopPoint, source.getEndStopPoint())
						)	)
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
			pm.insert(prefix + DATA_DISPLAY_DURATION, getDisplayDuration());
			pm.insert(prefix + DATA_DIGITIZED_VERSION, getDigitizedVersion());

			string tagsString = boost::algorithm::join(_tags, ",");
			pm.insert(prefix + DATA_TAGS, tagsString);

			if(withScenario && getScenario())
			{
				pm.insert(prefix + DATA_SCENARIO_ID, getScenario()->getKey());
				pm.insert(prefix + DATA_SCENARIO_NAME, getScenario()->getName());
			}

			// Section
			if(getSection())
			{
				boost::shared_ptr<ParametersMap> sectionPM(new ParametersMap);
				getSection()->toParametersMap(*sectionPM, true);
				pm.insert(TAG_SECTION, sectionPM);
			}

			// Message alternatives
			BOOST_FOREACH(const MessageAlternatives::value_type& it, _messageAlternatives)
			{
				boost::shared_ptr<ParametersMap> altPM(new ParametersMap);
				it.second->toParametersMap(*altPM);
				pm.insert(TAG_MESSAGE_ALTERNATIVE, altPM);
			}

			if(withRecipients)
			{
				boost::shared_ptr<ParametersMap> recipientsPM(new ParametersMap);

				// Locks the linked objects
				mutex::scoped_lock(_linkedObjectsMutex);

				// Loop on linked objects
				LinkedObjectsToParametersMap(
					_linkedObjects,
					*recipientsPM
				);
				pm.insert(TAG_RECIPIENTS, recipientsPM);
			}

			pm.insert(ATTR_CALENDAR_ID, getCalendar() ? getCalendar()->getKey() : 0);

			// Ineo Terminus parameters
			pm.insert(prefix + DATA_REPEAT_INTERVAL, getRepeatInterval());
			pm.insert(prefix + DATA_WITH_ACK, getWithAck());
			pm.insert(prefix + DATA_MULTIPLE_STOPS, getMultipleStops());
			pm.insert(prefix + DATA_PLAY_TTS, getPlayTts());
			pm.insert(prefix + DATA_LIGHT, getLight());
			pm.insert(prefix + DATA_DIRECTION_SIGN_CODE, getDirectionSignCode());
			pm.insert(prefix + DATA_START_STOP_POINT, getStartStopPoint());
			pm.insert(prefix + DATA_END_STOP_POINT, getEndStopPoint());

			dataSourceLinksToParametersMap(pm);

		}




		//////////////////////////////////////////////////////////////////////////
		/// Checks if the message must be displayed on the specified broadcast point
		/// according to the display parameters
		bool Alarm::_isOnBroadcastPoint(
			const BroadcastPoint& point,
			const ParametersMap& parameters
		) const	{
			// Locks the linked objects
			mutex::scoped_lock(_linkedObjectsMutex);
			// Asks the broadcast point
			return point.displaysMessage(_linkedObjects, parameters);
		}


		//////////////////////////////////////////////////////////////////////////
		/// Public view of _isOnBroadcastPoint.
		bool Alarm::isOnBroadcastPoint(
			const BroadcastPoint& point,
			const util::ParametersMap& parameters
		) const {
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


		void Alarm::addLinkedObject(
			const AlarmObjectLink& link
		) const	{
			// Locks the cache
			mutex::scoped_lock(_linkedObjectsMutex);

			// Adds the object in the cache
			LinkedObjects::iterator it(
				_linkedObjects.find(
					link.getRecipient()->getFactoryKey()
			)	);
			if(it == _linkedObjects.end())
			{
				it = _linkedObjects.insert(
					make_pair(
						link.getRecipient()->getFactoryKey(),
						LinkedObjects::mapped_type()
					)
				).first;
			}
			it->second.insert(&link);
		}



		void Alarm::removeLinkedObject( const AlarmObjectLink& link ) const
		{
			// Locks the cache
			mutex::scoped_lock(_linkedObjectsMutex);

			// Removes the object of the cache
			LinkedObjects::iterator it(
				_linkedObjects.find(
					link.getRecipient()->getFactoryKey()
			)	);
			if(it != _linkedObjects.end())
			{
				it->second.erase(&link);
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Exports linked objects into a parameters map.
		/// @param linkedObjects the objects to export
		/// @param pm the parameters map to populate
		void Alarm::LinkedObjectsToParametersMap(
			const LinkedObjects& linkedObjects,
			util::ParametersMap& pm
		){
			BOOST_FOREACH(const LinkedObjects::value_type& ar, linkedObjects)
			{
				BOOST_FOREACH(const LinkedObjects::mapped_type::value_type& it, ar.second)
				{
					boost::shared_ptr<ParametersMap> arPM(new ParametersMap);
					arPM->insert(TABLE_COL_ID, it->getObjectId());
					arPM->insert(ATTR_LINK_ID, it->getKey());
					arPM->insert(ATTR_LINK_PARAMETER, it->getParameter());
					pm.insert(ar.first, arPM);
			}	}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Extracts the linked objects from the cache, for a recipient type only.
		/// @param recipientKey the factory key of the recipient type
		/// @return a copy of the list of the recipients
		Alarm::LinkedObjects::mapped_type Alarm::getLinkedObjects(
			const std::string& recipientKey
		) const	{

			// Locks the cache
			mutex::scoped_lock(_linkedObjectsMutex);

			// Search the recipient key in the cache
			LinkedObjects::iterator it(
				_linkedObjects.find(
					recipientKey
			)	);

			// If not found return empty list
			if(it == _linkedObjects.end())
			{
				return LinkedObjects::mapped_type();
			}

			return it->second;
		}

		bool
		Alarm::belongsToTemplate() const
		{
			return dynamic_cast<const ScenarioTemplate*>(getScenario());
		}


		void Alarm::clearBroadcastPointsCache() const
		{
			_broadcastPointsCache.clear();
		}

		bool Alarm::isApplicable( boost::posix_time::ptime& when ) const
		{
			if (belongsToTemplate())
			{
				return false;
			}
			const SentScenario* sentScenario = dynamic_cast<const SentScenario*>(getScenario());
			if (!sentScenario->getIsEnabled()) return false;
			
			// Then check if specific application periods are defined for the current message

			if(getCalendar())
			{
				// Search for an application period including the checked date
				BOOST_FOREACH(
					const ScenarioCalendar::ApplicationPeriods::value_type& period,
					getCalendar()->getApplicationPeriods()
					) {
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
					(sentScenario->getPeriodStart().is_not_a_date_time() || sentScenario->getPeriodStart() <= when) &&
					(sentScenario->getPeriodEnd().is_not_a_date_time() || sentScenario->getPeriodEnd() >= when)
					;
			}
				
		}
		


		boost::posix_time::ptime Alarm::getApplicationStart(
			const boost::posix_time::ptime& when
		) const {
			posix_time::ptime result = posix_time::not_a_date_time;
			if (belongsToTemplate())
			{
				return result;
			}
			const SentScenario* sentScenario = dynamic_cast<const SentScenario*>(getScenario());
			if (!sentScenario->getIsEnabled()) return result;

			// Then check if specific application periods are defined for the current message
			if(getCalendar())
			{
				// Search for an application period including the checked date
				BOOST_FOREACH(
					const ScenarioCalendar::ApplicationPeriods::value_type& period,
					getCalendar()->getApplicationPeriods()
				){
					if(period->getValue(when))
					{
						result = period->getStart(when);
					}
				}
			}
			else
			{
				// Then refer to the simple start/end date of the scenario
				if ((sentScenario->getPeriodStart().is_not_a_date_time() || sentScenario->getPeriodStart() >= when) &&
					(sentScenario->getPeriodEnd().is_not_a_date_time() || sentScenario->getPeriodEnd() <= when))
				{
					result = sentScenario->getPeriodStart();
				}
			}
			return result;
		}



		boost::posix_time::ptime Alarm::getApplicationEnd(
			const boost::posix_time::ptime& when
		) const {
			posix_time::ptime result = posix_time::not_a_date_time;
			if (belongsToTemplate())
			{
				return result;
			}
			const SentScenario* sentScenario = dynamic_cast<const SentScenario*>(getScenario());
			if (!sentScenario->getIsEnabled()) return result;

			// Then check if specific application periods are defined for the current message
			if(getCalendar())
			{
				// Search for an application period including the checked date
				BOOST_FOREACH(
					const ScenarioCalendar::ApplicationPeriods::value_type& period,
					getCalendar()->getApplicationPeriods()
				){
					if(period->getValue(when))
					{
						result = period->getEnd(when);
					}
				}
			}
			else
			{
				// Then refer to the simple start/end date of the scenario
				if ((sentScenario->getPeriodStart().is_not_a_date_time() || sentScenario->getPeriodStart() >= when) &&
					(sentScenario->getPeriodEnd().is_not_a_date_time() || sentScenario->getPeriodEnd() <= when))
				{
					result = sentScenario->getPeriodEnd();
				}
			}
			return result;
		}

		
		const Scenario*
		Alarm::getScenario() const
		{
			return get<ParentScenario>().get_ptr();
		}

		const ScenarioCalendar*
		Alarm::getCalendar() const
		{
			return get<Calendar>().get_ptr();
		}


		const MessagesSection*
		Alarm::getSection() const
		{
			return get<MessagesSection>().get_ptr();
		}
		

		void
		Alarm::setScenario(const Scenario* value)
		{
			set<ParentScenario>(value
								? boost::optional<Scenario&>(*const_cast<Scenario*>(value))
								: boost::none);
		}

		
		void
		Alarm::setSection(const MessagesSection* value)
		{
			set<MessagesSection>(value
								 ? boost::optional<MessagesSection&>(*const_cast<MessagesSection*>(value))
								 : boost::none);
		}

		
		void
		Alarm::setCalendar(const ScenarioCalendar* value)
		{
			set<Calendar>(value
						  ? boost::optional<ScenarioCalendar&>(*const_cast<ScenarioCalendar*>(value))
						  : boost::none);
		}

		
		void Alarm::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if (get<ParentScenario>())
			{
				get<ParentScenario>()->addMessage(*this);
				if (!belongsToTemplate())
				{
					MessagesModule::UpdateActivatedMessages();
				}
			}
		}


		void Alarm::unlink()
		{
			if (get<ParentScenario>())
			{
				get<ParentScenario>()->removeMessage(*this);
				if (!belongsToTemplate())
				{
					MessagesModule::UpdateActivatedMessages();
				}
			}
		}

		
		
}	}

