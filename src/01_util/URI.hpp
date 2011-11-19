
/** URI class header.
	@file URI.hpp

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

#ifndef SYNTHESE_util_URI_hpp__
#define SYNTHESE_util_URI_hpp__

#include <string>

namespace synthese
{
	namespace util
	{
		//////////////////////////////////////////////////////////////////////////
		/// URI class.
		///	@ingroup m01
		///	@author Hugues Romain
		/// @since 3.3.0
		/// @date 2011
		//////////////////////////////////////////////////////////////////////////
		/// Links :
		///	<ul>
		///		<li>http://www.codeguru.com/cpp/cpp/cpp_mfc/article.php/c4029</li>
		///		<li>http://www.rfc-editor.org/rfc/rfc1738.txt</li>
		///	</ul>
		class URI
		{
		public:
			static const std::string PARAMETER_SEPARATOR;
			static const std::string PARAMETER_ASSIGNMENT;




			//////////////////////////////////////////////////////////////////////////
			/// Perform URL-decoding on a string.
			/// @param in the string to decode
			/// @retval out the decoded string
			/// @return false if the encoding was invalid.
			static bool Decode(const std::string& in, std::string& out);



			//////////////////////////////////////////////////////////////////////////
			/// Encodes a string to the format of an URL
			/// @param value the character to encode
			/// @return string the encoded string
			/// @auhtor Hugues Romain
			/// @date 2010
			/// @since 3.1.18
			//////////////////////////////////////////////////////////////////////////
			static std::string Encode(const std::string& value);

		private:
			URI();

		};
	}
}

#endif // SYNTHESE_util_URI_hpp__
