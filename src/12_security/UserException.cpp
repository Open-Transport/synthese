
#include "UserException.h"

namespace synthese
{
	namespace security
	{

		UserException::UserException(const std::string& message)
			: util::Exception(message)
		{

		}

		UserException::~UserException() throw ()
		{

		}
	}
}
