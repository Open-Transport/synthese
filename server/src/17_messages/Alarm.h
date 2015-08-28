
/** Alarm class header.
	@file Alarm.h

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

#ifndef SYNTHESE_MESSAGES_ALARM_H
#define SYNTHESE_MESSAGES_ALARM_H

#include "PointerField.hpp"

#include "ImportableTemplate.hpp"
#include "MessagesTypes.h"
#include "Registrable.h"

#include <string>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace messages
	{
		class AlarmObjectLink;
		class BroadcastPoint;
		class MessageAlternative;
		class MessageType;
		class MessagesSection;
		class Scenario;
		class ScenarioCalendar;

		class Alarm:
			public virtual util::Registrable,
			public PointerField<Alarm, Alarm>,
			public impex::ImportableTemplate<Alarm>
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<Alarm>	Registry;

			static const std::string DATA_MESSAGE_ID;
			static const std::string DATA_CONTENT;
			static const std::string DATA_PRIORITY;
			static const std::string DATA_SCENARIO_ID;
			static const std::string DATA_SCENARIO_NAME;
			static const std::string DATA_TITLE;
			static const std::string DATA_DONE;
			static const std::string DATA_DISPLAY_DURATION;
			static const std::string DATA_DIGITIZED_VERSION;
			static const std::string DATA_TAGS;
			static const std::string DATA_REPEAT_INTERVAL;
			static const std::string DATA_WITH_ACK;
			static const std::string DATA_MULTIPLE_STOPS;
			static const std::string DATA_PLAY_TTS;
			static const std::string DATA_LIGHT;
			static const std::string DATA_DIRECTION_SIGN_CODE;
			static const std::string DATA_START_STOP_POINT;
			static const std::string DATA_END_STOP_POINT;
			static const std::string DATA_REPEAT_INTERVAL;
			static const std::string DATA_WITH_ACK;
			static const std::string DATA_MULTIPLE_STOPS;
			static const std::string DATA_PLAY_TTS;
			static const std::string DATA_LIGHT;
			static const std::string DATA_DIRECTION_SIGN_CODE;
			static const std::string DATA_START_STOP_POINT;
			static const std::string DATA_END_STOP_POINT;

			static const std::string TAG_MESSAGE_ALTERNATIVE;
			static const std::string TAG_RECIPIENTS;
			static const std::string TAG_SECTION;
			static const std::string ATTR_LINK_PARAMETER;
			static const std::string ATTR_LINK_ID;
			static const std::string ATTR_CALENDAR_ID;

			typedef std::map<
				MessageType*,
				MessageAlternative*
			> MessageAlternatives;
			
			typedef std::map<
				std::string,
				std::set<
					const AlarmObjectLink*
			>	> LinkedObjects;

		protected:
			AlarmLevel			_level;
			std::string			_shortMessage;  //!< Alarm message
			std::string			_longMessage;  //!< Alarm message
			const Scenario* 	_scenario;
			bool				_rawEditor;
			bool				_done;
			const MessagesSection* _section;
			const ScenarioCalendar* _calendar;
			std::size_t			_displayDuration;
			std::string			_digitizedVersion;
			std::set<std::string> _tags;
			int					_repeatInterval;
			bool				_withAck;
			bool				_multipleStops;
			bool				_playTts;
			bool				_light;
			int					_directionSignCode;
			int					_startStopPoint;
			int					_endStopPoint;

			// Links
			//@{
				mutable MessageAlternatives _messageAlternatives;
				mutable LinkedObjects _linkedObjects;
				mutable boost::mutex _linkedObjectsMutex;
			//@}

			Alarm(
				util::RegistryKeyType key,
				const Scenario* scenario
			);
			Alarm(const Alarm& source);
			Alarm(
				const Alarm& source,
				const Scenario* scenario,
				const ScenarioCalendar* calendar
			);

			bool _isOnBroadcastPoint(
				const BroadcastPoint& point,
				const util::ParametersMap& parameters
			) const;

		public:
			virtual ~Alarm();

			//! @name Getters
			//@{
			const std::string&		getShortMessage()	const { return _shortMessage; }
				const std::string&		getLongMessage()	const { return _longMessage; }
				const AlarmLevel&		getLevel()			const { return _level; }
				const Scenario*			getScenario()		const { return _scenario; }
				bool					getRawEditor() const { return _rawEditor; }
				bool					getDone() const { return _done; }
				const MessageAlternatives& getMessageAlternatives() const { return _messageAlternatives; }
				const MessagesSection* getSection() const { return _section; }
				const ScenarioCalendar* getCalendar() const { return _calendar; }
				std::size_t				getDisplayDuration() const { return _displayDuration;}
				const std::string&		getDigitizedVersion()	const { return _digitizedVersion; }
				const std::set<std::string>& getTags() const { return _tags; }
				int						getRepeatInterval() const { return _repeatInterval; }
				bool					getWithAck() const { return _withAck; }
				bool					getMultipleStops() const { return _multipleStops; }
				bool					getPlayTts() const { return _playTts; }
				bool					getLight() const { return _light; }
				int						getDirectionSignCode() const { return _directionSignCode; }
				int						getStartStopPoint() const { return _startStopPoint; }
				int						getEndStopPoint() const { return _endStopPoint; }
			//@}

			//! @name Setters
			//@{
				void setLevel (const AlarmLevel& level){ _level = level; }
				void setShortMessage( const std::string& message){ _shortMessage = message; }
				void setLongMessage( const std::string& message){ _longMessage = message; }
				void setScenario(const Scenario* scenario){ _scenario = scenario; }
				void setRawEditor(bool value){ _rawEditor = value; }
				void setDone(bool value){ _done = value; }
				void setMessageAlternatives(const MessageAlternatives& value) const { _messageAlternatives = value; }
				void addLinkedObject(const AlarmObjectLink& link) const;
				void removeLinkedObject(const AlarmObjectLink& link) const;
				void setSection(const MessagesSection* value){ _section = value; }
				void setCalendar(const ScenarioCalendar* value){ _calendar = value; }
				void setDisplayDuration(std::size_t value){ _displayDuration = value; }
				void setDigitizedVersion( const std::string& value){ _digitizedVersion = value; }
				void setTags(const std::set<std::string> tags) { _tags = tags; }
				void setRepeatInterval(int value){ _repeatInterval = value; }
				void setWithAck(bool value){ _withAck = value; }
				void setMultipleStops(bool value){ _multipleStops = value; }
				void setPlayTts(bool value){ _playTts = value; }
				void setLight(bool value){ _light = value; }
				void setDirectionSignCode(int value){ _directionSignCode = value; }
				void setStartStopPoint(int value){ _startStopPoint = value; }
				void setEndStopPoint(int value){ _endStopPoint = value; }
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Message CMS exporter.
				/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Messages_in_CMS
				//////////////////////////////////////////////////////////////////////////
				/// @param pm parameters map to populate
				/// @param prefix prefix to add to the field names
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withScenario,
					std::string prefix = std::string(),
					bool withRecipients = false
				) const;



				static void LinkedObjectsToParametersMap(
					const LinkedObjects& linkedObjects,
					util::ParametersMap& pm
				);


				virtual bool isOnBroadcastPoint(
					const BroadcastPoint& point,
					const util::ParametersMap& parameters
				) const;


				LinkedObjects::mapped_type getLinkedObjects(
					const std::string& recipientKey
				) const;


				LinkedObjects & getLinkedObjects() const
				{
					return _linkedObjects;
				}			

			//@}
		};
}	}

#endif
