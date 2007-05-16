#include "01_util/Conversion.h"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;


BOOST_AUTO_TEST_CASE (testLongLongConversion)
{

     long long lli = 123456789012345LL;
     std::string lls = Conversion::ToString (lli);
      
     BOOST_CHECK_EQUAL (std::string ("123456789012345"), lls);
     BOOST_CHECK_EQUAL (lli, Conversion::ToLongLong ("123456789012345"));
}


