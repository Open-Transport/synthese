#include "01_util/iostreams/Compression.h"

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
	std::stringstream tmp;

	std::stringstream ss;
	filtering_stream<output> fs;
	fs.push (zlib_compressor());
	fs.push (tmp);

	boost::iostreams::copy (is, fs);
	fs.pop ();

	int size = tmp.str ().length ();

	os << size << '#';
	boost::iostreams::copy (tmp, os);

    }



    void Compression::ZlibDecompress (std::istream& is, std::ostream& os)
    {
	char buffer[32];
	is.getline (buffer, sizeof(buffer), '#');
	int size (atoi (buffer));
	if (size == 0) return;

	filtering_stream<input> fs;

	fs.push (zlib_decompressor());
	fs.push (restrict (is, 0, size));

	boost::iostreams::copy(fs, os);
	// fs.set_auto_close (false);
	fs.pop ();

	// Warning filtering stream is closed at destruction causing all underlying device to be clolsed as well.

    }





}
}

