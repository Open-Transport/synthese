#include "AlgorithmModule.h"

#include "AlgorithmModule.inc.cpp"

namespace synthese
{
	namespace algorithm
	{
		void moduleRegister()
		{
			AlgorithmModule::integrate();
		}
	}
}
