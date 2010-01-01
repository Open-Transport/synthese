
/** ReservationUserUpdateAction class implementation.
	@file ReservationUserUpdateAction.cpp
	@author Hugues
	@date 2009

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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ReservationUserUpdateAction.h"
#include "ResaModule.h"
#include "Request.h"
#include "UserTableSync.h"
#include "User.h"
#include "DBLogModule.h"
#include "ResaRight.h"
#include "SecurityLog.h"
#include "ResaDBLog.h"
#include "OnlineReservationRule.h"

#include <boost/algorithm/string/replace.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace dblog;

	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::ReservationUserUpdateAction>::FACTORY_KEY("ReservationUserUpdateAction");
	}

	namespace resa
	{
		const string ReservationUserUpdateAction::PARAMETER_USER_ID(Action_PARAMETER_PREFIX + "u");
		const string ReservationUserUpdateAction::PARAMETER_LOGIN = Action_PARAMETER_PREFIX + "login";
		const string ReservationUserUpdateAction::PARAMETER_SURNAME = Action_PARAMETER_PREFIX + "surn";
		const string ReservationUserUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";
		const string ReservationUserUpdateAction::PARAMETER_ADDRESS = Action_PARAMETER_PREFIX + "addr";
		const string ReservationUserUpdateAction::PARAMETER_POSTAL_CODE(Action_PARAMETER_PREFIX + "post");
		const string ReservationUserUpdateAction::PARAMETER_CITY = Action_PARAMETER_PREFIX + "city";
		const string ReservationUserUpdateAction::PARAMETER_PHONE = Action_PARAMETER_PREFIX + "phon";
		const string ReservationUserUpdateAction::PARAMETER_EMAIL = Action_PARAMETER_PREFIX + "email";
		const string ReservationUserUpdateAction::PARAMETER_AUTHORIZED_LOGIN(Action_PARAMETER_PREFIX + "al");
		const string ReservationUserUpdateAction::PARAMETER_AUTORESA_ACTIVATED = Action_PARAMETER_PREFIX + "aa";
		
		
		
		ParametersMap ReservationUserUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_user.get()) map.insert(PARAMETER_USER_ID, _user->getKey());
			return map;
		}
		
		
		
		void ReservationUserUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_user = UserTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_USER_ID),
					*_env
					);

				_login = map.getDefault<string>(PARAMETER_LOGIN);

				if(!_login.empty())
				{
					UserTableSync::SearchResult users(
						UserTableSync::Search(
							*_env,
							_login,
							optional<string>(),
							optional<string>(),
							optional<string>(),
							optional<RegistryKeyType>(),
							logic::indeterminate,
							logic::indeterminate,
							_user->getKey(),
							0, 1, false, false, false, false, FIELDS_ONLY_LOAD_LEVEL
					)	);
					if(!users.empty())
					{
						throw ActionException("Ce login est d�j� utilis�.");
					}
				}

				_surname = map.getDefault<string>(PARAMETER_SURNAME);

				_name = map.getDefault<string>(PARAMETER_NAME);
				if (_name.empty())
					throw ActionException("Le nom de l'utilisateur ne peut �tre vide");

				_address = map.getDefault<string>(PARAMETER_ADDRESS);

				_postalCode = map.getDefault<string>(PARAMETER_POSTAL_CODE);

				_phone = map.getDefault<string>(PARAMETER_PHONE);

				_city = map.getDefault<string>(PARAMETER_CITY);

				_email = map.getDefault<string>(PARAMETER_EMAIL);

				_authorizedLogin = map.getOptional<bool>(PARAMETER_AUTHORIZED_LOGIN);

				_autoResaActivated = map.getOptional<bool>(PARAMETER_AUTORESA_ACTIVATED);

				if(	_autoResaActivated && *_autoResaActivated)
				{
					if(_surname.empty()) throw ActionException("Le pr�nom du client doit �tre rempli pour activer l'auto-r�servation");
					if(_name.empty()) throw ActionException("Le nom du client doit �tre rempli pour activer l'auto-r�servation");
					if(_phone.empty()) throw ActionException("Le num�ro de t�l�phone du client doit �tre rempli pour activer l'auto-r�servation");
					if(_email.empty()) throw ActionException("L'adresse e-mail du client doit �tre remplie pour activer l'auto-r�servation");
				}
			}
			catch (ObjectNotFoundException<User>& e)
			{
				throw ActionException("Utilisateur introuvable");
			}
		}
		
		
		
		void ReservationUserUpdateAction::run(Request& request)
		{
			stringstream s;
			DBLogModule::appendToLogIfChange(s, "Login", _user->getLogin(), _login);
			DBLogModule::appendToLogIfChange(s, "Adresse", _user->getAddress(), _address);
			DBLogModule::appendToLogIfChange(s, "E-mail", _user->getEMail(), _email);
			DBLogModule::appendToLogIfChange(s, "Code postal", _user->getPostCode(), _postalCode);
			DBLogModule::appendToLogIfChange(s, "Ville", _user->getCityText(), _city);
			DBLogModule::appendToLogIfChange(s, "T�l�phone", _user->getPhone(), _phone);
			if(_authorizedLogin)
			{
				DBLogModule::appendToLogIfChange(s, "Autorisation de connexion", lexical_cast<string>(_user->getConnectionAllowed()), lexical_cast<string>(*_authorizedLogin));
			}
			DBLogModule::appendToLogIfChange(s, "Nom", _user->getName(), _name);
			DBLogModule::appendToLogIfChange(s, "Pr�nom", _user->getSurname(), _surname);

			_user->setLogin(_login);
			_user->setAddress(_address);
			_user->setEMail(_email);
			_user->setPostCode(_postalCode);
			_user->setCityText(_city);
			_user->setPhone(_phone);
			if(_authorizedLogin)
			{
				_user->setConnectionAllowed(*_authorizedLogin);
			}
			_user->setName(_name);
			_user->setSurname(_surname);

			if(!s.str().empty())
			{
				SecurityLog::addUserAdmin(request.getUser().get(), _user.get(), s.str());
				ResaDBLog::AddUserAdminEntry(*request.getSession(), *_user, s.str());
			}


			if(	_autoResaActivated &&
				(	_user->getProfile()->getKey() == ResaModule::GetBasicResaCustomerProfile()->getKey() ||
					_user->getProfile()->getKey() == ResaModule::GetAutoResaResaCustomerProfile()->getKey()
			)	){
				stringstream s2;
				DBLogModule::appendToLogIfChange(
					s2,
					"Activation auto-r�servation",
					_user->getProfile()->getKey() == ResaModule::GetAutoResaResaCustomerProfile()->getKey() ? "OUI" : "NON",
					*_autoResaActivated ? "OUI" : "NON"
				);
				_user->setProfile(
					(*_autoResaActivated ? ResaModule::GetAutoResaResaCustomerProfile() : ResaModule::GetBasicResaCustomerProfile()).get()
				);

				// Change detection
				if(!s2.str().empty())
				{
					ResaDBLog::AddUserChangeAutoResaActivationEntry(*request.getSession(), *_user);

					// Send confirmation e-mail to the customer
					if(_autoResaActivated)
					{
						// Create a login if empty
						if(_user->getLogin().empty())
						{
							string idealLogin(_user->getSurname().substr(0,1) + _user->getName());
							algorithm::replace_all(idealLogin, " ", string());
							algorithm::replace_all(idealLogin, "'", string());
							algorithm::to_lower(idealLogin);

							UserTableSync::SearchResult logins(UserTableSync::Search(*_env, idealLogin));

							if(logins.empty())
							{
								_user->setLogin(idealLogin);
							}
							else
							{
								for(int i=0; i<999999; ++i)
								{
									UserTableSync::SearchResult logins(UserTableSync::Search(*_env, idealLogin + lexical_cast<string>(i)));

									if(logins.empty())
									{
										_user->setLogin(idealLogin + lexical_cast<string>(i));
										break;
									}
								}
							}
						}

						// Create random password if empty
						if(_user->getPassword().empty())
						{
							_user->setRandomPassword();
						}

						// Send confirmation email
						if(ResaModule::GetReservationContact())
						{
							if(ResaModule::GetReservationContact()->sendCustomerEMail(*_user))
							{
								ResaDBLog::AddEMailEntry(*request.getSession(), *_user, "Message d'activation de r�servation en ligne");
							}
						}
					}
				}
			}

			UserTableSync::Save(_user.get());
		}
		
		
		
		bool ReservationUserUpdateAction::isAuthorized(const Profile& profile
		) const {
			return profile.isAuthorized<ResaRight>(WRITE);
		}



		void ReservationUserUpdateAction::setUser( boost::shared_ptr<const security::User> value )
		{
			_user = const_pointer_cast<User>(value);
		}
	}
}
