/** NotificationChannel abstract implementation
	@file NotificationChannel.cpp
	@author Yves Martin
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include <Alarm.h>
#include <AlarmObjectLink.h>
#include <CMSScript.hpp>
#include <CityTableSync.h>
#include <CommercialLine.h>
#include <CommercialLineTableSync.h>
#include <Env.h>
#include <MessageAlternative.hpp>
#include <MessageType.hpp>
#include <NamedPlace.h>
#include <NotificationChannel.hpp>
#include <NotificationLog.hpp>
#include <NotificationProvider.hpp>
#include <Object.hpp>
#include <ParametersMap.h>
#include <ParametersMapField.hpp>
#include <StopArea.hpp>
#include <StopAreaTableSync.hpp>
#include <TransportNetworkTableSync.h>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/foreach.hpp>
#include <boost/optional/optional.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <map>
#include <sstream>
#include <utility>

using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace messages;

	namespace messages
	{
		const string NotificationChannel::ATTR_KEY = "key";

		const string NotificationChannel::TIME_FACET_FORMAT = "%d-%m-%Y %H:%M";

		const string NotificationChannel::VARIABLE_ALARM_ID = "alarm_id";
		const string NotificationChannel::VARIABLE_SHORT_MESSAGE = "short_message";
		const string NotificationChannel::VARIABLE_MESSAGE = "message";
		const string NotificationChannel::VARIABLE_STOP_IDS = "stop_ids";
		const string NotificationChannel::VARIABLE_LINE_IDS = "line_ids";
		const string NotificationChannel::VARIABLE_ID_SEPARATOR = ",";
		const string NotificationChannel::VARIABLE_URL = "url";
		const string NotificationChannel::VARIABLE_APPLICATION_BEGIN = "application_begin";
		const string NotificationChannel::VARIABLE_APPLICATION_BEGIN_ISO = "application_begin_iso";
		const string NotificationChannel::VARIABLE_APPLICATION_END = "application_end";
		const string NotificationChannel::VARIABLE_APPLICATION_END_ISO = "application_end_iso";
		const string NotificationChannel::VARIABLE_EVENT_TYPE = "event_type";
		const string NotificationChannel::VARIABLE_EVENT_TIME = "event_time";
		const string NotificationChannel::VARIABLE_EVENT_TIME_ISO = "event_time_iso";


		//////////////////////////////////////////////////////////////////////////
		/// Exports the notification channel into a parameters map.
		/// @param pm the parameters map to populate
		void NotificationChannel::toParametersMap(
			util::ParametersMap& pm
		) const {
			pm.insert(ATTR_KEY, getFactoryKey());
		}



		// Default implementation for channel implementation with no script field
		std::vector<std::string> NotificationChannel::_getScriptParameterNames() const
		{
			vector<string> empty;
			return empty;
		}



		std::string NotificationChannel::_getMessageAlternative(
			const Alarm* alarm,
			const boost::optional<MessageType&> type,
			bool& messageTypeFound
		) {
			// Initialize message alternative with the long message of the alarm, in case we do not find the requested alternative
			std::string messageAlternative = alarm->getLongMessage();
			// If type is not set, set messageTypeFound to true
			messageTypeFound = !type.is_initialized();

			if(type)
			{
				util::RegistryKeyType typeKey = type.get().getKey();

				BOOST_FOREACH(const Alarm::MessageAlternatives::value_type& alternative, alarm->getMessageAlternatives())
				{
					const MessageType* alternativeType = alternative.first;
					const MessageAlternative* alternativeText = alternative.second;
					if(alternativeType->getKey() == typeKey)
					{
						// Alternative message found, return its content
						messageAlternative = alternativeText->getContent();
						messageTypeFound = true;
					}
				}
			}

			return messageAlternative;
		}


		bool NotificationChannel::_setMessageVariable(
			ParametersMap& variables,
			const Alarm* alarm,
			const boost::optional<MessageType&> type
		) {
			bool result = false;
			std::string message = _getMessageAlternative(alarm, type, result);
			variables.insert(VARIABLE_MESSAGE, message);
			return result;
		}



		string NotificationChannel::_formatTime(const boost::posix_time::ptime time)
		{
			static std::locale loc(
				std::cout.getloc(),
				new time_facet(TIME_FACET_FORMAT.data())
			);
			stringstream ss;
			ss.imbue(loc);
			ss << time;
			return ss.str();
		}



		void NotificationChannel::_setApplicationDateVariables(
			util::ParametersMap& variables,
			const Alarm* alarm,
			const NotificationType eventType,
			const ptime& eventTime
		) {
			const ptime begin = alarm->getApplicationStart(eventTime);

			if (!begin.is_not_a_date_time())
			{
				const string beginTime = _formatTime(begin);
				const string beginIso = to_iso_string(begin);
				variables.insert(VARIABLE_APPLICATION_BEGIN, beginTime);
				variables.insert(VARIABLE_APPLICATION_BEGIN_ISO, beginIso);
				if (eventType != END)
				{
					variables.insert(VARIABLE_EVENT_TIME, beginTime);
					variables.insert(VARIABLE_EVENT_TIME_ISO, beginIso);
				}
			}

			const ptime end = alarm->getApplicationEnd(eventTime);
			if (!end.is_not_a_date_time())
			{
				const string endTime = _formatTime(end);
				const string endIso = to_iso_string(end);
				variables.insert(VARIABLE_APPLICATION_END, endTime);
				variables.insert(VARIABLE_APPLICATION_END_ISO, endIso);
				if (eventType == END)
				{
					variables.insert(VARIABLE_EVENT_TIME, endTime);
					variables.insert(VARIABLE_EVENT_TIME_ISO, endIso);
				}
			}
		}



		void NotificationChannel::_setTestApplicationDateVariables(
			util::ParametersMap& variables,
			const boost::posix_time::ptime& eventTime
		) {
			string beginTime = _formatTime(eventTime);
			string beginIso = to_iso_string(eventTime);
			variables.insert(VARIABLE_APPLICATION_BEGIN, beginTime);
			variables.insert(VARIABLE_APPLICATION_BEGIN_ISO, beginIso);
			variables.insert(VARIABLE_EVENT_TIME, beginTime);
			variables.insert(VARIABLE_EVENT_TIME_ISO, beginIso);

			ptime end = eventTime + minutes(30);
			variables.insert(VARIABLE_APPLICATION_END, _formatTime(end));
			variables.insert(VARIABLE_APPLICATION_END_ISO, to_iso_string(end));
		}



		util::ParametersMap NotificationChannel::generateScriptFields(
			const NotificationProvider* provider,
			const Alarm* alarm,
			const NotificationType eventType,
			const boost::posix_time::ptime& eventTime
		) const {
			// Generate variables for rendering
			ParametersMap scriptParameters;

			scriptParameters.insert(VARIABLE_EVENT_TYPE, string(NotificationEvent::TYPE_NAMES[eventType]));
			scriptParameters.insert(VARIABLE_SHORT_MESSAGE, alarm->getShortMessage());
			scriptParameters.insert(VARIABLE_URL, alarm->getDigitizedVersion());

			// Add message alternative to parameters map
			// Use begin message type by default
			boost::optional<MessageType&> type = (END == eventType) ? provider->get<MessageTypeEnd>() : provider->get<MessageTypeBegin>();
			bool messageTypeFound = _setMessageVariable(scriptParameters, alarm, type);
			if (false == messageTypeFound)
			{
				const std::string details = "Type de message manquant: " + (type ? type->getName() : "indéfini");
				NotificationLog::AddNotificationProviderFailure(provider, details, alarm);
			}

			// Browse alarm linked objects to build lines and stops ID lists
			std::stringstream linesStream;
			bool firstLineInStream = true;
			BOOST_FOREACH(const AlarmObjectLink* link, alarm->getLinkedObjects("line"))
			{
				RegistryTableType tableId(decodeTableId(link->getObjectId()));
				if (tableId == pt::CommercialLineTableSync::TABLE.ID)
				{
					boost::shared_ptr<const pt::CommercialLine> line = Env::GetOfficialEnv().get<pt::CommercialLine>(link->getObjectId());
					if(line.get())
					{
						if(firstLineInStream)
						{
							firstLineInStream = false;
						}
						else
						{
							linesStream << VARIABLE_ID_SEPARATOR;
						}
						linesStream << line->getName();
					}
				}
				else if (tableId == pt::TransportNetworkTableSync::TABLE.ID)
				{
					boost::shared_ptr<const pt::TransportNetwork> network = Env::GetOfficialEnv().get<pt::TransportNetwork>(link->getObjectId());
					if(network.get())
					{
						if(firstLineInStream)
						{
							firstLineInStream = false;
						}
						else
						{
							linesStream << VARIABLE_ID_SEPARATOR;
						}
						linesStream << network->getName();
					}
				}
			}
			scriptParameters.insert(VARIABLE_LINE_IDS, linesStream.str());

			std::stringstream stopsStream;
			bool firstStopInStream = true;

			BOOST_FOREACH(const AlarmObjectLink* link, alarm->getLinkedObjects("stoparea"))
			{
				RegistryTableType tableId(decodeTableId(link->getObjectId()));
				if (tableId == pt::StopAreaTableSync::TABLE.ID)
				{
					boost::shared_ptr<const pt::StopArea> stop = Env::GetOfficialEnv().get<pt::StopArea>(link->getObjectId());
					if(stop.get())
					{
						if(firstStopInStream)
						{
							firstStopInStream = false;
						}
						else
						{
							stopsStream << VARIABLE_ID_SEPARATOR;
						}
						stopsStream << stop->getName();
					}
				}
				else if (tableId == geography::CityTableSync::TABLE.ID)
				{
					boost::shared_ptr<const geography::City> city = Env::GetOfficialEnv().get<geography::City>(link->getObjectId());
					if(city.get())
					{
						if(firstStopInStream)
						{
							firstStopInStream = false;
						}
						else
						{
							stopsStream << VARIABLE_ID_SEPARATOR;
						}
						stopsStream << city->getName();
					}
				}
			}
			scriptParameters.insert(VARIABLE_STOP_IDS, stopsStream.str());

			scriptParameters.insert(
				VARIABLE_ALARM_ID,
				boost::lexical_cast<std::string>(alarm->getKey())
			);

			// ApplicationPeriod only available for Alarm, not for library template
			if (dynamic_cast<const Alarm*>(alarm))
			{
				// for END notifications, event time is already out of period,
				// message activation waits for next minute, so go back few seconds
				ptime variablesTime =
					(eventType == END) ? eventTime - seconds(65) : eventTime;

				_setApplicationDateVariables(
					scriptParameters,
					static_cast<const Alarm*>(alarm),
					eventType,
					variablesTime
				);
			}
			else
			{
				_setTestApplicationDateVariables(scriptParameters, eventTime);
			}

			// Insert point for derived classes to add their own script parameters
			_addVariables(scriptParameters);

			// Render script fields
			ParametersMap result;
			ParametersMap providerParameters = provider->get<Parameters>();

			BOOST_FOREACH(std::string paramName, _getScriptParameterNames())
			{
				string code = providerParameters.get<string>(paramName);
				if (!code.empty()) {
					cms::CMSScript script(code);
					if (!script.empty()) {
						string value = script.eval(scriptParameters);
						if (!value.empty()) {
							result.insert(paramName, value);
						}
					}
				}
			}
			return result;
		}
	}



	ptime NotificationChannel::_getMessageUpdate(
		const Alarm* alarm,
		const boost::optional<MessageType&> type,
		bool& messageTypeFound
	) {
		// Initialize with alarm message update timestamp, in case we do not find the requested alternative
		ptime result = alarm->getLastUpdate();
		// If type is not set, set messageTypeFound to true
		messageTypeFound = !type.is_initialized();

		if(type)
		{
			util::RegistryKeyType typeKey = type.get().getKey();

			BOOST_FOREACH(const Alarm::MessageAlternatives::value_type& alternative, alarm->getMessageAlternatives())
			{
				const MessageType* alternativeType = alternative.first;
				const MessageAlternative* alternativeMessage = alternative.second;
				if(alternativeType->getKey() == typeKey)
				{
					// Alternative message found, return its content
					result = alternativeMessage->getLastUpdate();
					messageTypeFound = true;
				}
			}
		}
		return result;
	}



	bool NotificationChannel::checkForUpdate(const Alarm* alarm, boost::shared_ptr<NotificationEvent> beginEvent)
	{
		boost::optional<NotificationProvider&> provider = beginEvent->get<NotificationProvider>();
		boost::optional<MessageType&> type =
			(END == beginEvent->get<EventType>())
				? provider->get<MessageTypeEnd>()
				: provider->get<MessageTypeBegin>();
		bool messageTypeFound = false;
		ptime messageUpdate = _getMessageUpdate(alarm, type, messageTypeFound);

		if (false == messageTypeFound)
		{
			const std::string details = "Type de message manquant: " + (type ? type->getName() : "indéfini");
			NotificationLog::AddNotificationProviderFailure(&(provider.get()), details, alarm);
		}

		bool result = false;
		if (beginEvent->get<LastAttempt>().is_not_a_date_time())
		{
			result = (messageUpdate > beginEvent->get<Time>());
		}
		else
		{
			result = (messageUpdate > beginEvent->get<LastAttempt>());
		}
		return result;
	}

}
