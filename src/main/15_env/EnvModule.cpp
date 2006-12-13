
#include "15_env/EnvModule.h"

namespace synthese
{
	namespace env
	{
		Environment::Registry EnvModule::_environments;

		void EnvModule::initialize()
		{
		}


		Environment::Registry& 
			EnvModule::getEnvironments ()
		{
			return _environments;
		}
	}
}
