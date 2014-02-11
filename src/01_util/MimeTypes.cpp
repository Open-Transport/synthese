
/** MimeTypes class implementation.
	@file MimeTypes.cpp

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

#include "MimeTypes.hpp"

#include "Exception.h"

#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	namespace util
	{
		const MimeType MimeTypes::CSS("text", "css", "css", "");
		const MimeType MimeTypes::CSV("text", "csv", "csv", "");
		const MimeType MimeTypes::HTML("text", "html", "html", "htm", "");
		const MimeType MimeTypes::JSON("application", "json", "json", "");
		const MimeType MimeTypes::XML("text", "xml", "xml", "");
		const MimeType MimeTypes::JS("application", "javascript", "js", "");
		const MimeType MimeTypes::PNG("image", "png", "png", "");
		const MimeType MimeTypes::JPEG("image", "jpeg", "jpg", "jpeg", "");
		const MimeType MimeTypes::TEXT("text", "plain", "txt", "");
		const MimeType MimeTypes::GIF("image", "gif", "gif", "");
		const MimeType MimeTypes::PDF("application", "pdf", "pdf", "");
		const MimeType MimeTypes::TTF("application", "x-font-ttf", "ttf", "");



		const MimeType& MimeTypes::GetMimeTypeByExtension(
			const std::string& extension
		){
			BOOST_FOREACH(const MimeType* mimeType, MimeType::_types)
			{
				BOOST_FOREACH(const std::string& ext, mimeType->getExtensions())
				{
					if(ext == extension)
					{
						return *mimeType;
					}
				}
			}
			throw Exception("Mime type not found");
		}



		const MimeType& MimeTypes::GetMimeTypeByString( const std::string& fullType )
		{
			BOOST_FOREACH(const MimeType* mimeType, MimeType::_types)
			{
				if(string(*mimeType) == fullType)
				{
					return *mimeType;
				}
			}
			throw Exception("Mime type not found");
		}
}	}
