
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
#include "PointerField2.hpp"
#include "EnumObjectField.hpp"
#include "DataSourceLinksField.hpp"
#include "PtimeField.hpp"

#include "ImportableTemplate.hpp"
#include "MessagesTypes.h"
#include "MessagesSection.hpp"
#include "ScenarioCalendar.hpp"
#include "Scenario.h"

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

		FIELD_ENUM(Level, AlarmLevel)
		FIELD_STRING(ShortMessage)
		FIELD_STRING(LongMessage)
		FIELD_POINTER2(ParentScenario, Scenario)
		FIELD_BOOL(RawEditor)
		FIELD_BOOL(Done)
		FIELD_POINTER(Calendar, ScenarioCalendar)
		FIELD_SIZE_T(DisplayDuration)
		FIELD_STRING(DigitizedVersion)
		FIELD_STRING(Tags)
		FIELD_INT(RepeatInterval)
		FIELD_BOOL(WithAck)
		FIELD_BOOL(MultipleStops)
		FIELD_BOOL(PlayTts)
		FIELD_BOOL(Light)
		FIELD_INT(DirectionSignCode)
		FIELD_ID(StartStopPoint)
		FIELD_ID(EndStopPoint)
		FIELD_PTIME(LastActivationStart)
		FIELD_PTIME(LastActivationEnd)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Level),
			FIELD(ShortMessage),
			FIELD(LongMessage),
			FIELD(ParentScenario),
			FIELD(RawEditor),
			FIELD(Done),
			FIELD(messages::MessagesSection),
			FIELD(Calendar),
			FIELD(DataSourceLinksWithoutUnderscore),
			FIELD(DisplayDuration),
			FIELD(DigitizedVersion),
			FIELD(Tags),
			FIELD(RepeatInterval),
			FIELD(WithAck),
			FIELD(MultipleStops),
			FIELD(PlayTts),
			FIELD(Light),
			FIELD(DirectionSignCode),
			FIELD(StartStopPoint),
			FIELD(EndStopPoint),
			FIELD(LastUpdate),
			FIELD(LastActivationStart),
			FIELD(LastActivationEnd)
			> AlarmRecord;

		class Alarm:
			public Object<Alarm, AlarmRecord>,
			public impex::ImportableTemplate<Alarm>
		{
		public:
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
			static const std::string DATA_LAST_UPDATE;

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

			typedef std::map<
				std::pair<
					const BroadcastPoint*,
					util::ParametersMap
				>, bool
			> BroadcastPointsCache;

			mutable BroadcastPointsCache _broadcastPointsCache;

		protected:

			// Links
			//@{
				mutable MessageAlternatives _messageAlternatives;
				mutable LinkedObjects _linkedObjects;
				mutable boost::mutex _linkedObjectsMutex;
			//@}

			bool _isOnBroadcastPoint(
				const BroadcastPoint& point,
				const util::ParametersMap& parameters
			) const;

			/** Set last update timestamp to "now". */
			void updated();

		public:

			Alarm(
				util::RegistryKeyType key = 0);
			Alarm(const Alarm& source);

			virtual ~Alarm();

			//! @name Getters
			//@{
				virtual const MessageAlternatives& getMessageAlternatives() const { return _messageAlternatives; }

				virtual const std::string&		getShortMessage()	const { return get<ShortMessage>(); }
				virtual const std::string&		getLongMessage()	const { return get<LongMessage>(); }
				virtual const AlarmLevel&		getLevel()			const { return get<Level>(); }
				virtual const Scenario*			getScenario()		const;
				virtual const ScenarioCalendar* getCalendar() const;

				virtual const MessagesSection*	getSection() const;
				virtual bool					getRawEditor() const { return get<RawEditor>(); }
				virtual bool					getDone() const { return get<Done>(); }
				virtual std::size_t				getDisplayDuration() const { return get<DisplayDuration>(); }
				virtual const std::string&		getDigitizedVersion() const { return get<DigitizedVersion>(); }
				virtual const std::string&		getTags() const { return get<Tags>(); }
				virtual int						getRepeatInterval() const { return get<RepeatInterval>(); }
				virtual bool					getWithAck() const { return get<WithAck>(); }
				virtual bool					getMultipleStops() const { return get<MultipleStops>(); }
				virtual bool					getPlayTts() const { return get<PlayTts>(); }
				virtual bool					getLight() const { return get<Light>(); }
				virtual int						getDirectionSignCode() const { return get<DirectionSignCode>(); }
				virtual synthese::util::RegistryKeyType		getStartStopPoint() const { return get<StartStopPoint>(); }
				virtual synthese::util::RegistryKeyType		getEndStopPoint() const { return get<EndStopPoint>(); }
				const boost::posix_time::ptime getLastUpdate() const { return get<LastUpdate>(); }
				const boost::posix_time::ptime getLastActivationStart() const { return get<LastActivationStart>(); }
				const boost::posix_time::ptime getLastActivationEnd() const { return get<LastActivationEnd>(); }

			//@}

			//! @name Setters
			//@{
				virtual void setLevel (const AlarmLevel& level) { set<Level>(level); updated(); }
				virtual void setShortMessage( const std::string& message) { set<ShortMessage>(message); updated(); }
				virtual void setLongMessage( const std::string& message) { set<LongMessage>(message); updated(); }
				virtual void setScenario(const Scenario* scenario);
				virtual void setRawEditor(bool value) { set<RawEditor>(value); }
				virtual void setDone(bool value) { set<Done>(value); updated(); }
				
				virtual void setSection(const MessagesSection* value);
				virtual void setCalendar(const ScenarioCalendar* value);
				
				virtual void setDisplayDuration(std::size_t value) { set<DisplayDuration>(value); updated(); }
				virtual void setDigitizedVersion( const std::string& value) { set<DigitizedVersion>(value); updated(); }
				virtual void setTags( const std::string value) { set<Tags>(value); updated(); }
				virtual void setRepeatInterval(int value) { set<RepeatInterval>(value); updated(); }
				virtual void setWithAck(bool value) { set<WithAck>(value); updated(); }
				virtual void setMultipleStops(bool value) { set<MultipleStops>(value); updated(); }
				virtual void setPlayTts(bool value) { set<PlayTts>(value); updated(); }
				virtual void setLight(bool value) { set<Light>(value); updated(); }
				virtual void setDirectionSignCode(int value) { set<DirectionSignCode>(value); updated(); }
				virtual void setStartStopPoint(synthese::util::RegistryKeyType value) { set<StartStopPoint>(value); updated(); }
				virtual void setEndStopPoint(synthese::util::RegistryKeyType value) { set<EndStopPoint>(value); updated(); }
			//@}

			void setMessageAlternatives(const MessageAlternatives& value) { _messageAlternatives = value; updated(); }

			void addLinkedObject(const AlarmObjectLink& link);
			void removeLinkedObject(const AlarmObjectLink& link);

			bool belongsToTemplate() const;

			/**
				Checks if the current message is active at the specified time.
				@param when the time to check
				@return true if the message must be displayed at the specified time
			*/
			bool isApplicable(
				const boost::posix_time::ptime& when
			) const;

			/**
				Returns the start timestamp for the alarm period active at the specified time.
				If alarm is not applicable at "when", posix_time::not_a_date_time is returned.

				@param when the time to check
				@return start date for the application period or not_a_date_time if no start date
			*/
			boost::posix_time::ptime getApplicationStart(
				const boost::posix_time::ptime& when
			) const;

			/**
				Returns the end timestamp for the alarm period active at the specified time.
				If alarm is not applicable at "when", posix_time::not_a_date_time is returned.

				@param when the time to check
				@return end date for the application period or not_a_date_time if no end date
			*/
			boost::posix_time::ptime getApplicationEnd(
				const boost::posix_time::ptime& when
			) const;

			/**
				Test if Alarm has already been activated by user action.

				Specific case where LastActivationStart == LastActivationEnd
				returns false.

				@return true if current date after LastActivationStart but not after LastActivationEnd
			*/
			bool isActivated() const;

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();

				/** Alarm is now activated */
				void activationStarted();

				/** Alarm activation just ended */
				void activationEnded();
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
				
				void clearBroadcastPointsCache() const;

				LinkedObjects::mapped_type getLinkedObjects(
					const std::string& recipientKey
				) const;


				LinkedObjects & getLinkedObjects() const
				{
					return _linkedObjects;
				}
			//@}
		};

		typedef std::vector<boost::shared_ptr<Alarm> > Alarms;
}	}

#endif
