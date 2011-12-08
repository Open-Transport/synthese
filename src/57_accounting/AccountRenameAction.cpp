
/** AccountRenameAction class implementation.
	@file AccountRenameAction.cpp
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"

#include "Account.h"
#include "AccountTableSync.h"

#include "AccountRenameAction.h"

using namespace std;

namespace synthese
{
	using namespace server;

	namespace util
	{
		template<> const string FactorableTemplate<Action, accounts::AccountRenameAction>::FACTORY_KEY("account_rename");
	}

	namespace accounts
	{
		const string AccountRenameAction::PARAMETER_VALUE = Action_PARAMETER_PREFIX + "va";



		AccountRenameAction::AccountRenameAction()
			: util::FactorableTemplate<Action, AccountRenameAction>()
		{
		}



		ParametersMap AccountRenameAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(PARAMETER_xxx, _xxx);
			return map;
		}



		void AccountRenameAction::_setFromParametersMap(const ParametersMap& map)
		{
			_value = map.get<string>(PARAMETER_VALUE);

			RegistryKeyType id(request.getObjectId());
			try
			{
				_account = AccountTableSync::GetEditable(id);
			}
			catch (...)
			{
				throw ActionException("Account not found");
			}
		}



		void AccountRenameAction::run(Request& request)
		{
			_account->setName(_value);
			AccountTableSync::Save(_account.get());
		}
	}
}
