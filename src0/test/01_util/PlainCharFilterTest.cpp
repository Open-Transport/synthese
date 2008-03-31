#include "01_util/PlainCharFilter.h"

#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;
namespace io = boost::iostreams;


BOOST_AUTO_TEST_CASE (testFiltering)
{
      std::stringstream sout;

      PlainCharFilter filter;
      
      io::filtering_ostream out;
      out.push (filter);
      out.push (sout);

      {
	  out << "áàâä" << std::flush;
	  std::string expected ("aaaa");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÁÀÄÂ" << std::flush;
	  std::string expected ("AAAA");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "éèêë" << std::flush;
	  std::string expected ("eeee");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÉÈÊË" << std::flush;
	  std::string expected ("EEEE");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "íìïî" << std::flush;
	  std::string expected ("iiii");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÍÌÏÎ" << std::flush;
	  std::string expected ("IIII");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "óòöô" << std::flush;
	  std::string expected ("oooo");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÓÒÖÔ" << std::flush;
	  std::string expected ("OOOO");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "úùüû" << std::flush;
	  std::string expected ("uuuu");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÚÙÜÛ" << std::flush;
	  std::string expected ("UUUU");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ç" << std::flush;
	  std::string expected ("c");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Ç" << std::flush;
	  std::string expected ("C");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
}





