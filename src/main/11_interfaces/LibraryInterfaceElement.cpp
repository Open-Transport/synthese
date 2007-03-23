
/** LibraryInterfaceElement class header.
	@file LibraryInterfaceElement.cpp

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

#include "01_util/FactoryException.h"

#include "11_interfaces/LibraryInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

namespace synthese
{
	using namespace util;

	namespace interfaces
	{
		LibraryInterfaceElement* LibraryInterfaceElement::create( const std::string & text, const InterfacePage* page )
		{
			// Trim the left spaces
			size_t start_pos;

			for (start_pos = 0; start_pos < text.size() && text[start_pos] == ' '; ++start_pos);

			// Empty line : return null
			if (start_pos >= text.size())
				return NULL;

			// Search for the end of the keyword
			size_t word_end_pos = start_pos;
			for (; word_end_pos < text.size() && text[word_end_pos] != ' '; ++word_end_pos);

			LibraryInterfaceElement* lie = NULL;
			try
			{
				lie = Factory<LibraryInterfaceElement>::create(text.substr(start_pos, word_end_pos - start_pos));
			}
			catch (FactoryException<LibraryInterfaceElement> e)
			{
				throw InterfacePageException("Specified interface function not found " + text.substr(start_pos, word_end_pos - start_pos));
			}
			lie->_page = page;
			lie->parse((text.size() > word_end_pos + 1)
				? text.substr(word_end_pos + 1, text.size() - word_end_pos - 1) 
				: "");

			return lie;
		}

		void LibraryInterfaceElement::parse( const std::string& text )
		{
			ValueElementList vai(text, _page);
			storeParameters(vai);
		}
	}
}
