#define BOOST_AUTO_TEST_MAIN


#include <boost/test/auto_unit_test.hpp>
// #include <boost/test/included/unit_test_framework.hpp>


int add( int i, int j ) { return i+j; }



BOOST_AUTO_TEST_SUITE( suite_name );


BOOST_AUTO_TEST_CASE( my_test )
{
    // seven ways to detect and report the same error:
    BOOST_CHECK( add( 2,2 ) == 4 );        // #1 continues on error

    BOOST_REQUIRE( add( 2,2 ) == 4 );      // #2 throws on error

    if( add( 2,2 ) != 4 )
      BOOST_ERROR( "Ouch..." );            // #3 continues on error

    if( add( 2,2 ) != 4 )
      BOOST_FAIL( "Ouch..." );             // #4 throws on error

    if( add( 2,2 ) != 4 ) throw "Ouch..."; // #5 throws on error

    BOOST_CHECK_MESSAGE( add( 2,2 ) == 4,  // #6 continues on error
                         "add(..) result: " << add( 2,2 ) );

    BOOST_CHECK_EQUAL( add( 2,2 ), 4 );	  // #7 continues on error
}


BOOST_AUTO_TEST_CASE( my_test2 )
{
    BOOST_CHECK( false );        // #1 continues on error

}


BOOST_AUTO_TEST_SUITE_END ();


/*


#include "15_env/Axis.h"
#include "15_env/Calendar.h"
#include "15_env/Line.h"

#include "04_time/Hour.h"
#include "04_time/Schedule.h"

#include <iostream>


namespace synthese
{
namespace routeplanner
{






  void 
  RoutePlannerTest::setUp () 
  {

  }


  void 
  RoutePlannerTest::tearDown() 
  {

  } 





}
}


*/
