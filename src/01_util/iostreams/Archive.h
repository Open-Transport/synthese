
/** Archive utility class header.
	@file Archive.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_UTIL_ARCHIVE_H
#define SYNTHESE_UTIL_ARCHIVE_H


#include <iostream>


#include <boost/logic/tribool.hpp>
#include <boost/filesystem/path.hpp>




namespace synthese
{
	namespace util
	{

		/**
		    @ingroup m01
		*/
		class Archive
		{
		private:

		    static const char LINK_INDICATOR_NORMAL_FILE;
		    static const char LINK_INDICATOR_NORMALOLD_FILE;    // Unhandled
		    static const char LINK_INDICATOR_HARDLINK_FILE;     // Unhandled
		    static const char LINK_INDICATOR_SYMBOLIC_FILE;     // Unhandled
		    static const char LINK_INDICATOR_CHARSPECIAL_FILE;  // Unhandled
		    static const char LINK_INDICATOR_BLOCKSPECIAL_FILE; // Unhandled
		    static const char LINK_INDICATOR_DIRECTORY_FILE;
		    static const char LINK_INDICATOR_FIFO_FILE;         // Unhandled
		    static const char LINK_INDICATOR_CONTIGUOUS_FILE;   // Unhandled

		    Archive ();
		    ~Archive ();

			/** Tar a file or directory and writes output to os.
			    This is a naive implementation, yet portable. ustar extension is not handled
			    nor posix permissions.
			    This method can be called any number of times to add more entries
			    to the tarball.
			*/
			static void Tar (const boost::filesystem::path& baseDir,
					 const boost::filesystem::path& relativePath,
					 std::ostream& os, bool recursive);

		public:

			static void Tar (const boost::filesystem::path& baseDir,
					 const boost::filesystem::path& relativePath,
					 std::ostream& os);

		};
	}
}
#endif
