
/** UniqueStringsSet class header.
	@file UniqueStringsSet.h

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

#ifndef SYNTHESE_util_UniquesStringsSet_h__
#define SYNTHESE_util_UniquesStringsSet_h__

#include <set>
#include <string>

namespace synthese
{
	namespace util
	{
		/** Set of strings with auto differentiation at insertion if two strings are identical.
			@ingroup m01

			If two strings are identical, the second one is differentiated by adding a number at its end, after a space.

			Example :
				- dummy
				- dummy 1
				- dummy 2

			If a "dummy 2" string is added, then a second number is added :
				- dummy 2 1
		*/
		class UniqueStringsSet
		{
		public:
			typedef std::set<std::string> StringsSet;

		private:
			StringsSet _strings;

		public:
			const std::string& getUniqueString(const std::string& text);
			const StringsSet& getStringsSet() const;

		};
	}
}

#endif // SYNTHESE_util_UniquesStringsSet_h__
