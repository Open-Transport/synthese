
/** ValueElementList class implementation.
	@file ValueElementList.cpp

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

#include <vector>

#include "ValueElementList.h"
#include "ParameterValueInterfaceElement.h"
#include "LibraryInterfaceElement.h"
#include "StaticValueInterfaceElement.h"
#include "InterfacePageException.h"

#include "Exception.h"
#include "Factory.h"

#include <boost/lexical_cast.hpp>

using namespace boost;

namespace synthese
{
	using namespace std;
	using namespace util;

	namespace interfaces
	{
		ValueElementList::ValueElementList(const std::string& text, const InterfacePage* page)
		{
			parse(text, page);
		}


		ValueElementList::ValueElementList()
		{
		}

		const ParametersVector ValueElementList::fillParameters(
			const ParametersVector& parameters
			, VariablesMap& vars
			, const void* object, const server::Request* request ) const
		{
			ParametersVector pv;
			for (ElementsList::const_iterator it = _elements.begin(); it != _elements.end(); ++it)
			{
				pv.push_back((*it)->getValue(parameters, vars, object, request));
			}
			return pv;
		}

		boost::shared_ptr<LibraryInterfaceElement> ValueElementList::front()
		{
			if (_elements.size() > 0)
			{
				boost::shared_ptr<LibraryInterfaceElement> vie(_elements.front());
				_elements.pop_front();
				return vie;
			}
			else
				return boost::shared_ptr<LibraryInterfaceElement>();
		}

		size_t ValueElementList::size() const
		{
			return _elements.size();
		}

		bool ValueElementList::isEmpty() const
		{
			return _elements.size() == 0;
		}

		void ValueElementList::takeFrom(ValueElementList& vel, const InterfacePage* page )
		{
			for (; vel._elements.size() > 0; vel._elements.pop_back())
			{
				_elements.push_front(vel._elements.back());
				_elements.front()->setPage(page);
			}
		}

		void ValueElementList::parse(const std::string& text, const InterfacePage* page)
		{
			size_t position = 0;
			std::vector<std::string> elements;

			// Search end of current word ignoring spaces within braces
			size_t wordEndPos;
			int numberOfOpenedBraces = 0;
			for (; position < text.size(); position = wordEndPos+1)
			{
				// Purge initial spaces
				for (; position < text.size() && text[position]==' '; ++position );

				if (position >= text.size())
					break;

				numberOfOpenedBraces = 0;
				char delimiter = 0;

				for (wordEndPos = position;
					wordEndPos < text.size() && (numberOfOpenedBraces != 0 || text[wordEndPos] != ' ');
					++wordEndPos)
				{
					// Choosing the delimiter character
					if (numberOfOpenedBraces == 0 && (text[wordEndPos] == '{' || text[wordEndPos] == '['))
						delimiter = text[wordEndPos];

					if (delimiter)
					{
						if (text[wordEndPos] == delimiter)
							++numberOfOpenedBraces;
						if ((delimiter == '{' && text[wordEndPos] == '}') || (delimiter == '[' && text[wordEndPos] == ']'))
							--numberOfOpenedBraces;
						if (numberOfOpenedBraces < 0)
							throw InterfacePageException("Parse error : too much closing braces at "+ lexical_cast<string>(wordEndPos) +" in "+ text);
					}
				}

				elements.push_back(text.substr(position, wordEndPos - position));
			}

			if (numberOfOpenedBraces > 0)
				throw InterfacePageException("Parse error : too much opening braces in "+ text);

			// Registering each word as ValueInterfaceElement
			for (vector<string>::const_iterator it = elements.begin(); it != elements.end(); ++it)
			{
				boost::shared_ptr<LibraryInterfaceElement> vie;
				const std::string& str = *it;

				// Case 1 : Parameter
				if(str.size() > 1 && str[0] == '@')
				{
					vie.reset(new ParameterValueInterfaceElement(str.substr(1)));
				}

				// Case 2 : single word
				else if (str.at(0) != '{' && str.at(0) != '[')
				{
					vie.reset(new StaticValueInterfaceElement(str));
				}

				// Case 3 : multiple word
				else if(str.size() > 1 && ((str.at(0) == '{' && str.at(1) != '{') || str.at(0) == '['))
				{
					vie.reset(new StaticValueInterfaceElement(str.substr(1, str.size() - 2)));
				}

				// Case 4 : recursive call
				else if(str.size() > 4 && str.substr(0,2) == "{{" && str.substr(str.size() -2, 2) == "}}")
				{
					// Search of the end of the first word
					for (position = 2; position < str.size() - 2 && str.at(position) !=' '; ++position);

					try
					{
						vie.reset(Factory<LibraryInterfaceElement>::create(str.substr(2, position - 2)));
						ValueElementList vel;
						if (position < str.size() - 3)
							vel.parse(str.substr(position + 1, str.size() - position - 3), page);
						vie->storeParameters(vel);
					}
					catch(Exception& e)
					{
						throw InterfacePageException("Function not found in "+ text);
					}
				}
				else
					throw InterfacePageException("Unspecified parse error in "+ text);

				vie->setPage(page);
				_elements.push_back(vie);
			}

		}



		void ValueElementList::push_back( boost::shared_ptr<LibraryInterfaceElement> element )
		{
			_elements.push_back(element);
		}
	}
}
