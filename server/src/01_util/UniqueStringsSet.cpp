
/** UniqueStringsSet class implementation.
	@file UniqueStringsSet.cpp

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

#include "UniqueStringsSet.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	namespace util
	{
		const std::string& UniqueStringsSet::getUniqueString( const std::string& text )
		{
			string textToInsert(text);
			int i(0);
			while(_strings.find(textToInsert) != _strings.end())
			{
				++i;
				textToInsert = text + " " + lexical_cast<string>(i);
			}
			return *_strings.insert(textToInsert).first;
		}



		const UniqueStringsSet::StringsSet& UniqueStringsSet::getStringsSet() const
		{
			return _strings;
		}
	}
}
