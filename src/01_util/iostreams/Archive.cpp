#include "01_util/iostreams/Archive.h"
#include "01_util/Conversion.h"

#include <boost/iostreams/copy.hpp>
#include <boost/filesystem/operations.hpp>


#include <sstream>
#include <fstream>
#include <iomanip>


using synthese::util::Conversion;

using namespace boost::iostreams;
using namespace boost::filesystem;


namespace synthese
{

namespace util
{


    const char Archive::LINK_INDICATOR_NORMAL_FILE ('0');
    const char Archive::LINK_INDICATOR_NORMALOLD_FILE ('\0');
    const char Archive::LINK_INDICATOR_HARDLINK_FILE ('1');
    const char Archive::LINK_INDICATOR_SYMBOLIC_FILE ('2');
    const char Archive::LINK_INDICATOR_CHARSPECIAL_FILE ('3');
    const char Archive::LINK_INDICATOR_BLOCKSPECIAL_FILE ('4');
    const char Archive::LINK_INDICATOR_DIRECTORY_FILE ('5');
    const char Archive::LINK_INDICATOR_FIFO_FILE ('6');
    const char Archive::LINK_INDICATOR_CONTIGUOUS_FILE ('7');



    void Archive::Tar (const boost::filesystem::path& baseDir,
		       const boost::filesystem::path& relativePath,
		       std::ostream& os)
    {
		Archive::Tar (baseDir, relativePath, os, true);
	}



    void Archive::Tar (const boost::filesystem::path& baseDir,
		       const boost::filesystem::path& relativePath,
		       std::ostream& os, bool recursive)
    {
		// The ostream MUST have been opened with std::ios_base::binary
		// otherwise corrupted archive under windows

	boost::filesystem::path entry (baseDir / relativePath);

	bool isDir (boost::filesystem::is_directory (entry));
	char linkIndicator = LINK_INDICATOR_NORMAL_FILE;
	std::string filename (relativePath.string ());

	if (isDir)
	{
	    linkIndicator = LINK_INDICATOR_DIRECTORY_FILE;
	    filename.append ("/");
	}
	else
	{
	    // assert (is_regular (entry));
	}

	// First write header
        // 	  100 	name 	name of file
        // 	    8 	mode 	file mode
        // 	    8 	uid 	owner user ID
        // 	    8 	gid 	owner group ID
        // 	    12 	size 	length of file in bytes
        // 	    12 	mtime 	modify time of file
        // 	    8 	chksum 	checksum for header
        // 	    1 	link 	indicator for links
        // 	  100 	linkname 	name of linked file

	long size = isDir ? 0 : file_size (entry);
	std::time_t lastModificationTime = last_write_time (entry);

	std::stringstream ss;

	ss << Conversion::ToPostpaddedString (filename, '\0', 100);
	ss << (isDir ? "0000755" : "0000644") << '\0';  // posix permissions are lost; could do better.
	ss << "0000000" << '\0';
	ss << "0000000" << '\0';
	ss << Conversion::ToPrepaddedString (Conversion::ToOctalString (size), '0', 11) << '\0';
	ss << Conversion::ToPrepaddedString (Conversion::ToOctalString (lastModificationTime), '0', 11) << '\0';
	ss << "        "; // temporary blanks for checksum calculation
	ss << linkIndicator;
	ss << Conversion::ToPostpaddedString ("", '\0', 100);
	ss << "ustar  " << '\0';
	ss << Conversion::ToPostpaddedString ("", '\0', 32);
	ss << Conversion::ToPostpaddedString ("", '\0', 32);

	ss << Conversion::ToPostpaddedString ("", '\0', 183);


	std::string header = ss.str ();
	int checksum = 0;
	for (int i=0; i<512; ++i) checksum += ((int) header[i]);

	std::string checksumstr (Conversion::ToPrepaddedString (Conversion::ToOctalString (checksum), '0', 6));
	for (int i=0; i<6; ++i) header[148+i] = checksumstr[i];
	header[148+6] = '\0';

	os << header << std::flush;

	if (isDir)
	{
	    // recursively add directory entries
	    directory_iterator end_iter;
	    for ( directory_iterator dir_itr(entry);
		  dir_itr != end_iter;
		  ++dir_itr )
	    {
		path p (relativePath / dir_itr->leaf ());
		Archive::Tar (baseDir, p, os, recursive);
	    }
	}
	else
	{
	    char buffer[512];
		std::ifstream ifs (entry.string ().c_str (), std::ifstream::binary);
	    while (!ifs.eof ())
	    {
			memset (buffer, '\0', 512);
			ifs.read (buffer, 512);
			if (ifs.gcount () > 0)
			{
				os.write (buffer, 512);
				os.flush ();
			}
		}


	    ifs.close ();

	}

    }


/* sample code
    {
	std::ofstream taros ("/tmp/unit.tar", std::ios_base::binary);
	boost::filesystem::path basedir ("/home/mjambert/Workspace/chouette/src/main/webapp");
	{
	    boost::filesystem::path entry ("js");
	    Archive::Tar (basedir, entry, taros);
	}
	{
	    boost::filesystem::path entry ("test.swf");
	    Archive::Tar (basedir, entry, taros);
	}

	taros.close ();
    }
*/

}
}

