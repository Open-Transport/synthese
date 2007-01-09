
#include "01_util/Conversion.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "30_server/ServerModule.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciUpdateCustomerAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;
	
	namespace vinci
	{
		const string VinciUpdateCustomerAction::PARAMETER_NAME = Action::PARAMETER_PREFIX + "na";
		const string VinciUpdateCustomerAction::PARAMETER_SURNAME = Action::PARAMETER_PREFIX + "sn";
		const string VinciUpdateCustomerAction::PARAMETER_ADDRESS = Action::PARAMETER_PREFIX + "ad";
		const string VinciUpdateCustomerAction::PARAMETER_POST_CODE = Action::PARAMETER_PREFIX + "pc";
		const string VinciUpdateCustomerAction::PARAMETER_CITY = Action::PARAMETER_PREFIX + "ci";
		const string VinciUpdateCustomerAction::PARAMETER_COUNTRY = Action::PARAMETER_PREFIX + "co";
		const string VinciUpdateCustomerAction::PARAMETER_EMAIL = Action::PARAMETER_PREFIX + "em";
		const string VinciUpdateCustomerAction::PARAMETER_PHONE = Action::PARAMETER_PREFIX + "ph";
		const string VinciUpdateCustomerAction::PARAMETER_ID = Action::PARAMETER_PREFIX + "id";


		Request::ParametersMap VinciUpdateCustomerAction::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_NAME, _name));
			map.insert(make_pair(PARAMETER_SURNAME, _surname));
			map.insert(make_pair(PARAMETER_ADDRESS, _address));
			map.insert(make_pair(PARAMETER_POST_CODE, _postCode));
			map.insert(make_pair(PARAMETER_CITY, _city));
			map.insert(make_pair(PARAMETER_COUNTRY, _country));
			map.insert(make_pair(PARAMETER_EMAIL, _email));
			map.insert(make_pair(PARAMETER_PHONE, _phone));
			map.insert(make_pair(PARAMETER_ID, Conversion::ToString(_id)));
			return map;
		}

		void VinciUpdateCustomerAction::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;

			it = map.find(PARAMETER_NAME);
			if (it != map.end())
			{
				_name = it->second;
				map.erase(it);
			}

			it = map.find(PARAMETER_SURNAME);
			if (it != map.end())
			{
				_surname = it->second;
				map.erase(it);
			}

			it = map.find(PARAMETER_ADDRESS);
			if (it != map.end())
			{
				_address = it->second;
				map.erase(it);
			}

			it = map.find(PARAMETER_POST_CODE);
			if (it != map.end())
			{
				_postCode = it->second;
				map.erase(it);
			}

			it = map.find(PARAMETER_CITY);
			if (it != map.end())
			{
				_city = it->second;
				map.erase(it);
			}

			it = map.find(PARAMETER_COUNTRY);
			if (it != map.end())
			{
				_country = it->second;
				map.erase(it);
			}

			it = map.find(PARAMETER_EMAIL);
			if (it != map.end())
			{
				_email = it->second;
				map.erase(it);
			}

			it = map.find(PARAMETER_PHONE);
			if (it != map.end())
			{
				_phone = it->second;
				map.erase(it);
			}

			it = map.find(PARAMETER_ID);
			if (it != map.end())
			{
				_id = Conversion::ToLongLong(it->second);
				map.erase(it);
			}

		}

		void VinciUpdateCustomerAction::run()
		{
			VinciContract* contract = VinciContractTableSync::get(_id);
			User* user = UserTableSync::get(contract->getUserId());
			user->setName(_name);
			user->setSurname(_surname);
			user->setAddress(_address);
			user->setPostCode(_postCode);
			user->setCityText(_city);
			user->setCountry(_country);
			user->setEMail(_email);
			user->setPhone(_phone);
			UserTableSync::save(user);
		}
	}
}
