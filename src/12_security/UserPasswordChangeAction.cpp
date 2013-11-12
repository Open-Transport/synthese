
/** UserPasswordChangeAction class implementation.
	@file UserPasswordChangeAction.cpp
	@date 2013

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

#include "UserPasswordChangeAction.hpp"

#include "SecurityModule.h"
#include "ServerModule.h"
#include "ParametersMap.h"
#include "Request.h"
#include "ActionException.h"
#include "UserException.h"
#include "RequestException.h"
#include "User.h"
#include "UserTableSync.h"
#include "SecurityLog.h"
#include "EMail.h"
#include "Webpage.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace cms;
	using namespace server;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, security::UserPasswordChangeAction>::FACTORY_KEY("UserPasswordChange");

	namespace security
	{
		const std::string UserPasswordChangeAction::PARAMETER_LOGIN = Action_PARAMETER_PREFIX + "login";
		const std::string UserPasswordChangeAction::PARAMETER_OLD_PASSWORD = Action_PARAMETER_PREFIX + "old_password";
		const std::string UserPasswordChangeAction::PARAMETER_NEW_PASSWORD = Action_PARAMETER_PREFIX + "new_password";
		const std::string UserPasswordChangeAction::PARAMETER_CMS = Action_PARAMETER_PREFIX + "cms";

		const std::string UserPasswordChangeAction::DATA_SUBJECT_OR_CONTENT("subject_or_content");
		const std::string UserPasswordChangeAction::DATA_USER_LOGIN("user_login");
		const std::string UserPasswordChangeAction::DATA_USER_SURNAME("user_surname");
		const std::string UserPasswordChangeAction::DATA_USER_NAME("user_name");
		const std::string UserPasswordChangeAction::DATA_USER_KEY("user_key");
		const std::string UserPasswordChangeAction::DATA_USER_PHONE("user_phone");
		const std::string UserPasswordChangeAction::DATA_USER_EMAIL("user_email");
		const std::string UserPasswordChangeAction::DATA_USER_PASSWORD("user_password");
		
		const std::string UserPasswordChangeAction::TYPE_SUBJECT("subject");
		const std::string UserPasswordChangeAction::TYPE_CONTENT("content");

		ParametersMap UserPasswordChangeAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void UserPasswordChangeAction::_setFromParametersMap(const ParametersMap& map )
		{
			try
			{
				_login = map.get<string>(PARAMETER_LOGIN);
				_old_password = map.get<string>(PARAMETER_OLD_PASSWORD);
				_new_password = map.get<string>(PARAMETER_NEW_PASSWORD);
				setPasswordChangeEMailCMS(
					Env::GetOfficialEnv().get<cms::Webpage>(map.get<RegistryKeyType>(PARAMETER_CMS))
				);
			}
			catch(ParametersMap::MissingParameterException e)
			{
				throw ActionException(e, *this);
			}
		}

		void UserPasswordChangeAction::run(Request& request)
		{
			/* Fetch user using a mail or login parameter */
			boost::shared_ptr<User> user;

			try
			{
				if (!_login.empty() &&
					!_old_password.empty() &&
					!_new_password.empty() &&
					_new_password != _old_password
				){
					user = UserTableSync::getUserFromLogin(_login);
				}
				else
				{
					throw ActionException("All fields must be filled");
				}

				if (!user)
				{
					throw ActionException("User not found");
				}	
				else
				{
					user->verifyPassword(_old_password);
				}
			}
			catch (ActionException& e)
			{
				throw Exception("Action load error : " + e.getMessage());	
			}
			catch (UserException& e)
			{
				throw Exception("Action load error : " + e.getMessage());
			}

			/* Change password and send a mail using a CMS id */
			try
			{
				/* senderEMail must be filled in loading parameters */
				std::string senderEMail(SecurityModule::getSenderEMail());
				if (senderEMail.empty())
					throw ActionException("There is no mail address for sender");

				/* Disallow deactivated users and users without profile */
				if (user->getConnectionAllowed() && user->getProfile())
				{
					/* Change password for current user */
					user->setPassword(_new_password);

					/* Before saving new password, check mail is sent */
					if (!sendPasswordChangeEMail(*user,senderEMail))
					{
						user->resetTempPassword();
						throw ActionException("Mail not sent");
					}
					else 
					{
						user->resetTempPassword();
						UserTableSync::Save(user.get());
						
						/* Add log entry in security log */
						SecurityLog::addUserAdmin(
							user.get(),
							user.get(),
							"Personnalisation de mot de passe"
						);
					}
				}
				else
				{
					throw ActionException("Connection impossible");
				}
			}
			catch (ActionException& e)
			{
				throw Exception("Action load error : " + e.getMessage());	
			}
		}



		void UserPasswordChangeAction::setLogin(const std::string& login)
		{
			_login = login;
		}



		void UserPasswordChangeAction::setPasswordChangeEMailCMS(boost::shared_ptr<const cms::Webpage> cms)
		{
			_cmsPasswordChangeEMail = cms;
		}



		bool UserPasswordChangeAction::sendPasswordChangeEMail(const User& user, const std::string sender) const 
		{
			/* Need a CMS in order to send a mail */
			if(!_cmsPasswordChangeEMail.get())
			{
				return false;
			}

			/* Build a mail and send it */
			try
			{
				EMail email(ServerModule::GetEMailSender());

				/* MIME type */
				string mimeType = _cmsPasswordChangeEMail.get()->getMimeType();
				if (mimeType == "text/html") {
					email.setFormat(EMail::EMAIL_HTML);
				}
				else
				{
					email.setFormat(EMail::EMAIL_TEXT);
				}

				/* Build header */
				email.setSender(sender);
				email.setSenderName(sender);

				stringstream subject;
				ParametersMap subjectMap;

				subjectMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_SUBJECT);

				_cmsPasswordChangeEMail.get()->display(subject, subjectMap);

				email.setSubject(subject.str());
				email.addRecipient(user.getEMail(), user.getName());

				/* Build content */
				stringstream content;
				ParametersMap contentMap;

				_cmsPasswordChangeEMail.get()->display(content, contentMap);
				email.setContent(content.str());

				/* Send mail */
				email.send();

				return true;
			}
			catch(boost::system::system_error)
			{
				return false;
			}
		}



		bool UserPasswordChangeAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
	}
}
