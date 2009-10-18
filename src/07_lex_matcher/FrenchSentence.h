
/** FrenchSentence class header.
	@file FrenchSentence.h

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

#ifndef SYNTHESE_lexmatcher_FrenchSentence_h__
#define SYNTHESE_lexmatcher_FrenchSentence_h__

#include "FrenchPhoneticString.h"

#include <vector>
#include <string>

namespace synthese
{
	namespace lexmatcher
	{
		/** FrenchSentence class.
			@ingroup m07
		*/
		class FrenchSentence
		{
		public:
			typedef double ComparisonScore;
			
		private:
			std::string							_source;
			std::vector<FrenchPhoneticString>	_words;
			
			static std::string	_ConvertAlias(const std::string& source);
			
			
		public:
			FrenchSentence();
			FrenchSentence(const std::string& source);
			
			const std::string& getSource() const;
			
			ComparisonScore compare(const FrenchSentence& s) const;
			
			int operator<(const FrenchSentence& s) const;
			bool operator==(const FrenchSentence& s) const;
		};
	}
}

#endif // SYNTHESE_lexmatcher_FrenchSentence_h__
