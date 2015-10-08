
/** ReservationUserMergeAction class implementation.
	@file ReservationUserMergeAction.cpp
	@date 2014

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

#include "ReservationUserMergeAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "ResaModule.h"
#include "Request.h"
#include "Session.h"
#include "UserTableSync.h"
#include "User.h"
#include "RemoveObjectAction.hpp"
#include "DBLogModule.h"
#include "ResaRight.h"
#include "SecurityLog.h"
#include "ResaDBLog.h"
#include "DBLogEntryTableSync.h"
#include "ReservationTransactionTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace dblog;
    using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::ReservationUserMergeAction>::FACTORY_KEY("ReservationUserMergeAction");
	}

	namespace resa
	{
		const string ReservationUserMergeAction::PARAMETER_USER_ID(Action_PARAMETER_PREFIX + "u");
		const string ReservationUserMergeAction::PARAMETER_LOGIN = Action_PARAMETER_PREFIX + "login";
		const string ReservationUserMergeAction::PARAMETER_SURNAME = Action_PARAMETER_PREFIX + "surn";
		const string ReservationUserMergeAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";
		const string ReservationUserMergeAction::PARAMETER_ADDRESS = Action_PARAMETER_PREFIX + "addr";
		const string ReservationUserMergeAction::PARAMETER_POSTAL_CODE(Action_PARAMETER_PREFIX + "post");
		const string ReservationUserMergeAction::PARAMETER_CITY = Action_PARAMETER_PREFIX + "city";
		const string ReservationUserMergeAction::PARAMETER_PHONE = Action_PARAMETER_PREFIX + "phon";
		const string ReservationUserMergeAction::PARAMETER_EMAIL = Action_PARAMETER_PREFIX + "email";
		const string ReservationUserMergeAction::PARAMETER_AUTHORIZED_LOGIN(Action_PARAMETER_PREFIX + "al");
		const string ReservationUserMergeAction::PARAMETER_AUTORESA_ACTIVATED = Action_PARAMETER_PREFIX + "aa";
        const string ReservationUserMergeAction::PARAMETER_USER_TO_DELETE(Action_PARAMETER_PREFIX + "utd");

		
		ParametersMap ReservationUserMergeAction::getParametersMap() const
        {
            ParametersMap map;

            if (_userToDelete.get())
            {
                map.insert(PARAMETER_USER_TO_DELETE, _userToDelete->getKey());
            }

            return map;
		}

		
		
		void ReservationUserMergeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
                if (map.getDefault<RegistryKeyType>(PARAMETER_USER_TO_DELETE,0) != 0)
                {
                    _userToDelete = UserTableSync::GetEditable(
                        map.get<RegistryKeyType>(PARAMETER_USER_TO_DELETE),
                        *_env
                    );
                }

                if (map.getDefault<RegistryKeyType>(PARAMETER_USER_ID,0) != 0)
                {
                    _userToMerge = UserTableSync::GetEditable(
                        map.get<RegistryKeyType>(PARAMETER_USER_ID),
                        *_env
                    );
                }

				_login = map.getDefault<string>(PARAMETER_LOGIN);
				_surname = map.getDefault<string>(PARAMETER_SURNAME);
				_name = map.getDefault<string>(PARAMETER_NAME);

				if (_name.empty())
					throw ActionException("Le nom de l'utilisateur ne peut être vide");

				_address = map.getDefault<string>(PARAMETER_ADDRESS);
				_postalCode = map.getDefault<string>(PARAMETER_POSTAL_CODE);
				_phone = map.getDefault<string>(PARAMETER_PHONE);
				_city = map.getDefault<string>(PARAMETER_CITY);
				_email = map.getDefault<string>(PARAMETER_EMAIL);
				_authorizedLogin = map.getOptional<bool>(PARAMETER_AUTHORIZED_LOGIN);
				_autoResaActivated = map.getOptional<bool>(PARAMETER_AUTORESA_ACTIVATED);

				if(	_autoResaActivated && *_autoResaActivated)
				{
					if(_surname.empty()) throw ActionException("Le prénom du client doit être rempli pour activer l'auto-réservation");
					if(_name.empty()) throw ActionException("Le nom du client doit être rempli pour activer l'auto-réservation");
					if(_phone.empty()) throw ActionException("Le numéro de téléphone du client doit être rempli pour activer l'auto-réservation");
					if(_email.empty()) throw ActionException("L'adresse e-mail du client doit être remplie pour activer l'auto-réservation");
				}
			}
			catch (ObjectNotFoundException<User>&)
			{
				throw ActionException("Utilisateur introuvable");
			}
		}



		void ReservationUserMergeAction::run(Request& request)
		{
            bool forceUpdate = false;

            // Check updates
            if ((_userToMerge->getName() != _name && !_name.empty()) ||
                (_userToMerge->getSurname() != _surname && !_surname.empty()) ||
                (_userToMerge->getEMail() != _email && !_email.empty())
            ){
                forceUpdate = true;
            }

			// Merge user values to userToMerge
			_userToMerge->setLogin(_login);
			_userToMerge->setAddress(_address);
			_userToMerge->setEMail(_email);
			_userToMerge->setPostCode(_postalCode);
			_userToMerge->setCityText(_city);
			_userToMerge->setPhone(_phone);
			_userToMerge->setName(_name);
			_userToMerge->setSurname(_surname);
			if(_authorizedLogin)
			{
				_userToMerge->setConnectionAllowed(*_authorizedLogin);
			}
			if (_autoResaActivated &&
				(_userToMerge->getProfile()->getKey() == ResaModule::GetBasicResaCustomerProfile()->getKey() ||
				 _userToMerge->getProfile()->getKey() == ResaModule::GetAutoResaResaCustomerProfile()->getKey())
			) {
				_userToMerge->setProfile(
					(*_autoResaActivated ? ResaModule::GetAutoResaResaCustomerProfile() : ResaModule::GetBasicResaCustomerProfile()).get()
				);
			}

            if (forceUpdate)
            {
                // Update reservation transactions values for userToMerge with new ones
                ReservationTransactionTableSync::SearchResult transactions(ReservationTransactionTableSync::SearchByUser(
                    *_env,
                    _userToMerge->getKey(),
                    boost::posix_time::ptime(boost::posix_time::not_a_date_time),
                    boost::posix_time::ptime(boost::posix_time::not_a_date_time),
                    true
                ));
                BOOST_FOREACH(const ReservationTransactionTableSync::SearchResult::value_type& transaction, transactions)
                {
                    transaction->set<CustomerName>(_userToMerge->getSurname()+" "+_userToMerge->getName());
					transaction->set<CustomerEmail>(_userToMerge->getEMail());
                    ReservationTransactionTableSync::Save(transaction.get());
                }
            }

			// Merge related logs
			DBLogEntryTableSync::SearchResult logs(DBLogEntryTableSync::SearchByUser(*_env, _userToDelete->getKey(), UP_LINKS_LOAD_LEVEL));
			BOOST_FOREACH(const DBLogEntryTableSync::SearchResult::value_type& log, logs)
            {
				ResaDBLog::UpdateCallEntryCustomer(log->getKey(), _userToMerge->getKey());
            }

			// Merge related ReservationTransaction
			ReservationTransactionTableSync::SearchResult transactions(ReservationTransactionTableSync::SearchByUser(
				*_env,
				_userToDelete->getKey(),
				boost::posix_time::ptime(boost::posix_time::not_a_date_time),
				boost::posix_time::ptime(boost::posix_time::not_a_date_time),
				true
			));
			BOOST_FOREACH(const ReservationTransactionTableSync::SearchResult::value_type& transaction, transactions)
			{
				// Attach reseervation transaction to merged user
				transaction->set<Customer>(*_userToMerge);

				if (!_userToMerge->getName().empty() && !_userToMerge->getSurname().empty())
				{
					transaction->set<CustomerName>(_userToMerge->getSurname()+" "+_userToMerge->getName());
				}
				if (!_userToMerge->getEMail().empty())
				{
					transaction->set<CustomerEmail>(_userToMerge->getEMail());
				}
				if (transaction->get<BookingUserId>() == _userToDelete->getKey())
				{
					transaction->set<BookingUserId>(_userToMerge->getKey());
				}
				if (transaction->get<CancelUserId>() == _userToDelete->getKey())
				{
					transaction->set<CancelUserId>(_userToDelete->getKey());
				}

				ReservationTransactionTableSync::Save(transaction.get());
			}

			// Save user which must be merged
			UserTableSync::Save(_userToMerge.get());

			// Remove user which must be deleted
            RemoveObjectAction deleteUserAction;
            deleteUserAction.setObjectId(_userToDelete->getKey());
            deleteUserAction.run(request);

			// Log user merging in SecurityLog
			SecurityLog::addUserAdmin(
				request.getSession()->getUser().get(),
				_userToMerge.get(),
				"Fusion de l'utilisateur " + _userToMerge->getLogin() + " avec " + _userToDelete->getLogin()
			);
		}



		bool ReservationUserMergeAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(WRITE);
		}



		void ReservationUserMergeAction::setUserToDelete(boost::shared_ptr<const security::User> user)
		{
			_userToDelete = const_pointer_cast<User>(user);
		}
	}
}
