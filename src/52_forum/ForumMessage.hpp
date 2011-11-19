
/** ForumMessage class header.
	@file ForumMessage.hpp

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

#ifndef SYNTHESE_forum_ForumMessage_hpp__
#define SYNTHESE_forum_ForumMessage_hpp__

#include "Registrable.h"
#include "Registry.h"

#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace forum
	{
		class ForumTopic;

		/** ForumMessage class.
			@ingroup m52
		*/
		class ForumMessage:
			public util::Registrable
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<ForumMessage>	Registry;

		private:
			ForumTopic* _topic;
			std::string _content;
			std::string _userEMail;
			std::string _userName;
			security::User* _user;
			boost::posix_time::ptime _date;
			bool _published;
			std::string _ip;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param id unique identifier of the object
			/// @author Hugues Romain
			/// @since 3.1.18
			/// @date 2010
			ForumMessage(util::RegistryKeyType id = 0);

			//! @name Getters
			//@{
				ForumTopic* getTopic() const { return _topic; }
				const std::string& getContent() const { return _content; }
				const std::string& getUserEMail() const { return _userEMail; }
				const std::string& getUserName() const { return _userName; }
				security::User* getUser() const { return _user; }
				boost::posix_time::ptime getDate() const { return _date; }
				bool getPublished() const { return _published; }
				const std::string& getIP() const { return _ip; }
			//@}

			//! @name Setters
			//@{
				void setTopic(ForumTopic* value){ _topic = value; }
				void setContent(const std::string& value){ _content = value; }
				void setUserEMail(const std::string& value){ _userEMail = value; }
				void setUserName(const std::string& value){ _userName = value; }
				void setUser(security::User* value){ _user = value; }
				void setDate(boost::posix_time::ptime value){ _date = value; }
				void setPublished(bool value){ _published = value; }
				void setIP(const std::string& value){ _ip = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_forum_ForumMessage_hpp__
