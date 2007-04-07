
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

#include "02_db/DBEmptyResultException.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "30_server/ServerModule.h"
#include "30_server/Request.h"
#include "30_server/ActionException.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciUpdateCustomerAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace time;
	using namespace db;
	
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

		ParametersMap VinciUpdateCustomerAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_NAME, _name));
			map.insert(make_pair(PARAMETER_SURNAME, _surname));
			map.insert(make_pair(PARAMETER_ADDRESS, _address));
			map.insert(make_pair(PARAMETER_POST_CODE, _postCode));
			map.insert(make_pair(PARAMETER_CITY, _city));
			map.insert(make_pair(PARAMETER_COUNTRY, _country));
			map.insert(make_pair(PARAMETER_EMAIL, _email));
			map.insert(make_pair(PARAMETER_PHONE, _phone));
			map.insert(make_pair(PARAMETER_BIRTH_DATE, _birthDate.toString()));
			return map;
		}

		void VinciUpdateCustomerAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_contract = VinciContractTableSync::get(_request->getObjectId());
				_user = UserTableSync::get(_contract->getUserId());

				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_NAME);
				if (it != map.end())
				{
					_name = it->second;
				}
				if (_name.empty())
					throw ActionException("Le nom ne peut être vide");

				it = map.find(PARAMETER_SURNAME);
				if (it != map.end())
				{
					_surname = it->second;
				}

				it = map.find(PARAMETER_ADDRESS);
				if (it != map.end())
				{
					_address = it->second;
				}

				it = map.find(PARAMETER_POST_CODE);
				if (it != map.end())
				{
					_postCode = it->second;
				}

				it = map.find(PARAMETER_CITY);
				if (it != map.end())
				{
					_city = it->second;
				}

				it = map.find(PARAMETER_COUNTRY);
				if (it != map.end())
				{
					_country = it->second;
				}

				it = map.find(PARAMETER_EMAIL);
				if (it != map.end())
				{
					_email = it->second;
				}

				it = map.find(PARAMETER_PHONE);
				if (it != map.end())
				{
					_phone = it->second;
				}

				it = map.find(PARAMETER_BIRTH_DATE);
				if (it != map.end())
				{
					_birthDate = Date::FromString(it->second);
				}

				it = map.find(PARAMETER_PASSPORT);
				if (it != map.end())
				{
					_passport = it->second;
				}

			}
			catch (DBEmptyResultException e)
			{
				throw ActionException("Contract not found");
			}
			catch (User::RegistryKeyException e)
			{
				throw ActionException("Contract without corresponding user");
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
			UserTableSync::save(_user);
			_contract->setPassport(_passport);
			VinciContractTableSync::save(_contract);
		}
	}
}
