
/** Session class header.
	@file Session.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_Session_H__
#define SYNTHESE_Session_H__

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/thread/mutex.hpp>

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace server
	{
		class Request;

		/** User session class.
			@ingroup m15
		*/
		class Session
		{
		private:
			typedef std::map<std::string, std::string> SessionVariables;

			const std::string			_key;
			const std::string			_ip;
			boost::shared_ptr<const security::User>	_user;
			boost::posix_time::ptime	_lastUse;
			SessionVariables			_sessionVariables;
			mutable boost::mutex		_mutex; //!< For thread safety in case of concurrent access with the same session.

			static const size_t KEY_LENGTH;
			static const std::string COOKIE_SESSIONID;
			static std::string generateKey();


		public:
			//!	\name Constructor and destructor
			//@{
				Session(const std::string& ip);

				/** Unregisters the session in the global session map.
				*/
				~Session();
			//@}

			//! \name Modifiers
			//@{
				/** Refresh the last use date after several controls.

					The controls are :
						- expiration of the session
						- ip must not have changed

					If a control fails, then a SessionException is thrown
				*/
				void controlAndRefresh(const std::string& ip);
			//@}

			//! \name Setters
			//@{
				void setUser(boost::shared_ptr<const security::User> user);
			//@}

			//! \name Getters
			//@{
				const std::string getKey() const;
				boost::shared_ptr<const security::User> getUser() const;
			//@}

			//! @name Queries
			//@{
				bool hasProfile() const;
			//@}

			//! @name Services
			//@{
				////////////////////////////////////////////////////////////////////
				/// Adds a cookie to the given request with this session identifier.
				/// This method doesn't check if the session is still valid, it has
				/// to be taken care of first.
				/// @param request Request where the session id cookie should be set.
				/// @author Sylvain Pasche
				/// @date 2011
				/// @since 3.3.0
				void setSessionIdCookie(Request &request) const;



				//////////////////////////////////////////////////////////////////////////
				/// Sets a session variable content.
				/// If the content is empty, the variable is removed.
				//////////////////////////////////////////////////////////////////////////
				/// @param variable variable identifier
				/// @param value value to set
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				void setSessionVariable(const std::string& variable, const std::string& value);



				//////////////////////////////////////////////////////////////////////////
				/// Gets a session variable content.
				/// @param variable variable identifier
				/// @return the content of the specified variable if it exists, empty string else
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				std::string getSessionVariable(const std::string& variable) const;
			//@}
		};
	}
}

#endif // SYNTHESE_Session_H__
