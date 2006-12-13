#include "SqlFilterTest.h"

#include "01_util/SqlFilter.h"

#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>

namespace synthese
{
namespace util
{

  namespace io = boost::iostreams;


  void 
  SqlFilterTest::setUp () 
  {

  }


  void 
  SqlFilterTest::tearDown() 
  {

  } 


  void
  SqlFilterTest::testFiltering ()
  {
      std::stringstream sout;

      SqlFilter filter;
      
      io::filtering_ostream out;
      out.push (filter);
      out.push (sout);

      {
	  out << "Hello\n\r\\\"\032;" << std::flush;
	  std::string expected ("Hello\\n\\r\\\\'\\Z,");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }


  }


}
}


