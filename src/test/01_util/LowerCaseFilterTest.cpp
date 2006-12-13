#include "LowerCaseFilterTest.h"

#include "01_util/LowerCaseFilter.h"

#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>

namespace synthese
{
namespace util
{

  namespace io = boost::iostreams;


  void 
  LowerCaseFilterTest::setUp () 
  {

  }


  void 
  LowerCaseFilterTest::tearDown() 
  {

  } 


  void
  LowerCaseFilterTest::testFiltering ()
  {
      std::stringstream sout;

      io::filtering_ostream out;
      out.push (LowerCaseFilter());
      out.push (sout);


      {
	  out << "HeLLo WorLD !!!" << std::endl ;
	  std::string expected ("hello world     ");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  
	  out << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << std::flush ;;
	  std::string expected ("abcdefghijklmnopqrstuvwxyz");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      sout.str ("");
      {
	  out << "Abc 1234 :,????Z" << std::flush ;;
	  std::string expected ("abc 1234       z");
	  CPPUNIT_ASSERT_EQUAL (expected, sout.str ());
      }
      
  }


}
}


