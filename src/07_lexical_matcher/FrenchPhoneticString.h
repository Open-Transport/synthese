
/** FrenchPhoneticString class header.
	@file FrenchPhoneticString.h

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

#ifndef SYNTHESE_lexmatcher_FrenchPhoneticString___
#define SYNTHESE_lexmatcher_FrenchPhoneticString___

#include "IConv.hpp"

#include <string>
#include <vector>
#include <limits>
#include <boost/thread/mutex.hpp>

#undef max
#undef min

namespace synthese
{
	namespace lexical_matcher
	{
		//////////////////////////////////////////////////////////////////////////
		/// Phonetic encoding for French language.
		/// Strings are converted to CP1252 8-bit strings before encoding.
		/// This encoding allows French characters like œ.
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
				ON,
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
			static util::IConv ICONV;
			static std::string ACCENTUATED_A;
			static std::string ACCENTUATED_E;
			static std::string ACCENTUATED_I;
			static std::string ACCENTUATED_O;
			static std::string ACCENTUATED_U;
			static std::string VOWELS;
			static std::string C_VOWELS;

			std::string	_source;
			std::string _plainLowerSource;
			PhoneticString _phonetic;

			static boost::mutex _IConvMutex;

			static bool _IsLast(const std::string& source, size_t pos, size_t len=1);
			static bool _IsFollowedBy(const std::string& source, size_t pos, const std::string& text);
			static bool _IsPrecededBy(const std::string& source, size_t pos, const std::string& text);
			static bool _IsAnyOf(const std::string& source, size_t pos, const std::string& chars);

			static std::string _to_plain_lower_copy_8bit(const std::string& source);

			//////////////////////////////////////////////////////////////////////////
			/// Try to convert the given string in UTF-8 to internal 8-bit enconding
			/// (CP1252). Return an empty string in case of conversion failure.
			/// @param text text to convert
			/// @author Sylvain Pasche
			/// @since 3.3.0
			/// @date 2011
			static std::string _convertTo8bits(const std::string& text);

		public:
			FrenchPhoneticString();
			FrenchPhoneticString(const std::string& source);

			bool startsWith(const FrenchPhoneticString& s) const;
			void setSource(const std::string& source);
			const std::string& getSource() const;
			const std::string& getPlainLowerSource() const { return _plainLowerSource; };
			const PhoneticString& getPhonetic() const;
			std::string getPhoneticString() const;
			static std::string to_plain_lower_copy(const std::string& text);

			LevenshteinDistance levenshtein(const FrenchPhoneticString& s) const;

			template<class T>
			static bool startsWithExact(const T& s1, const T& s2)
			{
				if(s1.size() > s2.size()) return false;

				size_t pos(0);
				for(;pos < s1.size();pos++)
					if(s1[pos] != s2[pos])
						return false;

				return true;
			}

			template<class T>
			static LevenshteinDistance Levenshtein(const T& s1, const T& s2)
			{
				if (s2.size () >= 256 || s1.size() >= 256) return std::numeric_limits<LevenshteinDistance>::max ();

				// Levenshtein Word Distance matrix.
				// Note that the dims are bounded to 256. It means that it is
				// forbidden to compare words larger than 256 characters each!
				LevenshteinDistance matrix[256][256];

				LevenshteinDistance n = s2.size();
				LevenshteinDistance m = s1.size();

				if (n == 0) return m;
				if (m == 0) return n;


				for(LevenshteinDistance i = 0; i <= n; i++) { matrix[i][0] = i; }
				for(LevenshteinDistance j = 1; j <= m; j++) { matrix[0][j] = j; }

				for (LevenshteinDistance i = 1; i <= n; i++ )
				{
					char sc = s2[i-1];
					for (LevenshteinDistance j = 1; j <= m;j++)
					{
						LevenshteinDistance v = matrix[i-1][j-1];
						if ( s1[j-1] !=  sc ) v++;
						matrix[i][j] = std::min(std::min(matrix[i-1][j] + 1, matrix[i][j-1] + 1 ), v );
					}
				}
				return matrix[n][m];
			}
		};
	}
}

#endif // SYNTHESE_lexmatcher_FrenchPhoneticString___
