#include "01_util/iostreams/Compression.h"

#pragma GCC diagmostic ignored "-Werror=sign-compare"
#include <boost/test/auto_unit_test.hpp>
#pragma GCC diagnostic pop

#include <iostream>

using namespace synthese::util;


BOOST_AUTO_TEST_CASE (testZlibCompression)
{
    {
	std::string s ( "create table toto(tt)");
	std::stringstream ss1;
	ss1 << s;
	std::stringstream ss2;
	Compression::ZlibCompress (ss1, ss2);
	std::stringstream ss3;
	Compression::ZlibDecompress (ss2, ss3);
	BOOST_CHECK_EQUAL (s, ss3.str ());
    }
/*    {
	std::string s ( "This is a sample text for compression. This is a sample text for compression.");
	std::stringstream ss1;
	ss1 << s;
	std::stringstream ss2;
	Compression::ZlibCompress (ss1, ss2);
	std::stringstream ss3;
	Compression::ZlibDecompress (ss2, ss3);
	BOOST_CHECK_EQUAL (s, ss3.str ());
    }
*/
/*
    {
	// 0 char test...
	std::string s;
	std::stringstream ss1;
	ss1 << s;
	std::stringstream ss2;
	Compression::ZlibCompress (ss1, ss2);
	std::stringstream ss3;
	Compression::ZlibDecompress (ss2, ss3);
	BOOST_CHECK_EQUAL (s, ss3.str ());
    }
    {
	// 1 char test...
	std::string s ( "T");
	std::stringstream ss1;
	ss1 << s;
	std::stringstream ss2;
	Compression::ZlibCompress (ss1, ss2);
	std::stringstream ss3;
	Compression::ZlibDecompress (ss2, ss3);
	std::cerr << "<" << s << ">" << "   " << "<" << ss3.str () << ">" << std::endl;
	BOOST_CHECK_EQUAL (s, ss3.str ());
    }
    {
	std::string s;
	for (int i=0; i<10000; ++i) s += ((char) (i % 255));
	std::stringstream ss1;
	ss1 << s;
	std::stringstream ss2;
	Compression::ZlibCompress (ss1, ss2);
	std::stringstream ss3;
	Compression::ZlibDecompress (ss2, ss3);
	BOOST_CHECK_EQUAL (s, ss3.str ());
    }
*/

}


