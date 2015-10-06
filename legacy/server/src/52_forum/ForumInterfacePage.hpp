
/** ForumInterfacePage class header.
	@file ForumInterfacePage.hpp

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

#ifndef SYNTHESE_forum_ForumInterfacePage_hpp__
#define SYNTHESE_forum_ForumInterfacePage_hpp__

#include <string>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace server
	{
		class Request;
	}

	namespace forum
	{
		class ForumTopic;
		class ForumMessage;

		//////////////////////////////////////////////////////////////////////////
		/// 52.11 Interface : forum module elements display.
		///	@ingroup m52
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.18
		class ForumInterfacePage
		{
		public:
			static const std::string DATA_TOPIC_ID;
			static const std::string DATA_TOPIC_NAME;
			static const std::string DATA_NAME;
			static const std::string DATA_USER_NAME;
			static const std::string DATA_USER_SURNAME;
			static const std::string DATA_USER_EMAIL;
			static const std::string DATA_USER_ID;
			static const std::string DATA_DATE;
			static const std::string DATA_MESSAGES_NUMBER;
			static const std::string DATA_LAST_MESSAGE_DATE;
			static const std::string DATA_CLOSED;
			static const std::string DATA_CONTENT;
			static const std::string DATA_RANK;
			static const std::string DATA_RANK_IS_ODD;


			//////////////////////////////////////////////////////////////////////////
			/// Display of a forum topic.
			/// @param stream stream to display on
			/// @param page template to use for the display
			/// @param dateDisplayPage template to use for the display of dates
			/// @param request current request
			/// @param topic topic to display
			/// @param rank rank of the topic in the list
			//////////////////////////////////////////////////////////////////////////
			/// <h3>Parameters sent to the display template</h3>
			///	<ul>
			///		<li>roid : id of the topic</li>
			///		<li>name : name of the topic</li>
			///		<li>user_name : name of the user who has created the topic</li>
			///		<li>user_surname : surname of the user who has created the topic</li>
			///		<li>user_email : e-mail of the user who has created the topic</li>
			///		<li>user_id : id of the user who has created the topic</li>
			///		<li>date : date of topic creation</li>
			///		<li>messages_number : number of messages in the topic</li>
			///		<li>last_message_date : last message date</li>
			///		<li>closed : closed topic</li>
			///		<li>rank : rank of the topic in the list</li>
			///		<li>rank_is_odd : rank oddness the topic in the list</li>
			///	</ul>
			static void DisplayTopic(
				std::ostream& stream,
				const cms::Webpage& page,
				boost::shared_ptr<const cms::Webpage> dateDisplayPage,
				const server::Request& request,
				const ForumTopic& topic,
				std::size_t rank
			);



			//////////////////////////////////////////////////////////////////////////
			/// Display of a forum message.
			/// @param stream stream to display on
			/// @param page template to use for the display
			/// @param dateDisplayPage template to use for the display of dates
			/// @param request current request
			/// @param message message to display
			/// @param rank rank of the topic in the list
			//////////////////////////////////////////////////////////////////////////
			/// <h3>Parameters sent to the display template</h3>
			///	<ul>
			///		<li>roid : id of the message</li>
			///		<li>topic_id : id of the topic</li>
			///		<li>topic_name : name of the topic</li>
			///		<li>user_name : name of the user who has created the message</li>
			///		<li>user_surname : surname of the user who has created the message</li>
			///		<li>user_email : e-mail of the user who has created the message</li>
			///		<li>user_id : id of the user who has created the message</li>
			///		<li>date : date of message publication</li>
			///		<li>content : content</li>
			///		<li>rank : rank of the message in the list</li>
			///		<li>rank_is_odd : rank oddness the message in the list</li>
			///	</ul>
			static void DisplayMessage(
				std::ostream& stream,
				const cms::Webpage& page,
				boost::shared_ptr<const cms::Webpage> dateDisplayPage,
				const server::Request& request,
				const ForumMessage& message,
				std::size_t rank
			);
		};
	}
}

#endif // SYNTHESE_forum_ForumInterfacePage_hpp__
