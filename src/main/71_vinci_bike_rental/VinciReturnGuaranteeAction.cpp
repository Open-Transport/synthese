
/** VinciReturnGuaranteeAction class implementation.
	@file VinciReturnGuaranteeAction.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#include "01_util/Conversion.h"

#include "04_time/DateTime.h"

#include "12_security/User.h"

#include "30_server/ActionException.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"

#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciReturnGuaranteeAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace accounts;
	using namespace time;
	using namespace db;
	
	namespace vinci
	{
		const string VinciReturnGuaranteeAction::PARAMETER_GUARANTEE_ID = Action_PARAMETER_PREFIX + "gu";


		ParametersMap VinciReturnGuaranteeAction::getParametersMap() const
		{
			ParametersMap map;
			if (_guarantee.get())
				map.insert(make_pair(PARAMETER_GUARANTEE_ID, Conversion::ToString(_guarantee->getKey())));
			return map;
		}

		void VinciReturnGuaranteeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_GUARANTEE_ID);
				if (it == map.end())
					throw ActionException("Guarantee not specified");
				_guarantee = TransactionTableSync::get(Conversion::ToLongLong(it->second));
			}
			catch(Transaction::ObjectNotFoundException& e)
			{
				throw ActionException(e.getMessage());
			}
		}

		void VinciReturnGuaranteeAction::run()
		{
			DateTime now;
			_guarantee->setEndDateTime(now);
			TransactionTableSync::save(_guarantee.get());
		}
	}
}
