
#include "12_security/User.h"

#include "71_vinci_bike_rental/VinciContract.h"

namespace synthese
{
	using namespace util;
	using namespace security;
	using namespace db;

	namespace vinci
	{
		VinciContract::VinciContract(uid id)
			: Registrable<uid, VinciContract>(id)
			, _user(NULL)
		{}

		void VinciContract::setUserId(uid id)
		{
			_userId = id;
		}

		uid VinciContract::getUserId() const
		{
			return _userId;
		}

		User* VinciContract::getUser() const
		{
			return _user;
		}

		VinciContract::~VinciContract()
		{
			delete _user;
		}
	}
}

