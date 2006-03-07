#include "RequestTest.h"

#include "70_server/Request.h"

#include <iostream>


namespace synthese
{
namespace server
{



  void 
  RequestTest::setUp () 
  {

  }


  void 
  RequestTest::tearDown() 
  {

  } 


  void
  RequestTest::testParametersParsing ()
  {
      {
	  Request request ("a=1&b=2&c=3&d=4");

	  CPPUNIT_ASSERT (request.hasParameter ("a"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("1"), request.getParameter ("a"));
	  CPPUNIT_ASSERT_EQUAL (1, request.getParameterAsInt ("a"));

	  CPPUNIT_ASSERT (request.hasParameter ("b"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("2"), request.getParameter ("b"));
	  CPPUNIT_ASSERT_EQUAL (2, request.getParameterAsInt ("b"));

	  CPPUNIT_ASSERT (request.hasParameter ("c"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("3"), request.getParameter ("c"));
	  CPPUNIT_ASSERT_EQUAL (3, request.getParameterAsInt ("c"));

	  CPPUNIT_ASSERT (request.hasParameter ("d"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("4"), request.getParameter ("d"));
	  CPPUNIT_ASSERT_EQUAL (4, request.getParameterAsInt ("d"));

	  request.addParameter ("e", "5");

	  CPPUNIT_ASSERT (request.hasParameter ("e"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("5"), request.getParameter ("e"));
	  CPPUNIT_ASSERT_EQUAL (5, request.getParameterAsInt ("e"));

	  CPPUNIT_ASSERT_EQUAL (std::string ("a=1&b=2&c=3&d=4&e=5"), request.toInternalString ());
	  
      }
      {
	  Request request ("b=2");

	  CPPUNIT_ASSERT (request.hasParameter ("b"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("2"), request.getParameter ("b"));
	  CPPUNIT_ASSERT_EQUAL (2, request.getParameterAsInt ("b"));

	  request.addParameter ("a", "1");

	  CPPUNIT_ASSERT (request.hasParameter ("a"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("1"), request.getParameter ("a"));
	  CPPUNIT_ASSERT_EQUAL (1, request.getParameterAsInt ("a"));

	  // Parameters are ordered !
	  CPPUNIT_ASSERT_EQUAL (std::string ("a=1&b=2"), request.toInternalString ());
	  
      }

  }


}
}

