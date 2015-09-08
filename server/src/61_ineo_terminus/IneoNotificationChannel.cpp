/** IneoNotificationChannel implementation
	@Ineo IneoNotificationChannel.cpp
	@author Camille Hue
	@date 2015

	This Http belongs to the SYNTHESE project (public transportation specialized software)
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

#include <CMSScript.hpp>
#include <FactorableTemplate.h>
#include <IneoNotificationChannel.hpp>
#include <IneoTerminusConnection.hpp>
#include <IneoTerminusModule.hpp>
#include <IneoTerminusLog.hpp>
#include <ParametersMap.h>
#include <Webpage.h>
#include <MessageApplicationPeriod.hpp>
#include <IneoFileFormat.hpp>

#include <string>

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace cms;
	using namespace ineo_terminus;
	using namespace util;
	using namespace messages;
	using namespace server;

	namespace util
	{
		template<> const std::string FactorableTemplate<NotificationChannel, IneoNotificationChannel>::FACTORY_KEY("Ineo SAE");
	}

	namespace ineo_terminus
	{
		const std::string IneoNotificationChannel::PARAMETER_CMS_INTERPRETER = "cms_interpreter_id";
		const std::string IneoNotificationChannel::PARAMETER_INEO_MESSAGE_TYPE = "ineo_message_type";
		const std::string IneoNotificationChannel::PARAMETER_NEEDS_REPEAT_INTERVAL = "needs_repeat_interval";
		const std::string IneoNotificationChannel::PARAMETER_NEEDS_WITH_ACK = "needs_with_ack";
		const std::string IneoNotificationChannel::PARAMETER_NEEDS_MULTIPLE_STOPS = "needs_multiple_stops";
		const std::string IneoNotificationChannel::PARAMETER_NEEDS_PLAY_TTS = "needs_play_tts";
		const std::string IneoNotificationChannel::PARAMETER_NEEDS_LIGHT = "needs_light";
		const std::string IneoNotificationChannel::PARAMETER_NEEDS_DIRECTION_SIGN_CODE = "needs_direction_sign_code";
		const std::string IneoNotificationChannel::PARAMETER_NEEDS_START_STOP_POINT = "needs_start_stop_point";
		const std::string IneoNotificationChannel::PARAMETER_NEEDS_END_STOP_POINT = "needs_end_stop_point";

		// Provide its own script fields list
		std::vector<std::string> IneoNotificationChannel::_getScriptParameterNames() const
		{
			vector<string> result;
			result.push_back(PARAMETER_CMS_INTERPRETER);
			result.push_back(PARAMETER_INEO_MESSAGE_TYPE);
			result.push_back(PARAMETER_NEEDS_REPEAT_INTERVAL);
			result.push_back(PARAMETER_NEEDS_WITH_ACK);
			result.push_back(PARAMETER_NEEDS_MULTIPLE_STOPS);
			result.push_back(PARAMETER_NEEDS_PLAY_TTS);
			result.push_back(PARAMETER_NEEDS_LIGHT);
			result.push_back(PARAMETER_NEEDS_DIRECTION_SIGN_CODE);
			result.push_back(PARAMETER_NEEDS_START_STOP_POINT);
			result.push_back(PARAMETER_NEEDS_END_STOP_POINT);
			return result;
		}


		bool IneoNotificationChannel::notifyEvent(const boost::shared_ptr<NotificationEvent> event)
		{
			const NotificationProvider* provider = &(*(event->get<NotificationProvider>()));
			const Alarm* alarm = &(*(event->get<Alarm>()));

			// Create XML from parameters
			ParametersMap fields = generateScriptFields(
				provider,
				alarm,
				event->get<EventType>(),
				event->get<Time>()
			);

			// Retrieve alternative message for this event
			boost::optional<MessageType&> messageType = (END == event->get<EventType>()) ? provider->get<MessageTypeEnd>() : provider->get<MessageTypeBegin>();
			bool messageTypeFound = false;
			std::string messageText = _getMessageAlternative(alarm, messageType, messageTypeFound);
			fields.insert("message_text", messageText);

			shared_ptr<ParametersMap> messagePM(new ParametersMap);
			alarm->toParametersMap(*messagePM, true, "", true);
			fields.insert("message", messagePM);

			shared_ptr<ParametersMap> periodPM(new ParametersMap);
			if(NULL != alarm->getCalendar())
			{
				BOOST_FOREACH(
					const ScenarioCalendar::ApplicationPeriods::value_type& period,
					alarm->getCalendar()->getApplicationPeriods()
				){
					period->toParametersMap(*periodPM, false, false, "");
				}
			}
			fields.insert("period", periodPM);

			string requestName = provider->getName();
			if (event->get<EventType>() == BEGIN)
			{
				fields.insert("type", string("Create"));
				requestName += "Create";
			}
			else if (event->get<EventType>() == END)
			{
				fields.insert("type", string("Delete"));
				requestName += "Delete";
			}
			fields.insert("messagerie", provider->getName());
			string requestID = lexical_cast<string>(IneoTerminusConnection::GetTheConnection()->getNextRequestID());
			fields.insert("ID", requestID);
			fields.insert("network_id", IneoTerminusModule::GetParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_NETWORK));
			fields.insert("datasource_id", IneoTerminusModule::GetParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_DATASOURCE));
			fields.insert("xsd_location", IneoTerminusModule::GetParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_XSD_LOCATION));
			fields.insert("ineo_stop_point_prefix", synthese::data_exchange::IneoFileFormat::Importer_::MNLP_PREFIX);

			if (provider->get<Parameters>().getDefault<RegistryKeyType>(PARAMETER_CMS_INTERPRETER, 0))
			{
				const cms::Webpage* interpreter = Env::GetOfficialEnv().get<Webpage>(provider->get<Parameters>().get<RegistryKeyType>(PARAMETER_CMS_INTERPRETER)).get();
				stringstream message;
				interpreter->display(message, fields);

				IneoTerminusConnection::GetTheConnection()->addMessage(message.str());
				// Log the message in db_log
				IneoTerminusLog::AddIneoTerminusInfoMessageEntry(requestName, requestID, alarm->getShortMessage());

				return true;
			}
			else
			{
				// Il faut un interpreter
				util::Log::GetInstance().warn("Interpreter n'existe pas pour cette messagerie (" + provider->getName() + ")");
				return false;
			}
		}

	}
}
