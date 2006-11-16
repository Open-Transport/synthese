

#include "FactoryTestChild1.h"

namespace synthese
{
	namespace util
	{

		const std::string FactoryTestChild1::_factory_key = Factory<FactoryTestBase>::integrate<FactoryTestChild1>("Child1");

	}
}
