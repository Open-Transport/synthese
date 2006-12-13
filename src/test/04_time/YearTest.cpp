#include "YearTest.h"

#include "04_time/Year.h"

#include <iostream>


namespace synthese
{
namespace time
{



  void 
  YearTest::setUp () 
  {

  }


  void 
  YearTest::tearDown() 
  {

  } 


  void
  YearTest::testLeapYears ()
  {
    {
      Year y (2000); // divisible by 400
      CPPUNIT_ASSERT (y.isLeapYear () == true);
      CPPUNIT_ASSERT_EQUAL (366, y.getDaysCount ());
    }
    {
      Year y (1000); // not divisible by 400 but divisible by 100
      CPPUNIT_ASSERT (y.isLeapYear () == false);
      CPPUNIT_ASSERT_EQUAL (365, y.getDaysCount ());
    }
    {
      Year y (2004); // not divisible by 400 nor 100, but divisible by 4
      CPPUNIT_ASSERT (y.isLeapYear () == true);
      CPPUNIT_ASSERT_EQUAL (366, y.getDaysCount ());
    }
    {
      Year y (2006); // not divisible by 400 nor 100, but divisible by 4
      CPPUNIT_ASSERT (y.isLeapYear () == false);
      CPPUNIT_ASSERT_EQUAL (365, y.getDaysCount ());
    }


  }


}
}


