#include "FactoryTest.h"

#include "FactoryTestBase.h"
#include "01_util/FactoryException.h"

#include <iostream>


namespace synthese
{
	namespace util
	{

		void FactoryTest::setUp () 
		{

		}


		void FactoryTest::tearDown() 
		{

		} 


		void FactoryTest::testFactory ()
		{
			CPPUNIT_ASSERT ( Factory<FactoryTestBase>::contains("Child1") );
			CPPUNIT_ASSERT ( Factory<FactoryTestBase>::contains("Child2") );
			CPPUNIT_ASSERT ( !Factory<FactoryTestBase>::contains("Child3") );

			FactoryTestBase* object1 = Factory<FactoryTestBase>::create("Child1");
			FactoryTestBase* object2 = Factory<FactoryTestBase>::create("Child2");

			// The child3 registration is "forgotten"
			FactoryTestBase* notfound = NULL;
			try
			{
				notfound = Factory<FactoryTestBase>::create("Child3");
				CPPUNIT_ASSERT(0);
			}
			catch (FactoryException<FactoryTestBase> e)
			{
				CPPUNIT_ASSERT(1);
			}

			CPPUNIT_ASSERT (object1 != NULL);
			CPPUNIT_ASSERT_EQUAL (object1->getIndex(), 1);
//			CPPUNIT_ASSERT (Factory<FactoryTestBase>::getKey<FactoryTestChild1>() == "Child1");
			CPPUNIT_ASSERT (object2 != NULL);
			CPPUNIT_ASSERT_EQUAL (object2->getIndex(), 2);
//			CPPUNIT_ASSERT (Factory<FactoryTestBase>::getKey<FactoryTestChild2>() == "Child2");
			CPPUNIT_ASSERT (notfound == NULL);
//			CPPUNIT_ASSERT (Factory<FactoryTestBase>::getKey<FactoryTestChild3>() == "");
			
			Factory<FactoryTestBase>::Iterator it = Factory<FactoryTestBase>::begin();
			
			CPPUNIT_ASSERT( it != Factory<FactoryTestBase>::end() );
		//	CPPUNIT_ASSERT( dynamic_cast<FactoryTestChild1*>(*it) != NULL );
			CPPUNIT_ASSERT( it.getKey() == "Child1" );
			CPPUNIT_ASSERT_EQUAL( it->getIndex(), 1 );
			FactoryTestBase* obj1 = *it;
			FactoryTestBase* obj2 = *it;
			CPPUNIT_ASSERT_EQUAL( obj1, obj2 );

			++it;

			CPPUNIT_ASSERT( it != Factory<FactoryTestBase>::end() );
			CPPUNIT_ASSERT( *it != obj1 );
		//	CPPUNIT_ASSERT( dynamic_cast<FactoryTestChild2*>(*it) != NULL );
			CPPUNIT_ASSERT( it.getKey() == "Child2" );
			CPPUNIT_ASSERT_EQUAL( it->getIndex(), 2 );
			FactoryTestBase* obj3 = *it;
			FactoryTestBase* obj4 = *it;
			CPPUNIT_ASSERT_EQUAL( obj3, obj4 );
			obj3 = it.getObject();

			++it;
			CPPUNIT_ASSERT( it == Factory<FactoryTestBase>::end() );

			CPPUNIT_ASSERT_EQUAL( obj3->getIndex(), 2 );
			delete obj3;

		}







	}
}

