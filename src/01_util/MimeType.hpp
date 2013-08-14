
/** MimeType class header.
	@file MimeType.hpp

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

#ifndef SYNTHESE_util_MimeType_hpp__
#define SYNTHESE_util_MimeType_hpp__

#include <vector>
#include <string>

namespace synthese
{
	namespace util
	{
		class MimeTypes;

		/** MimeType class.
			@ingroup m01
		*/
		class MimeType
		{
		public:
			typedef std::vector<std::string> Extensions;
			typedef std::vector<const MimeType*> Types;

		private:
			std::string _mainType;
			std::string _subType;
			Extensions _extensions;

			static Types _types;

			friend class MimeTypes;

		public:
			MimeType(
				const std::string& fullType
			);

			MimeType(
				const std::string& mainType,
				const std::string& subType,
				const char* extensions,
				...
			);
			MimeType();

			const std::string& getMainType() const { return _mainType; }
			const std::string& getSubType() const { return _subType; }
			const Extensions& getExtensions() const { return _extensions; }
			const std::string& getDefaultExtension() const;

			operator std::string() const;
			bool operator==(const std::string& v) const;
			bool operator==(const MimeType& v) const;
			bool operator!=(const MimeType& v) const;
		};

		bool operator==(const std::string& v, const MimeType& v1);
	}
}

#endif // SYNTHESE_util_MimeType_hpp__

