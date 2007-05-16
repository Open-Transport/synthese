
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
	  out << "áàâä" << std::flush;
	  std::string expected ("&aacute;&agrave;&acirc;&auml;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÁÀÄÂ" << std::flush;
	  std::string expected ("&Aacute;&Agrave;&Auml;&Acirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "éèêë" << std::flush;
	  std::string expected ("&eacute;&egrave;&ecirc;&euml;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÉÈÊË" << std::flush;
	  std::string expected ("&Eacute;&Egrave;&Ecirc;&Euml;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "íìïî" << std::flush;
	  std::string expected ("&iacute;&igrave;&iuml;&icirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÍÌÏÎ" << std::flush;
	  std::string expected ("&Iacute;&Igrave;&Iuml;&Icirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "óòöô" << std::flush;
	  std::string expected ("&oacute;&ograve;&ouml;&ocirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÓÒÖÔ" << std::flush;
	  std::string expected ("&Oacute;&Ograve;&Ouml;&Ocirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "úùüû" << std::flush;
	  std::string expected ("&uacute;&ugrave;&uuml;&ucirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÚÙÜÛ" << std::flush;
	  std::string expected ("&Uacute;&Ugrave;&Uuml;&Ucirc;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ç" << std::flush;
	  std::string expected ("&ccedil;");
	  BOOST_REQUIRE_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Ç" << std::flush;
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

