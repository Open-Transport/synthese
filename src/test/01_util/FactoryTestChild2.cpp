

#include "FactoryTestChild2.h"

namespace synthese
{
namespace util
{

	const std::string FactoryTestChild2::_factory_key = Factory<FactoryTestBase>::integrate<FactoryTestChild2>("Child2");
}
}
