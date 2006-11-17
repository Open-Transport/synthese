
#include "InterfaceModule.h"

namespace synthese
{
	namespace server
	{
		const std::string InterfaceModule::_factory_key = Factory<ModuleClass>::integrate<InterfaceModule>("11_interfaces");

		void InterfaceModule::initialize( const Server* server )
		{

		}
	}
}
