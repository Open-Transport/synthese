
#include "71_vinci_bike_rental/VinciContract.h"

namespace synthese
{
	using namespace util;

	namespace vinci
	{
		VinciContract::VinciContract(uid id)
			: Registrable<uid, VinciContract>(id)
		{}

		void VinciContract::setUserId(uid id)
		{
			_userId = id;
		}

		uid VinciContract::getUserId() const
		{
			return _userId;
		}
	}
}
