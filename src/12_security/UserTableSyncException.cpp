
#include "UserTableSyncException.h"

namespace synthese
{
	namespace security
	{

		UserTableSyncException::UserTableSyncException(const std::string& message)
			: util::Exception(message)
		{

		}

		UserTableSyncException::~UserTableSyncException() throw ()
		{

		}
	}
}
