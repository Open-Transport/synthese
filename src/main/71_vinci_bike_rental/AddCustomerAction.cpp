
/** AddCustomerAction class implementation.
	@file AddCustomerAction.cpp

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

#include "12_security/User.h"
#include "12_security/UserTableSync.h"
#include "12_security/Profile.h"

#include "30_server/Request.h"
#include "30_server/ParametersMap.h"
#include "30_server/QueryString.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/AddCustomerAction.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<Action,vinci::AddCustomerAction>::FACTORY_KEY("vinciaddcustomer");
	}

	namespace vinci
	{
		const string AddCustomerAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";
		const string AddCustomerAction::PARAMETER_SURNAME = Action_PARAMETER_PREFIX + "surname";


		ParametersMap AddCustomerAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_SURNAME, _surname);
			return map;
		}

		void AddCustomerAction::_setFromParametersMap(const ParametersMap& map )
		{
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
			transform(_name.begin(), _name.end(), _name.begin(), toupper);
			_surname = map.getString(PARAMETER_SURNAME, true, FACTORY_KEY);
			transform(_surname.begin(), _surname.end(), _surname.begin(), toupper);
			_request->setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void AddCustomerAction::run()
		{
			shared_ptr<User> user(new User);
			shared_ptr<const Profile> profile = VinciBikeRentalModule::getCustomerProfile();
			
			user->setName(_name);
			user->setSurname(_surname);
			user->setProfile(profile.get());
			UserTableSync::save(user.get());

			shared_ptr<VinciContract> contract(new VinciContract);
			contract->setUserId(user->getKey());
			VinciContractTableSync::save(contract.get());

			_request->setObjectId(contract->getKey());
		}
	}
}
