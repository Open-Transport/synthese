#include "04_time/Year.h"
#include <iostream>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::time;


BOOST_AUTO_TEST_CASE (testLeapYears)
{
    {
	Year y (2000); // divisible by 400
	BOOST_CHECK (y.isLeapYear () == true);
	BOOST_CHECK_EQUAL (366, y.getDaysCount ());
    }
    {
	Year y (1000); // not divisible by 400 but divisible by 100
	BOOST_CHECK (y.isLeapYear () == false);
	BOOST_CHECK_EQUAL (365, y.getDaysCount ());
    }
    {
	Year y (2004); // not divisible by 400 nor 100, but divisible by 4
	BOOST_CHECK (y.isLeapYear () == true);
	BOOST_CHECK_EQUAL (366, y.getDaysCount ());
    }
    {
	Year y (2006); // not divisible by 400 nor 100, but divisible by 4
	BOOST_CHECK (y.isLeapYear () == false);
	BOOST_CHECK_EQUAL (365, y.getDaysCount ());
    }
    
    
 }



