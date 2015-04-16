
#include "UserException.h"

namespace synthese
{
	namespace security
	{

		UserException::UserException(const std::string& message)
			: Exception(message)
		{

		}

		UserException::~UserException() throw ()
		{

		}
	}
}
