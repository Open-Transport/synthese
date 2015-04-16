
/** Session class header.
	@file Session.h

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

#ifndef SYNTHESE_Session_H__
#define SYNTHESE_Session_H__

#include <string>
#include <map>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/thread/recursive_mutex.hpp>
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
		public:
			static const std::string ATTR_ID;
			static const std::string TAG_USER;
			static const std::string ATTR_IP;
			static const std::string ATTR_LAST_USE;

			typedef std::map<std::string, std::string> SessionVariables;
			typedef std::map<std::string, boost::shared_ptr<Session> > SessionMap;
			typedef std::set<const Request*> Requests;

		private:
			static SessionMap _sessionMap;
			static boost::mutex	_sessionMapMutex;

			const std::string			_key;
			const std::string			_ip;
			boost::shared_ptr<const security::User>	_user;
			boost::posix_time::ptime	_lastUse;
			SessionVariables			_sessionVariables;

			mutable boost::recursive_mutex		_mutex; //!< For thread safety in case of concurrent access with the same session.
			mutable boost::mutex	_requestsListMutex;
			Requests		_requests;


			static const size_t KEY_LENGTH;
			static const std::string COOKIE_SESSIONID;


			//!	\name Constructor and destructor
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Constructor.
				/// @param ip IP address of the user
				/// @param key forced key (empty = auto generated key)
				/// The constructor is private : use Session::New to initiate a session
				Session(
					const std::string& ip,
					std::string key = std::string()
				);
			//@}

				bool _removeSessionFromMap();

		public:
			//! \name Setters
			//@{
				void setUser(boost::shared_ptr<const security::User> user){ _user = user; }
			//@}

			//! \name Getters
			//@{
				const std::string getKey() const { return _key; }
				boost::shared_ptr<const security::User> getUser() const { return _user; }
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



				void removeSessionIdCookie(Request &request) const;



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



				void registerRequest(const Request& request);
				void unregisterRequest(const Request& request);



				//////////////////////////////////////////////////////////////////////////
				/// Session deletion helper.
				/// @param session the session to delete
				static void Delete(boost::shared_ptr<Session> session);



				//////////////////////////////////////////////////////////////////////////
				/// Session creation helper.
				/// @param ip IP address of the user
				/// @param request the request to register on the session
				/// @param key session key to force (empty = auto generated key)
				/// @return the newly created session
				static boost::shared_ptr<synthese::server::Session> New(
					const std::string& ip,
					std::string key = std::string()
				);

				
				
				//////////////////////////////////////////////////////////////////////////
				/// Session fetcher.
				/// @param key the key of the session
				/// @param ip IP address of the user
				/// @param request the request to register on the session
				/// @param exceptionIfNotFound if false, a NULL pointer is returned instead
				/// of an exception in case of a not found key.
				/// If found, the session is checked and updated :
				///  - check of the ip (must not have changed)
				///  - check of the expiration time of the session
				///  - refresh the last use time
				/// @throw SessionException if the session does not exist or if a check fails
				static boost::shared_ptr<synthese::server::Session> Get(
					const std::string& key,
					const std::string& ip,
					bool exceptionIfNotFound = true
				);



				//////////////////////////////////////////////////////////////////////////
				/// Sessions map getter.
				/// @return the current sessions
				static SessionMap& GetSessions(){ return _sessionMap; }



				//////////////////////////////////////////////////////////////////////////
				/// Export to a parameters map.
				/// @param pm the parameters map to populate
				/// @retval pm the parameters map to populate
				void toParametersMap(
					util::ParametersMap& pm
				) const;
			//@}
		};
	}
}

#endif // SYNTHESE_Session_H__
