#ifndef SYNTHESE_UTIL_FACTORYTEST_H
#define SYNTHESE_UTIL_FACTORYTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
	namespace util
	{


		class FactoryTest : public CPPUNIT_NS::TestCase
		{
			CPPUNIT_TEST_SUITE(FactoryTest);
			CPPUNIT_TEST(testFactory);
			CPPUNIT_TEST_SUITE_END();

		public:

			void setUp(void);
			void tearDown(void);

		protected:

			void testFactory ();

		};




	}
}


#endif 
