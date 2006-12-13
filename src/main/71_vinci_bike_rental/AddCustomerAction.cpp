
#include "12_security/User.h"
#include "12_security/UserTableSync.h"
#include "12_security/Profile.h"

#include "30_server/ServerModule.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/AddCustomerAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;

	namespace vinci
	{
		const string AddCustomerAction::PARAMETER_NAME = Action::PARAMETER_PREFIX + "name";
		const string AddCustomerAction::PARAMETER_SURNAME = Action::PARAMETER_PREFIX + "surname";


		Request::ParametersMap AddCustomerAction::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_NAME, _name));
			map.insert(make_pair(PARAMETER_SURNAME, _surname));
			return map;
		}

		void AddCustomerAction::setFromParametersMap( server::Request::ParametersMap& map )
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
		}

		void AddCustomerAction::run()
		{
			User* user = new User;
			Profile* profile = VinciBikeRentalModule::getCustomerProfile();
			
			user->setName(_name);
			user->setSurname(_surname);
			user->setProfile(profile);
			UserTableSync::save(ServerModule::getSQLiteThread(), user);

			VinciContract* contract = new VinciContract;
			contract->setUserId(user->getKey());
			VinciContractTableSync::save(ServerModule::getSQLiteThread(), contract);

			_request->setObjectId(contract->getKey());

			delete user;
			delete profile;
			delete contract;
		}
	}
}
