
#include "30_server/ServerModuleException.h"

namespace synthese
{
	namespace server
	{
		ServerModuleException::ServerModuleException(const std::string& message)
			: util::Exception(message)
		{

		}

		ServerModuleException::~ServerModuleException() throw ()
		{

		}
	}
}