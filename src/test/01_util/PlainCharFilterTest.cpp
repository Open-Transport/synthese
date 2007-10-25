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
	  out << "áàâ�" << std::flush;
	  std::string expected ("aaaa");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "�ä��" << std::flush;
	  std::string expected ("AAAA");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "é�êë" << std::flush;
	  std::string expected ("eeee");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "��æ�" << std::flush;
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
	  out << "�ü�ô" << std::flush;
	  std::string expected ("IIII");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "óòö�" << std::flush;
	  std::string expected ("oooo");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "����" << std::flush;
	  std::string expected ("OOOO");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "úù�û" << std::flush;
	  std::string expected ("uuuu");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "è�ý�" << std::flush;
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
	  out << "�" << std::flush;
	  std::string expected ("C");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
}





