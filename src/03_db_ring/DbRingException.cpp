#include "DbRingException.h"

namespace synthese
{
	namespace dbring
	{
		DbRingException::DbRingException ( const std::string& message)
			: synthese::Exception (message)
		{
		}

		DbRingException::~DbRingException () throw ()
		{
		}
	}
}
