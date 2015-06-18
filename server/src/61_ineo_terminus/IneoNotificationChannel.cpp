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

#include <FactorableTemplate.h>
#include <IneoNotificationChannel.hpp>
#include <ParametersMap.h>

#include <string>

using namespace boost;
using namespace std;

namespace synthese
{
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

			util::Log::GetInstance().debug("Ajout d'un message XML dans la pile à envoyer à Ineo");

			stringstream message;
			stringstream dateMessage;
			ptime now(second_clock::local_time());
			dateMessage << setw( 2 ) << setfill ( '0' ) <<
				now.date().day() << "/" <<
				setw( 2 ) << setfill ( '0' ) <<
				static_cast<long>(now.date().month()) << "/" <<
				setw( 2 ) << setfill ( '0' ) <<
				now.date().year();
			stringstream heureMessage;
			heureMessage << setw( 2 ) << setfill ( '0' ) <<
				now.time_of_day().hours() << ":" <<
				setw( 2 ) << setfill ( '0' ) <<
				now.time_of_day().minutes () << ":" <<
				setw( 2 ) << setfill ( '0' ) <<
				now.time_of_day().seconds();
			string requestName = provider->getName();
			if (event->get<EventType>() == BEGIN)
			{
				requestName += "Create";
			}
			else if (event->get<EventType>() == END)
			{
				requestName += "Delete";
			}
			requestName += "MessageRequest";
			message << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>" << char(10) <<
				"<" << requestName << ">" << char(10) <<
				"<ID>" <<
				lexical_cast<string>(IneoTerminusConnection::GetNextRequestID()) <<
				"</ID>" << char(10) <<
				"<RequestTimeStamp>" <<
				dateMessage.str() <<
				" " <<
				heureMessage.str() <<
				"</RequestTimeStamp>" << char(10) <<
				"<RequestorRef>Terminus</RequestorRef>" << char(10) <<
				"<Messaging>" << char(10) <<
				"<Name>" <<
				alarm->getShortMessage() <<
				"</Name>" << char(10) <<
				"<Dispatching>Repete</Dispatching>" << char(10) << // TO-DO : depends on the request
				"<StartDate>01/01/1970</StartDate>" << char(10) <<
				"<StopDate>31/12/2199</StopDate>" << char(10) <<
				"<StartTime>00:00:00</StartTime>" << char(10) <<
				"<StopTime>23:59:00</StopTime>" << char(10) <<
				"<RepeatPeriod>" << // TO-DO : only write it if provider needs_repeat_interval
				lexical_cast<string>(alarm->getRepeatInterval()) <<
				"</RepeatPeriod>" << char(10) <<
				"<Inhibition>non</Inhibition>" << char(10) << // TO-DO : depends on the request
				"<Color>Vert</Color>" << char(10) << // TO-DO : extract from parameters and alarm
				"<Text>" << char(10) <<
				alarm->getLongMessage() << char(10) << //TO-DO : split it and write <Line></Line> tags (for the moment they must be written in the message)
				"</Text>" << char(10) <<
				"<Recipients>" << char(10) <<
				"<AllNetwork />" << char(10) << // TO-DO : extract from alarm object links
				"</Recipients>" << char(10) <<
				"</Messaging>" << char(10) <<
				"</" << requestName << ">";

			IneoTerminusConnection::GetTheConnection()->addMessage(message.str());

			return true;
		}

	}
}
