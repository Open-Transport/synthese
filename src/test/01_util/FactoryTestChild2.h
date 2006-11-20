#ifndef SYNTHESE_UTIL_FactoryTestChild2_h__
#define SYNTHESE_UTIL_FactoryTestChild2_h__



#include "FactoryTestBase.h"

namespace synthese
{
	namespace util
	{

		class FactoryTestChild2 : public FactoryTestBase
		{

		public:
			int getIndex() const { return 2; }
		};

	}
}
#endif // FactoryTestChild2_h__