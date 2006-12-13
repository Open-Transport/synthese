#include "ConversionTest.h"

#include "01_util/Conversion.h"



namespace synthese
{
namespace util
{


  void 
  ConversionTest::setUp () 
  {

  }


  void 
  ConversionTest::tearDown() 
  {

  } 


  void
  ConversionTest::testLongLongConversion ()
  {
      long long lli = 123456789012345LL;
      std::string lls = Conversion::ToString (lli);
      
      CPPUNIT_ASSERT_EQUAL (std::string ("123456789012345"), lls);
      CPPUNIT_ASSERT_EQUAL (lli, Conversion::ToLongLong ("123456789012345"));

  }







}
}


