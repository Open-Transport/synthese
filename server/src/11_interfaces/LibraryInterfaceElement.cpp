
/** LibraryInterfaceElement class header.
	@file LibraryInterfaceElement.cpp

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

#include "LibraryInterfaceElement.h"

#include "ValueElementList.h"
#include "InterfacePageException.h"

#include "Factory.h"
#include "FactoryException.h"
#include "Registry.h"

#include <sstream>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace interfaces
	{
		boost::shared_ptr<LibraryInterfaceElement> LibraryInterfaceElement::create( const std::string & text, const InterfacePage* page )
		{
			// Trim the left spaces
			size_t start_pos;

			for (start_pos = 0; start_pos < text.size() && text[start_pos] == ' '; ++start_pos) ;

			// Empty line : return null
			if (start_pos >= text.size())
				return boost::shared_ptr<LibraryInterfaceElement>();

			// Search for the end of the keyword
			size_t word_end_pos = start_pos;
			for (; word_end_pos < text.size() && text[word_end_pos] != ' '; ++word_end_pos) ;

			boost::shared_ptr<LibraryInterfaceElement> lie;
			try
			{
				lie.reset(Factory<LibraryInterfaceElement>::create(text.substr(start_pos, word_end_pos - start_pos)));
			}
			catch (FactoryException<LibraryInterfaceElement>&)
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

		const string LibraryInterfaceElement::getValue(
			const interfaces::ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object /*= NULL */
			, const server::Request* request /*= NULL  */
		) const	{
			stringstream s;
			display(s, parameters, variables, object, request);
			return s.str();
		}

		bool LibraryInterfaceElement::isFalse( const ParametersVector& pv, interfaces::VariablesMap& variables , const void* object /*= NULL */, const server::Request* request /*= NULL  */ ) const
		{
			string value(getValue(pv, variables, object, request));
			return value.empty() || value == "0" || value == lexical_cast<string>(UNKNOWN_VALUE);
		}

		void LibraryInterfaceElement::setPage(const InterfacePage* page )
		{
			_page = page;
		}
	}
}
