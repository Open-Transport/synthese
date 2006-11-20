
#include "12_security/Right.h"

namespace synthese
{
	namespace security
	{


		Right::Right( std::string parameter, Level privateLevel, Level publicLevel )
			: _parameter(parameter), _privateRightLevel(privateLevel), _publicRightLevel(publicLevel)
		{

		}
	}
}
