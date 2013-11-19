
/** UserPasswordRecoveryAction class implementation.
	@file UserPasswordRecoveryAction.cpp
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

#include "UserPasswordRecoveryAction.hpp"

#include "SecurityModule.h"
#include "ServerModule.h"
#include "ParametersMap.h"
#include "Request.h"
#include "ActionException.h"
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

	template<> const string util::FactorableTemplate<Action, security::UserPasswordRecoveryAction>::FACTORY_KEY("UserPasswordRecovery");

	namespace security
	{
		const std::string UserPasswordRecoveryAction::PARAMETER_LOGIN = Action_PARAMETER_PREFIX + "login";
		const std::string UserPasswordRecoveryAction::PARAMETER_MAIL = Action_PARAMETER_PREFIX + "mail";
		const std::string UserPasswordRecoveryAction::PARAMETER_CMS = Action_PARAMETER_PREFIX + "cms";

		const std::string UserPasswordRecoveryAction::DATA_SUBJECT_OR_CONTENT("subject_or_content");
		const std::string UserPasswordRecoveryAction::DATA_USER_LOGIN("user_login");
		const std::string UserPasswordRecoveryAction::DATA_USER_SURNAME("user_surname");
		const std::string UserPasswordRecoveryAction::DATA_USER_NAME("user_name");
		const std::string UserPasswordRecoveryAction::DATA_USER_KEY("user_key");
		const std::string UserPasswordRecoveryAction::DATA_USER_PHONE("user_phone");
		const std::string UserPasswordRecoveryAction::DATA_USER_EMAIL("user_email");
		const std::string UserPasswordRecoveryAction::DATA_USER_PASSWORD("user_password");
		
		const std::string UserPasswordRecoveryAction::TYPE_SUBJECT("subject");
		const std::string UserPasswordRecoveryAction::TYPE_CONTENT("content");

		ParametersMap UserPasswordRecoveryAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void UserPasswordRecoveryAction::_setFromParametersMap(const ParametersMap& map )
		{
			try
			{
				_login = map.get<string>(PARAMETER_LOGIN);
				_mail = map.get<string>(PARAMETER_MAIL);
				setPasswordRecoveryEMailCMS(
					Env::GetOfficialEnv().get<cms::Webpage>(map.get<RegistryKeyType>(PARAMETER_CMS))
				);
			}
			catch(ParametersMap::MissingParameterException e)
			{
				throw ActionException(e, *this);
			}
		}

		void UserPasswordRecoveryAction::run(Request& request)
		{
			/* Fetch user using a mail or login parameter */
			boost::shared_ptr<User> user;

			try
			{
				if (!_login.empty())
				{
					user = UserTableSync::getUserFromLogin(_login);
				}
				else if (!_mail.empty())
				{
					user = UserTableSync::getUserFromMail(_mail);
				}
				else
				{
					throw ActionException("At least one field must be filled");
				}

				if (!user)
					throw ActionException("User not found");
			}
			catch (ActionException& e)
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
					/* Generate new password for current user */
					user->setRandomPassword();
					
					/* Before saving new password, check mail is sent */
					if (!sendPasswordRecoveryEMail(*user,senderEMail))
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
							"Demande de nouveau mot de passe"
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



		void UserPasswordRecoveryAction::setLogin(const std::string& login)
		{
			_login = login;
		}



		void UserPasswordRecoveryAction::setMail(const std::string& mail)
		{
			_mail = mail;
		}



		void UserPasswordRecoveryAction::setPasswordRecoveryEMailCMS(boost::shared_ptr<const cms::Webpage> cms)
		{
			_cmsPasswordRecoveryEMail = cms;
		}



		bool UserPasswordRecoveryAction::sendPasswordRecoveryEMail(const User& user, const std::string sender) const 
		{
			/* Need a CMS in order to send a mail */
			if(!_cmsPasswordRecoveryEMail.get())
			{
				return false;
			}

			/* Build a mail and send it */
			try
			{
				EMail email(ServerModule::GetEMailSender());

				/* MIME type */
				string mimeType = _cmsPasswordRecoveryEMail.get()->getMimeType();
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

				_cmsPasswordRecoveryEMail.get()->display(subject, subjectMap);

				email.setSubject(subject.str());
				email.addRecipient(user.getEMail(), user.getName());

				/* Build content */
				stringstream content;
				ParametersMap contentMap;

				contentMap.insert(DATA_SUBJECT_OR_CONTENT, TYPE_CONTENT);
				contentMap.insert(DATA_USER_LOGIN, user.getLogin());
				contentMap.insert(DATA_USER_PASSWORD, user.getPassword());
				contentMap.insert(DATA_USER_EMAIL, user.getEMail());
				contentMap.insert(DATA_USER_PHONE, user.getPhone());
				contentMap.insert(DATA_USER_NAME, user.getName());
				contentMap.insert(DATA_USER_SURNAME, user.getSurname());

				_cmsPasswordRecoveryEMail.get()->display(content, contentMap);
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



		bool UserPasswordRecoveryAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
	}
}
