/** SmtpNotificationChannel implementation
	@file SmtpNotificationChannel.cpp
	@author Yves Martin
	@date 2015

	This Smtp belongs to the SYNTHESE project (public transportation specialized software)
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
#include <EMail.h>
#include <NotificationEvent.hpp>
#include <NotificationProvider.hpp>
#include <Object.hpp>
#include <ParametersMapField.hpp>
#include <SmtpNotificationChannel.hpp>
#include <ServerModule.h>

#include <boost/algorithm/string.hpp>
#include <boost/optional/optional.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace messages;

	namespace util
	{
		template<> const string FactorableTemplate<NotificationChannel, SmtpNotificationChannel>::FACTORY_KEY("Email");
	}

	namespace messages
	{
		// Parameter name for Email sender address
		const string SmtpNotificationChannel::PARAMETER_EMAIL_SENDER_ADDRESS = "email_sender_address";

		// Parameter name for Email sender name
		const string SmtpNotificationChannel::PARAMETER_EMAIL_SENDER_NAME = "email_sender_name";

		// Parameter name for Email recipients
		const string SmtpNotificationChannel::PARAMETER_EMAIL_RECIPIENTS = "email_recipients";

		// Delimiters for Email recipients
		const string SmtpNotificationChannel::EMAIL_RECIPIENTS_DELIMITERS = ",;";

		// Parameter name for Email content type (TEXT or HTML)
		const string SmtpNotificationChannel::PARAMETER_EMAIL_CONTENT_TYPE = "email_content_type";

		// Parameter name for CMS field with Email subject
		const string SmtpNotificationChannel::PARAMETER_EMAIL_SUBJECT = "email_subject";

		// Parameter name for CMS field with Email content
		const string SmtpNotificationChannel::PARAMETER_EMAIL_CONTENT = "email_content";



		// Provide its own script fields list
		std::vector<std::string> SmtpNotificationChannel::_getScriptParameterNames() const
		{
			vector<string> result;
			result.push_back(PARAMETER_EMAIL_SUBJECT);
			result.push_back(PARAMETER_EMAIL_CONTENT);
			result.push_back(PARAMETER_EMAIL_RECIPIENTS);
			return result;
		}



		bool SmtpNotificationChannel::notifyEvent(const boost::shared_ptr<NotificationEvent> event)
		{
			const NotificationProvider* provider = &(*(event->get<NotificationProvider>()));
			const Alarm* alarm = &(*(event->get<Alarm>()));
			// Generate field values
			ParametersMap scriptFields = generateScriptFields(provider, alarm, event->get<EventType>());

			if (!(provider->get<Parameters>().isDefined(PARAMETER_EMAIL_SENDER_ADDRESS))
				|| !scriptFields.isDefined(PARAMETER_EMAIL_RECIPIENTS)
				|| !scriptFields.isDefined(PARAMETER_EMAIL_SUBJECT)
				|| !scriptFields.isDefined(PARAMETER_EMAIL_CONTENT))
			{
				return true;	// Explicitly nothing to notify
			}

			string recipients = scriptFields.get<string>(PARAMETER_EMAIL_RECIPIENTS);
			string subject = scriptFields.get<string>(PARAMETER_EMAIL_SUBJECT);
			string content = scriptFields.get<string>(PARAMETER_EMAIL_CONTENT);
			if (recipients.empty() || subject.empty() || content.empty())
			{
				return true;	// Explicitly nothing to notify
			}

			// The e-mail
			util::EMail email(
				server::ServerModule::GetEMailSender()
			);
			email.setSubject(subject);
			email.setContent(content);
			if (provider->get<Parameters>().getDefault<int>(PARAMETER_EMAIL_CONTENT_TYPE, 0) == 1)
			{
				email.setFormat(util::EMail::EMAIL_HTML);
			}
			else
			{
				email.setFormat(util::EMail::EMAIL_TEXT);
			}
			email.setSender(provider->get<Parameters>().get<string>(PARAMETER_EMAIL_SENDER_ADDRESS));
			email.setSenderName(provider->get<Parameters>().getDefault<string>(PARAMETER_EMAIL_SENDER_NAME, ""));

			// Split with "," and ";" separators and loop on each recipient
			std::vector<std::string> recipientAddresses;
			boost::split(recipientAddresses, recipients, boost::is_any_of(EMAIL_RECIPIENTS_DELIMITERS));

			BOOST_FOREACH(const string& aRecipientAddress, recipientAddresses)
			{
				if(!aRecipientAddress.empty())
				{
					// Add the recipient
					email.addRecipient(aRecipientAddress);
				}
			}

			// Send the e-mail to its recipients
			email.send();

			return true;
		}
	}
}
