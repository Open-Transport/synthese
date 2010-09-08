
#include "05_html/HTMLFilter.h"

#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>



#include <boost/test/auto_unit_test.hpp>

using namespace synthese::html;
namespace io = boost::iostreams;


BOOST_AUTO_TEST_CASE (testFiltering)
{
    
      std::stringstream sout;

      HTMLFilter filter;
      
      io::filtering_ostream out;
      out.push (filter);
      out.push (sout);

      {
	  out << "Ã¡Ã Ã¢Ã" << std::flush;
	  std::string expected ("&aacute;&agrave;&acirc;&auml;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÃÃ¤ÃÃ" << std::flush;
	  std::string expected ("&Aacute;&Agrave;&Auml;&Acirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Ã©ÃÃªÃ«" << std::flush;
	  std::string expected ("&eacute;&egrave;&ecirc;&euml;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÃÃÃ¦Ã" << std::flush;
	  std::string expected ("&Eacute;&Egrave;&Ecirc;&Euml;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Ã­Ã¬Ã¯Ã®" << std::flush;
	  std::string expected ("&iacute;&igrave;&iuml;&icirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÃÃ¼ÃÃ´" << std::flush;
	  std::string expected ("&Iacute;&Igrave;&Iuml;&Icirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Ã³Ã²Ã¶Ã" << std::flush;
	  std::string expected ("&oacute;&ograve;&ouml;&ocirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÃÃÃÃ" << std::flush;
	  std::string expected ("&Oacute;&Ograve;&Ouml;&Ocirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÃºÃ¹ÃÃ»" << std::flush;
	  std::string expected ("&uacute;&ugrave;&uuml;&ucirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Ã¨ÃÃ½Ã" << std::flush;
	  std::string expected ("&Uacute;&Ugrave;&Uuml;&Ucirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Ã§" << std::flush;
	  std::string expected ("&ccedil;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Ã" << std::flush;
	  std::string expected ("&Ccedil;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Salut tout le monde" << std::flush;
	  std::string expected ("Salut tout le monde");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
	  
}

