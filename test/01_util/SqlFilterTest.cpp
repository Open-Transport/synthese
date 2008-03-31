#include "01_util/SqlFilter.h"

#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;
namespace io = boost::iostreams;


BOOST_AUTO_TEST_CASE (testFiltering)
{
      std::stringstream sout;

      SqlFilter filter;
      
      io::filtering_ostream out;
      out.push (filter);
      out.push (sout);

      {
	  out << "Hello\n\r\\\"\032;" << std::flush;
	  std::string expected ("Hello\\n\\r\\\\'\\Z,");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }

}


