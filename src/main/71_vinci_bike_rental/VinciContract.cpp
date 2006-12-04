
#include "71_vinci_bike_rental/VinciContract.h"

namespace synthese
{
	namespace vinci
	{
		VinciContract::VinciContract(uid id=0)
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
