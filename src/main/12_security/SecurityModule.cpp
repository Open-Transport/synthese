
#include "SecurityModule.h"

namespace synthese
{
	namespace security
	{
		Profile::Registry SecurityModule::_profiles;

		void SecurityModule::initialize()
		{

		}

		Profile::Registry& SecurityModule::getProfiles()
		{
			return _profiles;
		}
	}
}