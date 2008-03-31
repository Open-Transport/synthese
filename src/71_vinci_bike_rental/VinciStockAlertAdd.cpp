
/** VinciStockAlertAdd class implementation.
	@file VinciStockAlertAdd.cpp
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

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"

#include "VinciStockAlertAdd.h"

#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"
#include "71_vinci_bike_rental/VinciStockAlert.h"
#include "71_vinci_bike_rental/VinciStockAlertTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace accounts;
	using namespace server;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::VinciStockAlertAdd>::FACTORY_KEY("VinciStockAlertAdd");
	}

	namespace vinci
	{
		const string VinciStockAlertAdd::PARAMETER_PRODUCT = Action_PARAMETER_PREFIX + "pr";
		const string VinciStockAlertAdd::PARAMETER_SITE = Action_PARAMETER_PREFIX + "si";
		const string VinciStockAlertAdd::PARAMETER_MIN = Action_PARAMETER_PREFIX + "mi";
		const string VinciStockAlertAdd::PARAMETER_MAX = Action_PARAMETER_PREFIX + "ma";

		
		
		VinciStockAlertAdd::VinciStockAlertAdd()
			: util::FactorableTemplate<Action, VinciStockAlertAdd>()
			, _min(UNKNOWN_VALUE), _max(UNKNOWN_VALUE)
		{
		}
		
		
		
		ParametersMap VinciStockAlertAdd::getParametersMap() const
		{
			ParametersMap map;
			if (_site.get())
				map.insert(PARAMETER_SITE, _site->getKey());
			if (_product.get())
				map.insert(PARAMETER_PRODUCT, _product->getKey());
			return map;
		}
		
		
		
		void VinciStockAlertAdd::_setFromParametersMap(const ParametersMap& map)
		{
			uid id = map.getUid(PARAMETER_SITE, true, FACTORY_KEY);
			try
			{
				_site = VinciSiteTableSync::Get(id);
			}
			catch (...)
			{
				throw ActionException("No such site");
			}

			id = map.getUid(PARAMETER_PRODUCT, true, FACTORY_KEY);
			try
			{
				_product = AccountTableSync::Get(id);
			}
			catch (...)
			{
				throw ActionException("No such product");
			}

			_min = map.getDouble(PARAMETER_MIN, true, FACTORY_KEY);
			_max = map.getDouble(PARAMETER_MAX, true, FACTORY_KEY);

			if (_min < 0)
				throw ActionException("Le minimum doit être un nombre positif");
			if (_max <= _min && _max != 0)
				throw ActionException("Le maximum doit être supérieur au minimum");
		}
		
		
		
		void VinciStockAlertAdd::run()
		{
			VinciStockAlert a;
			a.setSiteId(_site->getKey());
			a.setAccountId(_product->getKey());
			a.setMinAlert(_min);
			a.setMaxAlert(_max);
			VinciStockAlertTableSync::save(&a);
		}

		void VinciStockAlertAdd::setSite( boost::shared_ptr<const VinciSite> site )
		{
			_site = site;
		}

		void VinciStockAlertAdd::setProduct( boost::shared_ptr<const accounts::Account> product )
		{
			_product = product;
		}
	}
}
