
#include "30_server/ClientException.h"

namespace synthese
{
	namespace server
	{
		ClientException::ClientException(const std::string& message)
			: util::Exception(message)
		{

		}

		ClientException::~ClientException() throw ()
		{

		}
	}
}
