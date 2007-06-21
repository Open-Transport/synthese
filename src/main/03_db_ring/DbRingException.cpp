#include "DbRingException.h"

namespace synthese
{
	namespace dbring
	{
		DbRingException::DbRingException ( const std::string& message)
			: synthese::util::Exception (message)
		{
		}

		DbRingException::~DbRingException () throw ()
		{
		}
	}
}
