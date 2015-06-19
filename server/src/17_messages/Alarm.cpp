
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
#include "BroadcastPoint.hpp"
#include "DBConstants.h"
#include "Factory.h"
#include "MessageAlternative.hpp"
#include "MessagesSection.hpp"
#include "Scenario.h"
#include "AlarmTemplate.h"
#include "ParametersMap.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace impex;

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

		const string Alarm::TAG_MESSAGE_ALTERNATIVE = "message_alternative";
		const string Alarm::TAG_RECIPIENTS = "recipients";
		const string Alarm::TAG_SECTION = "section";

		const string Alarm::ATTR_LINK_ID = "link_id";
		const string Alarm::ATTR_LINK_PARAMETER = "link_parameter";
		const string Alarm::ATTR_CALENDAR_ID = "calendar_id";


		Alarm::Alarm(
			util::RegistryKeyType key,
			const Scenario* scenario
		):	Registrable(key),
			_level(ALARM_LEVEL_INFO),
			_scenario(scenario),
			_rawEditor(false),
			_done(true),
			_section(NULL),
			_calendar(NULL),
			_displayDuration(0)
		{}



		Alarm::Alarm(
			const Alarm& source
		):	Registrable(0),
			_level(source._level),
			_shortMessage(source._shortMessage),
			_longMessage(source._longMessage),
			_scenario(source._scenario),
			_rawEditor(source._rawEditor),
			_done(source._done),
			_section(source._section),
			_calendar(source._calendar),
			_displayDuration(source._displayDuration),
			_digitizedVersion(source._digitizedVersion),
			_tags(source._tags)
		{}



		Alarm::Alarm(
			const Alarm& source,
			const Scenario* scenario,
			const ScenarioCalendar* calendar
		):	Registrable(0),
			_level(source._level),
			_shortMessage(source._shortMessage),
			_longMessage(source._longMessage),
			_scenario(scenario),
			_rawEditor(source._rawEditor),
			_done(source._done),
			_section(source._section),
			_calendar(calendar),
			_displayDuration(source._displayDuration),
			_digitizedVersion(source._digitizedVersion),
			_tags(source._tags)
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
			if(_section)
			{
				boost::shared_ptr<ParametersMap> sectionPM(new ParametersMap);
				_section->toParametersMap(*sectionPM, true);
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

			pm.insert(ATTR_CALENDAR_ID, _calendar ? _calendar->getKey() : 0);

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
			return _isOnBroadcastPoint(point, parameters);
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



		// Class documentation
		/** @class Alarm
			Alarm message.
			@ingroup m17

			An alarm message is intended to be broadcasted at a time period into several destinations :
				- display screens
				- route planner results
				- etc.

			An alarm can be sent individually (single alarm) or in a group built from a scenario (grouped alarm)
			The _scenario attribute points to the group if applicable.
		*/
}	}

