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
#include <CommercialLine.h>
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



		bool NotificationChannel::_setMessageVariable(
			ParametersMap& variables,
			const Alarm* alarm,
			boost::optional<MessageType&> type
		) {
			bool result = true;
			if (type) {
				Alarm::MessageAlternatives::const_iterator it(
					alarm->getMessageAlternatives().find(&(*type))
				);
				if(it != alarm->getMessageAlternatives().end())
				{
					variables.insert(VARIABLE_MESSAGE, it->second->get<Content>());
					return true;
				}
				else
				{
					// lacking message alternative for channel
					result = false;
				}
			}
			variables.insert(VARIABLE_MESSAGE, alarm->getLongMessage());
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
			const SentAlarm* alarm,
			const NotificationType eventType,
			const ptime& now
		) {
			const ptime begin = alarm->getApplicationStart(now);

			if (!begin.is_not_a_date_time())
			{
				const string beginTime = _formatTime(begin);
				const string beginIso = to_iso_string(begin);
				variables.insert(VARIABLE_APPLICATION_BEGIN, beginTime);
				variables.insert(VARIABLE_APPLICATION_BEGIN_ISO, beginIso);
				if (eventType == BEGIN)
				{
					variables.insert(VARIABLE_EVENT_TIME, beginTime);
					variables.insert(VARIABLE_EVENT_TIME_ISO, beginIso);
				}
			}

			const ptime end = alarm->getApplicationEnd(now);
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
			util::ParametersMap& variables
		) {
			ptime begin = second_clock::local_time();
			string beginTime = _formatTime(begin);
			string beginIso = to_iso_string(begin);
			variables.insert(VARIABLE_APPLICATION_BEGIN, beginTime);
			variables.insert(VARIABLE_APPLICATION_BEGIN_ISO, beginIso);
			variables.insert(VARIABLE_EVENT_TIME, beginTime);
			variables.insert(VARIABLE_EVENT_TIME_ISO, beginIso);

			ptime end = begin + minutes(30);
			variables.insert(VARIABLE_APPLICATION_END, _formatTime(end));
			variables.insert(VARIABLE_APPLICATION_END_ISO, to_iso_string(end));
		}



		util::ParametersMap NotificationChannel::generateScriptFields(
			const NotificationProvider* provider,
			const Alarm* alarm,
			const NotificationType eventType
		) const {
			// Generate variables for rendering
			ParametersMap scriptParameters;

			if (eventType == BEGIN)
			{
				scriptParameters.insert(VARIABLE_EVENT_TYPE, "BEGIN");
			}
			else if (eventType == END)
			{
				scriptParameters.insert(VARIABLE_EVENT_TYPE, "END");
			}

			scriptParameters.insert(VARIABLE_SHORT_MESSAGE, alarm->getShortMessage());

			// Use begin message by default
			boost::optional<MessageType&> type = provider->get<MessageTypeBegin>();
			if (eventType == END)
			{
				type = provider->get<MessageTypeEnd>();
			}
			bool messageTypeFound = _setMessageVariable(scriptParameters, alarm, type);
			if (!messageTypeFound)
			{
				const std::string details = "Type de message manquant: " + type->getName();
				NotificationLog::AddNotificationProviderFailure(provider, details, alarm);
			}

			// Browse alarm linked objects to build lines and stops ID lists

			std::stringstream linesStream;
			bool firstLineInStream = true;
			BOOST_FOREACH(const AlarmObjectLink* link, alarm->getLinkedObjects("line"))
			{
				boost::shared_ptr<const pt::CommercialLine> line = Env::GetOfficialEnv().get<pt::CommercialLine>(link->getObjectId());
				if(line.get())
				{
					if(!firstLineInStream)
					{
						linesStream << VARIABLE_ID_SEPARATOR;
						firstLineInStream = false;
					}
					linesStream << line->getName();
				}
			}
			scriptParameters.insert(VARIABLE_LINE_IDS, linesStream.str());

			std::stringstream stopsStream;
			bool firstStopInStream = true;

			BOOST_FOREACH(const AlarmObjectLink* link, alarm->getLinkedObjects("stoparea"))
			{
				boost::shared_ptr<const pt::StopArea> stop = Env::GetOfficialEnv().get<pt::StopArea>(link->getObjectId());
				if(stop.get())
				{
					if(!firstStopInStream)
					{
						stopsStream << VARIABLE_ID_SEPARATOR;
						firstStopInStream = false;
					}
					stopsStream << stop->getName();
				}
			}
			scriptParameters.insert(VARIABLE_STOP_IDS, stopsStream.str());

			scriptParameters.insert(
				VARIABLE_ALARM_ID,
				boost::lexical_cast<std::string>(alarm->getKey())
			);

			// ApplicationPeriod only available for SentAlarm, not for library template
			if (dynamic_cast<const SentAlarm*>(alarm))
			{
				ptime now = second_clock::local_time();
				if (eventType == END)
				{
					now = now - seconds(65);
					// now is already out of period, message activation waits for next minute
					// In case of trouble, last attempt in NotificationEvent may be more accurate
				}
				_setApplicationDateVariables(
					scriptParameters,
					static_cast<const SentAlarm*>(alarm),
					eventType,
					now
				);
			}
			else
			{
				_setTestApplicationDateVariables(scriptParameters);
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
}
