
/** VinciStockFullfillAction class implementation.
	@file VinciStockFullfillAction.cpp
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

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"

#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"

#include "VinciStockFullfillAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace accounts;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::VinciStockFullfillAction>::FACTORY_KEY("vinci_stock_fullfill");
	}

	namespace vinci
	{
		const string VinciStockFullfillAction::PARAMETER_ACCOUNT_ID = Action_PARAMETER_PREFIX + "ac";
		const string VinciStockFullfillAction::PARAMETER_SITE_ID = Action_PARAMETER_PREFIX + "si";
		const string VinciStockFullfillAction::PARAMETER_PIECES = Action_PARAMETER_PREFIX + "pi";
		const string VinciStockFullfillAction::PARAMETER_COMMENT(Action_PARAMETER_PREFIX + "co");
		
		
		VinciStockFullfillAction::VinciStockFullfillAction()
			: util::FactorableTemplate<Action, VinciStockFullfillAction>()
		{
		}
		
		
		
		ParametersMap VinciStockFullfillAction::getParametersMap() const
		{
			ParametersMap map;
			if (_site.get())
				map.insert(PARAMETER_SITE_ID, _site->getKey());
			if (_account.get())
				map.insert(PARAMETER_ACCOUNT_ID, _account->getKey());
			return map;
		}
		
		
		
		void VinciStockFullfillAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(PARAMETER_ACCOUNT_ID, true, FACTORY_KEY));
			try
			{
				_account = AccountTableSync::Get(id);
			}
			catch (...)
			{
				throw ActionException("Bad value for account parameter ");	
			}

			id = map.getUid(PARAMETER_SITE_ID, true, FACTORY_KEY);
			try
			{
				_site = VinciSiteTableSync::Get(id);
			}
			catch (...)
			{
				throw ActionException("Bad value for site parameter ");	
			}

			_pieces = map.getDouble(PARAMETER_PIECES, true, FACTORY_KEY);
			_comment = map.getString(PARAMETER_COMMENT, false, FACTORY_KEY);
		}
		
		
		
		void VinciStockFullfillAction::run()
		{
			DateTime now(TIME_CURRENT);

			Transaction ft;
			ft.setStartDateTime(now);
			ft.setEndDateTime(now);
			string name(string((_pieces > 0) ? "Approvisionnement" : "Sortie") + " de stock");
			ft.setName(name);
			ft.setComment(_comment);
			TransactionTableSync::save(&ft);

			// Stock fullfill
			TransactionPart	ftp3;
			ftp3.setTransactionId(ft.getKey());
			ftp3.setAccountId(_account->getKey());
			ftp3.setAmount(_pieces);
			ftp3.setStockId(_site->getKey());
			TransactionPartTableSync::save(&ftp3);

			// Source stock
			uid id((_site->getParentSiteId() > 0) ? _account->getKey() : VinciBikeRentalModule::getStockChargeAccount()->getKey());
			TransactionPart ftp2;
			ftp2.setTransactionId(ft.getKey());
			ftp2.setAccountId(id);
			ftp2.setAmount(-_pieces);
			ftp2.setStockId(_site->getParentSiteId());
			TransactionPartTableSync::save(&ftp3);

		}

		void VinciStockFullfillAction::setSite( boost::shared_ptr<const VinciSite> site )
		{
			_site = site;
		}

		void VinciStockFullfillAction::setAccount( boost::shared_ptr<const accounts::Account> account )
		{
			_account = account;
		}
	}
}
