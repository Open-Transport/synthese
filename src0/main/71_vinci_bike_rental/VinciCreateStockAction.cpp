
/** VinciCreateStockAction class implementation.
	@file VinciCreateStockAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "VinciCreateStockAction.h"

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"
#include "30_server/QueryString.h"

#include "57_accounting/AccountTableSync.h"
#include "57_accounting/Account.h"

#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace accounts;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::VinciCreateStockAction>::FACTORY_KEY("create_stock");
	}

	namespace vinci
	{
		// const string VinciCreateStockAction::PARAMETER_ACCOUNT_ID = Action_PARAMETER_PREFIX + "ac";
		
		
		
		VinciCreateStockAction::VinciCreateStockAction()
			: util::FactorableTemplate<Action, VinciCreateStockAction>()
		{
		}
		
		
		
		ParametersMap VinciCreateStockAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void VinciCreateStockAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			try
			{
				_account = AccountTableSync::GetUpdateable(id);
			}
			catch (...)
			{
				throw ActionException("Account not found.");
			}

			if (_account->getStockAccountId() > 0)
				throw ActionException("Account already has a stock account");
		}
		
		
		
		void VinciCreateStockAction::run()
		{
			Account a;
			a.setName("Stock : "+ _account->getName());
			a.setRightClassNumber(VinciBikeRentalModule::VINCI_STOCK_CODE_PREFIX + _account->getRightClassNumber().substr(1));
			a.setRightUserId(_account->getRightUserId());
			a.setRightCurrency(VinciBikeRentalModule::getObjectCurrency().get());
			a.setLeftCurrency(VinciBikeRentalModule::getObjectCurrency().get());

			AccountTableSync::save(&a);

			_account->setStockAccountId(a.getKey());
			AccountTableSync::save(_account.get());
		}
	}
}
