#ifndef SYNTHESE_UTIL_FactoryTestBase_h__
#define SYNTHESE_UTIL_FactoryTestBase_h__

#include "01_util/Factory.h"

namespace synthese
{
	namespace util
	{

class FactoryTestBase
{
public:
	virtual int getIndex() const = 0;
};

	}
}
#endif // FactoryTestBase_h__