#ifndef SYNTHESE_UTIL_FactoryTestChild1_h__
#define SYNTHESE_UTIL_FactoryTestChild1_h__



#include "FactoryTestBase.h"

namespace synthese
{
	namespace util
	{

		class FactoryTestChild1 : public FactoryTestBase
		{
			static const std::string _factory_key;
			
		public:
			int getIndex() const { return 1; }
		};

	}
}
#endif // FactoryTestChild1_h__