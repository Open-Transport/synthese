
/** FrenchPhoneticString class header.
	@file FrenchPhoneticString.

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_lexmatcher_FrenchPhoneticString___
#define SYNTHESE_lexmatcher_FrenchPhoneticString___

#include <string>
#include <vector>

namespace synthese
{
	namespace lexmatcher
	{
		class FrenchPhoneticString
		{
		public:
			enum Phoneme
			{
				A,
				AN,
				B,
				CH,
				D,
				E,
				F,
				G,
				I,
				J,
				K,
				L,
				M,
				N,
				O,
				P,
				R,
				S,
				T,
				U,
				V,
				Z
			};
			
			typedef std::vector<Phoneme> PhoneticString;
			typedef size_t LevenshteinDistance;
			
		private:
			std::string	_source;
			PhoneticString _phonetic;
			
			static bool _IsLast(const std::string& source, size_t pos, size_t len=1);
			static bool _IsFollowedBy(const std::string& source, size_t pos, const std::string& text);
			static bool _IsPrecededBy(const std::string& source, size_t pos, const std::string& text);
		
		public:
			FrenchPhoneticString();
			FrenchPhoneticString(const std::string& source);
			
			void setSource(const std::string& source);
			const std::string& getSource() const;
			const PhoneticString& getPhonetic() const;
			
			LevenshteinDistance levenshtein(const FrenchPhoneticString& s) const;
		};
	}
}

#endif // SYNTHESE_lexmatcher_FrenchPhoneticString___
