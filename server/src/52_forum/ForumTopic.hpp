
/** ForumTopic class header.
	@file ForumTopic.hpp

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

#ifndef SYNTHESE_forum_ForumTopic_hpp__
#define SYNTHESE_forum_ForumTopic_hpp__

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
		//////////////////////////////////////////////////////////////////////////
		/// Forum topic.
		///	@ingroup m52
		/// @author Hugues Romain
		/// @since 3.1.18
		/// @date 2010
		class ForumTopic:
			public util::Registrable
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<ForumTopic>	Registry;

		private:
			util::RegistryKeyType _node;
			std::string _nodeExtension;
			std::string _userEMail;
			std::string _userName;
			security::User* _user;
			boost::posix_time::ptime _date;
			bool _published;
			bool _closed;
			std::string _ip;
			std::string _name;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param id unique identifier of the object
			/// @author Hugues Romain
			/// @since 3.1.18
			/// @date 2010
			ForumTopic(util::RegistryKeyType id = 0);

			//! @name Getters
			//@{
				util::RegistryKeyType getNode() const { return _node; }
				const std::string& getNodeExtension() const { return _nodeExtension; }
				const std::string& getUserEMail() const { return _userEMail; }
				const std::string& getUserName() const { return _userName; }
				security::User* getUser() const { return _user; }
				boost::posix_time::ptime getDate() const { return _date; }
				bool getPublished() const { return _published; }
				bool getClosed() const { return _closed; }
				const std::string& getIP() const { return _ip; }
				virtual std::string getName() const { return _name; }
			//@}

			//! @name Setters
			//@{
				void setNode(util::RegistryKeyType value){ _node = value; }
				void setNodeExtension(const std::string& value){ _nodeExtension = value; }
				void setUserEMail(const std::string& value){ _userEMail = value; }
				void setUserName(const std::string& value){ _userName = value; }
				void setUser(security::User* value){ _user = value; }
				void setDate(boost::posix_time::ptime value){ _date = value; }
				void setPublished(bool value){ _published = value; }
				void setClosed(bool value){ _closed = value; }
				void setIP(const std::string& value){ _ip = value; }
				void setName(const std::string& value){ _name = value; }
			//@}

		};
	}
}

#endif // SYNTHESE_forum_ForumTopic_hpp__
