
/** AccountAddAction class implementation.
	@file AccountAddAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "AccountAddAction.h"

#include "User.h"
#include "UserTableSync.h"

#include "ActionException.h"
#include "ParametersMap.h"

#include "Account.h"
#include "AccountTableSync.h"
#include "Currency.h"
#include "CurrencyTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<Action, accounts::AccountAddAction>::FACTORY_KEY("addaccount");
	}

	namespace accounts
	{
		const string AccountAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string AccountAddAction::PARAMETER_CURRENCY = Action_PARAMETER_PREFIX + "cu";
		const string AccountAddAction::PARAMETER_CLASS = Action_PARAMETER_PREFIX + "cl";
		const string AccountAddAction::PARAMETER_AUTO_INCREMENT_CLASS = Action_PARAMETER_PREFIX + "ai";
		const string AccountAddAction::PARAMETER_RIGHT_USER_ID = Action_PARAMETER_PREFIX + "ru";

		AccountAddAction::AccountAddAction()
		: util::FactorableTemplate<Action, AccountAddAction>()
		{
		}



		ParametersMap AccountAddAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_CLASS, _class);
			if (_currency.get())
				map.insert(PARAMETER_CURRENCY, _currency->getKey());
			if (_rightUser.get())
				map.insert(PARAMETER_RIGHT_USER_ID, _rightUser->getKey());
			map.insert(PARAMETER_AUTO_INCREMENT_CLASS, _autoIncrementClass);
			return map;
		}



		void AccountAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			_name = map.getDefault<string>(PARAMETER_NAME);
			if (_name.empty())
				throw ActionException("Name must be non empty.");
			// Search for existing account

			_class = map.getDefault<string>(PARAMETER_CLASS);
			if (_class.empty())
				throw ActionException("Class must be non empty");

			_autoIncrementClass = map.getDefautl<bool>(PARAMETER_AUTO_INCREMENT_CLASS, false);

			try
			{
				_currency = CurrencyTableSync::Get(map.get<RegistryKeyType>(PARAMETER_CURRENCY));
			}
			catch(...)
			{
				throw ActionException("Currency not found");
			}

			RegistryKeyType id = map.getDefault<RegistryKeyType(PARAMETER_RIGHT_USER_ID);
			if (id > 0)
			{
				try
				{
					_rightUser = UserTableSync::Get(id);
				}
				catch (...)
				{
					throw ActionException("User not found");
				}
			}
		}



		void AccountAddAction::run(Request& request)
		{
			Account a;
			a.setName(_name);
			a.setRightClassNumber(_autoIncrementClass ? AccountTableSync::GetNextCode(_class) : _class);
			a.setRightCurrency(_currency.get());
			a.setLeftCurrency(_currency.get());
			a.setRightUserId(_rightUser->getKey());

			AccountTableSync::Save(&a);
		}

		void AccountAddAction::setClass( const std::string& value )
		{
			_class = value;
		}

		void AccountAddAction::setCurrency( boost::shared_ptr<const Currency> value )
		{
			_currency = value;
		}

		void AccountAddAction::setAutoIncrementClass( bool value )
		{
			_autoIncrementClass = value;
		}

		void AccountAddAction::setRightUser( boost::shared_ptr<const security::User> value )
		{
			_rightUser = value;
		}
	}
}
