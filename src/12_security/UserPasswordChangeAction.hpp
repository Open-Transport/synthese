
/** UserPasswordChangeAction class header.
	@file UserPasswordChangeAction.hpp
	@date 2013

	This file belongs to the SYNTHESE project (public transportation specialized
	software)
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
	along with this program; if not, write to the Free Software Foundation,
	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_UserPasswordChangeAction_H__
#define SYNTHESE_UserPasswordChangeAction_H__

#include <boost/shared_ptr.hpp>

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace security
	{
		class User;

		/** Send a new auto-generated password to a customer
		    @ingroup m12Actions refActions
		 */
		class UserPasswordChangeAction:
			public util::FactorableTemplate<server::Action, UserPasswordChangeAction>
		{
		public:
			static const std::string PARAMETER_LOGIN;
			static const std::string PARAMETER_OLD_PASSWORD;
			static const std::string PARAMETER_NEW_PASSWORD;
			static const std::string PARAMETER_CMS;

			static const std::string DATA_SUBJECT_OR_CONTENT;
			static const std::string DATA_USER_LOGIN;
			static const std::string DATA_USER_SURNAME;
			static const std::string DATA_USER_NAME;
			static const std::string DATA_USER_KEY;
			static const std::string DATA_USER_PHONE;
			static const std::string DATA_USER_EMAIL;
			static const std::string DATA_USER_PASSWORD;

			static const std::string TYPE_SUBJECT;
			static const std::string TYPE_CONTENT;

		private:
			std::string _login;
			std::string _old_password;
			std::string _new_password;
			boost::shared_ptr<const cms::Webpage> _cmsPasswordChangeEMail;

		protected:
			/** Conversion from attributes to generic parameter maps. */
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Setters */
			void setLogin(const std::string& login);
			void setPasswordChangeEMailCMS(boost::shared_ptr<const cms::Webpage> cms);
			
			/** Action to run, defined by each subclass. */
			void run(server::Request& request);
			
			/** Sends e-mail to the customer containing his new generated password.
			 *	@param user User
			 *	@return true if the e-mail is sent
			 *	@date 2013
			 */
			bool sendPasswordChangeEMail(const security::User& user, const std::string sender) const;

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_UserPasswordChangeAction_H__
