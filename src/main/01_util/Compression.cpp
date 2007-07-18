#include "01_util/Compression.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/restrict.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include <sstream>


using namespace boost::iostreams;


namespace synthese
{

namespace util
{



    void Compression::ZlibCompress (std::istream& is, std::ostream& os)
    {
	// Note : this is important to add the block size in order to be able to exchange
	// several messages accross not EOF bounded streams (TCP dialog for instance).
	
	std::stringstream ss;
	filtering_stream<output> fs;
	fs.push (zlib_compressor());
	fs.push (ss);
	
	boost::iostreams::copy (is, fs);
	fs.reset ();
	
	os << ss.rdbuf()->in_avail () << '#';
	boost::iostreams::copy (ss, os);
    }
    

    void Compression::ZlibDecompress (std::istream& is, std::ostream& os)
    {
	static char buffer[32];
	is.getline (buffer, sizeof(buffer), '#');
	
	int size (atoi (buffer));

	filtering_stream<input> fs;
	fs.push (zlib_decompressor());
	fs.push (restrict (is, 0, size));
	
	boost::iostreams::copy(fs, os);
    }





}
}

