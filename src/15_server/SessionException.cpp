
#include "SessionException.h"

namespace synthese
{
	namespace server
	{
		SessionException::SessionException(const std::string& message)
			: util::Exception(message)
		{

		}

		SessionException::~SessionException() throw ()
		{

		}
	}
}
