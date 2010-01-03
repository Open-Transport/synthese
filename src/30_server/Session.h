
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

#include <boost/shared_ptr.hpp>

#include "DateTime.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace server
	{
		/** User session class.
			@ingroup m18
		*/
		class Session
		{
		private:
			const std::string			_key;
			const std::string			_ip;
			boost::shared_ptr<const security::User>	_user;
			synthese::time::DateTime	_lastUse;

			static const size_t KEY_LENGTH;
			static const int MAX_MINUTES_DURATION;
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
						- ip must not have change

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
		};
	}
}

#endif // SYNTHESE_Session_H__
