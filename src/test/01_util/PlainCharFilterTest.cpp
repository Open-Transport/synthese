#include "PlainCharFilterTest.h"

#include "01_util/PlainCharFilter.h"

#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>

namespace synthese
{
namespace util
{

  namespace io = boost::iostreams;


  void 
  PlainCharFilterTest::setUp () 
  {

  }


  void 
  PlainCharFilterTest::tearDown() 
  {

  } 


  void
  PlainCharFilterTest::testFiltering ()
  {
      std::stringstream sout;

      PlainCharFilter filter;
      
      io::filtering_ostream out;
      out.push (filter);
      out.push (sout);

      {
	  out << "áàâä" << std::flush;
	  std::string expected ("aaaa");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÁÀÄÂ" << std::flush;
	  std::string expected ("AAAA");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "éèêë" << std::flush;
	  std::string expected ("eeee");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÉÈÊË" << std::flush;
	  std::string expected ("EEEE");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "íìïî" << std::flush;
	  std::string expected ("iiii");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÍÌÏÎ" << std::flush;
	  std::string expected ("IIII");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "óòöô" << std::flush;
	  std::string expected ("oooo");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÓÒÖÔ" << std::flush;
	  std::string expected ("OOOO");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "úùüû" << std::flush;
	  std::string expected ("uuuu");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ÚÙÜÛ" << std::flush;
	  std::string expected ("UUUU");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "ç" << std::flush;
	  std::string expected ("c");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Ç" << std::flush;
	  std::string expected ("C");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
  }


}
}


