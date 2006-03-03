#include "HtmlFilterTest.h"
#include "HtmlFilter.h"

#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>

namespace synthese
{
namespace util
{

  namespace io = boost::iostreams;


  void 
  HtmlFilterTest::setUp () 
  {

  }


  void 
  HtmlFilterTest::tearDown() 
  {

  } 


  void
  HtmlFilterTest::testFiltering ()
  {
      std::stringstream sout;

      HtmlFilter filter;
      
      io::filtering_ostream out;
      out.push (filter);
      out.push (sout);

      {
	  out << "áàâä" << std::flush;
	  std::string expected ("&aacute;&agrave;&acirc;&auml;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÁÀÄÂ" << std::flush;
	  std::string expected ("&Aacute;&Agrave;&Auml;&Acirc;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "éèêë" << std::flush;
	  std::string expected ("&eacute;&egrave;&ecirc;&euml;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÉÈÊË" << std::flush;
	  std::string expected ("&Eacute;&Egrave;&Ecirc;&Euml;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "íìïî" << std::flush;
	  std::string expected ("&iacute;&igrave;&iuml;&icirc;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÍÌÏÎ" << std::flush;
	  std::string expected ("&Iacute;&Igrave;&Iuml;&Icirc;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "óòöô" << std::flush;
	  std::string expected ("&oacute;&ograve;&ouml;&ocirc;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÓÒÖÔ" << std::flush;
	  std::string expected ("&Oacute;&Ograve;&Ouml;&Ocirc;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "úùüû" << std::flush;
	  std::string expected ("&uacute;&ugrave;&uuml;&ucirc;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÚÙÜÛ" << std::flush;
	  std::string expected ("&Uacute;&Ugrave;&Uuml;&Ucirc;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ç" << std::flush;
	  std::string expected ("&ccedil;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Ç" << std::flush;
	  std::string expected ("&Ccedil;");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Salut tout le monde" << std::flush;
	  std::string expected ("Salut tout le monde");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
  }


}
}

