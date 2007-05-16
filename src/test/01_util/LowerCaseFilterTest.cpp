#include "01_util/LowerCaseFilter.h"

#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;
namespace io = boost::iostreams;


BOOST_AUTO_TEST_CASE (testEncodingDecoding)
{
      std::stringstream sout;

      io::filtering_ostream out;
      out.push (LowerCaseFilter());
      out.push (sout);


      {
	  out << "HeLLo WorLD !!!" << std::endl ;
	  std::string expected ("hello world     ");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  
	  out << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << std::flush ;;
	  std::string expected ("abcdefghijklmnopqrstuvwxyz");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Abc 1234 :,????Z" << std::flush ;;
	  std::string expected ("abc 1234       z");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
}

