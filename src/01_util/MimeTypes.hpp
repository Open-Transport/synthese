
/** MimeTypes class header.
	@file MimeTypes.hpp

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

#ifndef SYNTHESE_util_MimeTypes_hpp__
#define SYNTHESE_util_MimeTypes_hpp__

#include <string>

#include "MimeType.hpp"

namespace synthese
{
	namespace util
	{
		/** MimeTypes class.
			@ingroup m01
		*/
		class MimeTypes
		{
		private:
			MimeTypes();

		public:
			static const MimeType CSS;
			static const MimeType CSV;
			static const MimeType GIF;
			static const MimeType HTML;
			static const MimeType JPEG;
			static const MimeType JS;
			static const MimeType JSON;
			static const MimeType PDF;
			static const MimeType PNG;
			static const MimeType TEXT;
			static const MimeType XML;
			static const MimeType TTF;
			
			static const MimeType& GetMimeTypeByExtension(
				const std::string& extension
			); 

			static const MimeType& GetMimeTypeByString(
				const std::string& fullType
			); 
		};
}	}

#endif // SYNTHESE_util_MimeTypes_hpp__

