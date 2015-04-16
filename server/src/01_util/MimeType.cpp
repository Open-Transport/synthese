
/** MimeType class implementation.
	@file MimeType.cpp

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

#include "MimeType.hpp"

#include "Exception.h"
#include "MimeTypes.hpp"

#include <boost/algorithm/string.hpp>
#include <stdio.h> // va_start
#include <stdarg.h> // va_start

using namespace std;

using namespace boost::algorithm;

namespace synthese
{
	namespace util
	{
		MimeType::Types MimeType::_types;



		MimeType::MimeType(
			const std::string& mainType,
			const std::string& subType,
			const char* extensions,
			...
		):	_mainType(mainType),
			_subType(subType)
		{
			// Extensions
			const char* ext(extensions);
			va_list marker;
			size_t exts(0);
			for(va_start(marker, extensions); ext[0]; ext = va_arg(marker, const char*))
			{
				++exts;
				_extensions.push_back(string(ext));
			}
			va_end(marker);

			// Types collection
			if(exts)
			{
				_types.push_back(this);
			}
		}



		MimeType::MimeType():
			_mainType(MimeTypes::HTML._mainType),
			_subType(MimeTypes::HTML._subType),
			_extensions(MimeTypes::HTML._extensions)
		{}



		MimeType::MimeType(
			const std::string& fullType
		){
			vector<string> parts;
			split(parts, fullType, is_any_of("/"));
			if(parts.size() < 2)
			{
				throw Exception("No such Mime Type");
			}
			_mainType = parts[0];
			_subType = parts[1];
		}


		const std::string& MimeType::getDefaultExtension() const
		{
			return _extensions.empty() ? _subType : *_extensions.begin();
		}


		MimeType::operator std::string() const
		{
			return _mainType + "/" + _subType;
		}



		bool MimeType::operator==( const std::string& v ) const
		{
			return v == string(*this);
		}



		bool MimeType::operator==( const MimeType& v ) const
		{
			return _mainType == v._mainType && _subType == v._subType;
		}


		bool MimeType::operator!=( const MimeType& v ) const
		{
			return _mainType != v._mainType || _subType != v._subType;
		}


		bool operator==( const std::string& v, const MimeType& v1 )
		{
			return v == string(v1);
		}
}	}
