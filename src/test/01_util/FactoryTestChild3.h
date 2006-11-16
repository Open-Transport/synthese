#ifndef SYNTHESE_UTIL_FactoryTestChild3_h__
#define SYNTHESE_UTIL_FactoryTestChild3_h__



#include "FactoryTestBase.h"

namespace synthese
{
	namespace util
	{

		class FactoryTestChild3 : public FactoryTestBase
		{
			static bool _registered;
		public:
			int getIndex() const { return 3; }
		};

	}
}
#endif // FactoryTestChild3_h__