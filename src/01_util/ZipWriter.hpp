
/** ZipWriter class header.
	@file ZipWriter.hpp

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

/**
	Note:
	Current class was inspired by Google protobuf code : http://code.google.com/p/protobuf/
	Google protobuf (Protocol Buffers) is copyrighted by Google Inc.
*/

#ifndef SYNTHESE_util_ZipWriter_hpp__
#define SYNTHESE_util_ZipWriter_hpp__

#ifdef WIN32
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif
#include <sstream>
#include <vector>

class ZipWriter {
    public:
        ZipWriter(std::ostream & stream);
        ~ZipWriter();

        void Write(const std::string& filename, std::stringstream& contents);
        void WriteDirectory();

    private:
        struct FileInfo {
            std::string name;
            uint32_t offset;
            uint32_t size;
            uint32_t crc32;
        };

        std::vector<FileInfo> files_;

		std::ostream & stream_;
        uint32_t dir_len_;
        void WriteLittleEndian32(uint32_t val);
        void WriteShort(uint16_t val);
        void WriteString(const std::string& str);
};

#endif // SYNTHESE_util_ZipWriter_hpp__
