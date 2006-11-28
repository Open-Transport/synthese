
#ifndef SYNTHESE_VinciContract_H__
#define SYNTHESE_VinciContract_H__

#include "01_util/Registrable.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace vinci
	{
		class VinciContract : public util::Registrable<uid, VinciContract>
		{
			security::User*	_user;
		};
	}
}

#endif // SYNTHESE_VinciContract_H__
