
#include "ActionException.h"

namespace synthese
{
	namespace server
	{

		ActionException::ActionException ( const std::string& message)
			: util::Exception(message)
		{


		}
		ActionException::~ActionException() throw ()
		{

		}
	}
}