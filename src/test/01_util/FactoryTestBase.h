#ifndef SYNTHESE_UTIL_FactoryTestBase_h__
#define SYNTHESE_UTIL_FactoryTestBase_h__

#include "01_util/Factory.h"
#include "01_util/Factorable.h"

namespace synthese
{
	namespace util
	{

		class FactoryTestBase : public Factorable
		{
		public:
			virtual int getIndex() const = 0;
		};

	}
}
#endif // FactoryTestBase_h__
