
//////////////////////////////////////////////////////////////////////////////////////////
///	MailService class implementation.
///	@file MailService.cpp
///	@author Gael Sauvanet
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "MailService.hpp"

#include "EMail.h"
#include "RequestException.h"
#include "Request.h"
#include "DynamicRequest.h"
#include "ServerModule.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,cms::MailService>::FACTORY_KEY = "mail";

	namespace cms
	{
		const string MailService::PARAMETER_RECIPIENTS = "recipients";
		const string MailService::PARAMETER_SENDER_MAIL = "sender_mail";
		const string MailService::PARAMETER_SENDER_NAME = "sender_name";
		const string MailService::PARAMETER_SUBJECT = "subject";
		const string MailService::PARAMETER_CONTENT = "content";
		const string MailService::PARAMETER_HTML = "html";

		const string MailService::ATTR_RESULT = "result";

		ParametersMap MailService::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_RECIPIENTS, serializeRecipients(_recipients));
			map.insert(PARAMETER_SENDER_MAIL, _senderMail);
			map.insert(PARAMETER_SENDER_NAME, _senderName);
			map.insert(PARAMETER_SUBJECT, _subject);
			map.insert(PARAMETER_CONTENT, _content);
			map.insert(PARAMETER_HTML, _isHtml);
			return map;
		}



		void MailService::_setFromParametersMap(const ParametersMap& map)
		{
			_recipients = unserializeRecipients(map.getDefault<string>(PARAMETER_RECIPIENTS));
			_senderMail = map.getDefault<string>(PARAMETER_SENDER_MAIL);
			_senderName = map.getDefault<string>(PARAMETER_SENDER_NAME);
			_subject = map.getDefault<string>(PARAMETER_SUBJECT);
			_content = map.getDefault<string>(PARAMETER_CONTENT);
			_isHtml = map.getDefault<bool>(PARAMETER_HTML, false);
		}



		ParametersMap MailService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			// MailService is not available from direct http requests
			if(!dynamic_cast<const MailService*>(request.getFunction().get()))
			{
				try
				{
					EMail email(ServerModule::GetEMailSender());
					if(_isHtml)
					{
						email.setFormat(EMail::EMAIL_HTML);
					}
					else
					{
						email.setFormat(EMail::EMAIL_TEXT);
					}
					
					email.setSender(_senderMail);
					email.setSenderName(_senderName);
					email.setSubject(_subject);
					BOOST_FOREACH(const Recipients::value_type& recipient, _recipients)
					{
						email.addRecipient(recipient.first, recipient.second);
					}
					email.setContent(_content);
					email.send();

					map.insert(ATTR_RESULT, true);
				}
				catch(boost::system::system_error)
				{
					map.insert(ATTR_RESULT, false);
				}
			}
			else
			{
				map.insert(ATTR_RESULT, false);
			}

			return map;
		}



		bool MailService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string MailService::getOutputMimeType() const
		{
			return "text/plain";
		}



		std::string MailService::serializeRecipients(const Recipients recipients) 
		{
			std::stringstream s;
			bool first(true);
			BOOST_FOREACH(const Recipients::value_type& recipient, recipients)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ";";
				}
				if(recipient.second != "")
				{
					s << recipient.second << ":" << recipient.first;
				}
				else
				{
					s << recipient.first;
				}				
			}
			return s.str();
		}



		MailService::Recipients MailService::unserializeRecipients(const std::string recipientsString) 
		{
			Recipients recipients;

			std::vector<std::string> recipientsVector;
			boost::algorithm::split(recipientsVector, recipientsString, boost::is_any_of(";"));
			BOOST_FOREACH(const std::string& recipientString, recipientsVector)
			{
				std::vector<std::string> recipientVector;
				boost::algorithm::split(recipientVector, recipientString, boost::is_any_of(":"));
				if(recipientVector.size() > 1)
				{
					recipients.push_back(Recipients::value_type(recipientVector[1],recipientVector[0]));
				}
				else
				{
					recipients.push_back(Recipients::value_type(recipientVector[0],string()));
				}
			}
			return recipients;
		}
}	}
