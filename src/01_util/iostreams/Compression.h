#ifndef SYNTHESE_UTIL_COMPRESSION_H
#define SYNTHESE_UTIL_COMPRESSION_H

#include <iostream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/combine.hpp>
#include <boost/iostreams/filter/zlib.hpp>


namespace synthese
{
	namespace util
	{

		/** Service class for compressing data.
		    Note : Prefer these methods to boost ones (difficult to use and understand).

		@ingroup m01
		*/
		class Compression
		{
		private:

			Compression ();
			~Compression ();

		public:
			/**
			   Compress data incoming from a stream to another.
			*/
			static void ZlibCompress (std::istream& is, std::ostream& o);

			/**
			   Decompress data incoming from a stream to another.
			*/
			static void ZlibDecompress (std::istream& is, std::ostream& os);

		};
	}
}
#endif
