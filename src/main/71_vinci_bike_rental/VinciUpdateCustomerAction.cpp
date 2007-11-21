
/** VinciUpdateCustomerAction class implementation.
	@file VinciUpdateCustomerAction.cpp

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

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "30_server/ServerModule.h"
#include "30_server/Request.h"
#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciUpdateCustomerAction.h"

#include <boost/algorithm/string.hpp>


using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace time;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<server::Action, vinci::VinciUpdateCustomerAction>::FACTORY_KEY("vinciupdatecustomer");
	}
	
	namespace vinci
	{
		const string VinciUpdateCustomerAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string VinciUpdateCustomerAction::PARAMETER_SURNAME = Action_PARAMETER_PREFIX + "sn";
		const string VinciUpdateCustomerAction::PARAMETER_ADDRESS = Action_PARAMETER_PREFIX + "ad";
		const string VinciUpdateCustomerAction::PARAMETER_POST_CODE = Action_PARAMETER_PREFIX + "pc";
		const string VinciUpdateCustomerAction::PARAMETER_CITY = Action_PARAMETER_PREFIX + "ci";
		const string VinciUpdateCustomerAction::PARAMETER_COUNTRY = Action_PARAMETER_PREFIX + "co";
		const string VinciUpdateCustomerAction::PARAMETER_EMAIL = Action_PARAMETER_PREFIX + "em";
		const string VinciUpdateCustomerAction::PARAMETER_PHONE = Action_PARAMETER_PREFIX + "ph";
		const string VinciUpdateCustomerAction::PARAMETER_BIRTH_DATE = Action_PARAMETER_PREFIX + "bd";
		const string VinciUpdateCustomerAction::PARAMETER_PASSPORT = Action_PARAMETER_PREFIX + "pp";

		VinciUpdateCustomerAction::VinciUpdateCustomerAction()
			: FactorableTemplate<Action, VinciUpdateCustomerAction>()
			, _birthDate(TIME_UNKNOWN)
		{

		}

		ParametersMap VinciUpdateCustomerAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_SURNAME, _surname);
			map.insert(PARAMETER_ADDRESS, _address);
			map.insert(PARAMETER_POST_CODE, _postCode);
			map.insert(PARAMETER_CITY, _city);
			map.insert(PARAMETER_COUNTRY, _country);
			map.insert(PARAMETER_EMAIL, _email);
			map.insert(PARAMETER_PHONE, _phone);
			map.insert(PARAMETER_BIRTH_DATE, _birthDate);
			return map;
		}

		void VinciUpdateCustomerAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_contract = VinciContractTableSync::GetUpdateable(_request->getObjectId());
				_user = _contract->getUser();

				_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
				if (_name.empty())
					throw ActionException("Le nom ne peut être vide");
				boost::algorithm::to_upper (_name);

				_surname = map.getString(PARAMETER_SURNAME, false, FACTORY_KEY);
				boost::algorithm::to_upper (_surname);

				_address = map.getString(PARAMETER_ADDRESS, false, FACTORY_KEY);
				_postCode = map.getString(PARAMETER_POST_CODE, false, FACTORY_KEY);
				_city = map.getString(PARAMETER_CITY, false, FACTORY_KEY);
				transform(_city.begin(), _city.end(), _city.begin(), toupper);

				_country = map.getString(PARAMETER_COUNTRY, false, FACTORY_KEY);
				_email = map.getString(PARAMETER_EMAIL, false, FACTORY_KEY);
				_phone = map.getString(PARAMETER_PHONE, false, FACTORY_KEY);
				_birthDate = map.getDate(PARAMETER_BIRTH_DATE, false, FACTORY_KEY);
				_passport = map.getString(PARAMETER_PASSPORT, false, FACTORY_KEY);
			}
			catch (VinciContract::ObjectNotFoundException& e)
			{
				throw ActionException(e.getMessage());
			}
			catch (User::ObjectNotFoundException& e)
			{
				throw ActionException("Contract without corresponding user " + e.getMessage());
			}

		}

		void VinciUpdateCustomerAction::run()
		{
			_user->setName(_name);
			_user->setSurname(_surname);
			_user->setAddress(_address);
			_user->setPostCode(_postCode);
			_user->setCityText(_city);
			_user->setCountry(_country);
			_user->setEMail(_email);
			_user->setPhone(_phone);
			_user->setBirthDate(_birthDate);
			UserTableSync::save(_user.get());
			_contract->setPassport(_passport);
			VinciContractTableSync::save(_contract.get());
		}
	}
}
