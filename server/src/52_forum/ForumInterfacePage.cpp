
/** ForumInterfacePage class implementation.
	@file ForumInterfacePage.cpp

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

#include "ForumInterfacePage.hpp"
#include "DateTimeInterfacePage.h"
#include "ForumMessageTableSync.hpp"
#include "ForumTopicTableSync.hpp"
#include "User.h"
#include "Webpage.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace cms;
	using namespace util;

	namespace forum
	{
		const string ForumInterfacePage::DATA_TOPIC_ID("topic_id");
		const string ForumInterfacePage::DATA_TOPIC_NAME("topic_name");
		const string ForumInterfacePage::DATA_NAME("name");
		const string ForumInterfacePage::DATA_USER_NAME("user_name");
		const string ForumInterfacePage::DATA_USER_SURNAME("user_surname");
		const string ForumInterfacePage::DATA_USER_EMAIL("user_email");
		const string ForumInterfacePage::DATA_USER_ID("user_id");
		const string ForumInterfacePage::DATA_DATE("date");
		const string ForumInterfacePage::DATA_MESSAGES_NUMBER("messages_number");
		const string ForumInterfacePage::DATA_LAST_MESSAGE_DATE("last_message_date");
		const string ForumInterfacePage::DATA_CLOSED("closed");
		const string ForumInterfacePage::DATA_CONTENT("content");
		const string ForumInterfacePage::DATA_RANK("rank");
		const string ForumInterfacePage::DATA_RANK_IS_ODD("rank_is_odd");



		void ForumInterfacePage::DisplayTopic(
			std::ostream& stream,
			const cms::Webpage& page,
			boost::shared_ptr<const cms::Webpage> dateDisplayPage,
			const server::Request& request,
			const ForumTopic& topic,
			std::size_t rank
		){
			ParametersMap pm(request.getFunction()->getTemplateParameters());

			Env env;
			ForumMessageTableSync::SearchResult messages(
				ForumMessageTableSync::Search(
					env,
					topic.getKey(),
					optional<string>(),
					optional<RegistryKeyType>(),
					true,
					0,
					optional<size_t>(),
					true,
					false
			)	);
			pm.insert(DATA_MESSAGES_NUMBER, messages.size());

			pm.insert(Request::PARAMETER_OBJECT_ID, topic.getKey());
			pm.insert(DATA_NAME, topic.getName());
			if(dateDisplayPage.get())
			{
				stringstream content;
				DateTimeInterfacePage::Display(content, dateDisplayPage, request, topic.getDate());
				pm.insert(DATA_DATE, content.str());
			}
			else
			{
				pm.insert(DATA_DATE, lexical_cast<string>(topic.getDate()));
			}

			if(!messages.empty())
			{
				boost::shared_ptr<ForumMessage> message(messages.front());

				if(dateDisplayPage.get())
				{
					stringstream content;
					DateTimeInterfacePage::Display(content, dateDisplayPage, request, message->getDate());
					pm.insert(DATA_LAST_MESSAGE_DATE, content.str());
				}
				else
				{
					pm.insert(DATA_LAST_MESSAGE_DATE, lexical_cast<string>(message->getDate()));
				}
			}

			pm.insert(DATA_CLOSED, topic.getClosed());

			if(topic.getUser())
			{
				pm.insert(DATA_USER_ID, topic.getUser()->getKey());
				pm.insert(DATA_USER_EMAIL, topic.getUser()->getEMail());
				pm.insert(DATA_USER_NAME, topic.getUser()->getName());
				pm.insert(DATA_USER_SURNAME, topic.getUser()->getSurname());
			}
			else
			{
				pm.insert(DATA_USER_NAME, topic.getUserName());
				pm.insert(DATA_USER_EMAIL, topic.getUserEMail());
			}

			pm.insert(DATA_RANK, rank);
			pm.insert(DATA_RANK_IS_ODD, rank % 2);

			page.display(stream, request, pm);
		}



		void ForumInterfacePage::DisplayMessage(
			std::ostream& stream,
			const cms::Webpage& page,
			boost::shared_ptr<const cms::Webpage> dateDisplayPage,
			const server::Request& request,
			const ForumMessage& message,
			std::size_t rank
		){
			ParametersMap pm(request.getFunction()->getTemplateParameters());

			pm.insert(Request::PARAMETER_OBJECT_ID, message.getKey());
			if(message.getTopic())
			{
				pm.insert(DATA_TOPIC_ID, message.getTopic()->getKey());
				pm.insert(DATA_TOPIC_NAME, message.getTopic()->getName());
			}

			pm.insert(DATA_CONTENT, message.getContent());

			if(dateDisplayPage.get())
			{
				stringstream content;
				DateTimeInterfacePage::Display(content, dateDisplayPage, request, message.getDate());
				pm.insert(DATA_DATE, content.str());
			}
			else
			{
				pm.insert(DATA_DATE, lexical_cast<string>(message.getDate()));
			}

			if(message.getUser())
			{
				pm.insert(DATA_USER_ID, message.getUser()->getKey());
				pm.insert(DATA_USER_EMAIL, message.getUser()->getEMail());
				pm.insert(DATA_USER_NAME, message.getUser()->getName());
				pm.insert(DATA_USER_SURNAME, message.getUser()->getSurname());
			}
			else
			{
				pm.insert(DATA_USER_NAME, message.getUserName());
				pm.insert(DATA_USER_EMAIL, message.getUserEMail());
			}

			pm.insert(DATA_RANK, rank);
			pm.insert(DATA_RANK_IS_ODD, rank % 1);

			page.display(stream, request, pm);
		}
	}
}
