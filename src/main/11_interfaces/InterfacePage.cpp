
/** InterfacePage class implementation.
	@file InterfacePage.cpp

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

#include "01_util/Log.h"

#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/LineLabelInterfaceElement.h"
#include "11_interfaces/GotoInterfaceElement.h"
#include "11_interfaces/CommentInterfaceElement.h"
#include "11_interfaces/InterfacePageException.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace interfaces
	{

		void InterfacePage::display(
			std::ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& vars
			, const void* object /*= NULL*/
			, const server::Request* request) const
		{
			string label_to_go;
			for (LibraryInterfaceElement::Registry::const_iterator it = _components.begin(); it != _components.end(); ++it)
			{
				if (label_to_go.empty() || it->second->getLabel() == label_to_go)
				{
					label_to_go = it->second->display(stream, parameters, vars, object, request);
				}
			}
		}

		void InterfacePage::parse( const std::string& text )
		{
			int counter = 0;
			size_t start_pos;
			size_t end_pos;
			std::string last_label = "";
			for (start_pos = 0; start_pos < text.size(); start_pos = end_pos + 1)
			{
				for (end_pos = start_pos; end_pos < text.size() && text[end_pos] != '\n'; ++end_pos);

				std::string line = text.substr(start_pos, end_pos - start_pos);
				shared_ptr<LibraryInterfaceElement> lie;
				try
				{
					lie = LibraryInterfaceElement::create(line, getRegisteredSharedPointer());
					if (!lie.get())
						continue;
				}
				// Jump interface elements with parse errors
				catch (InterfacePageException& e)
				{
					Log::GetInstance().warn("Interface page parsing error on " + line + "\n", e);
					continue;
				}

				// Jump over a comment
				if (dynamic_pointer_cast<CommentInterfaceElement, LibraryInterfaceElement>(lie).get())
					continue;

				lie->setKey(counter++);

				// Store other types of interface elements
				_components.add(lie);
			}
		}

		void InterfacePage::setInterface(shared_ptr<const Interface> value )
		{
			_interface = value;
		}

		shared_ptr<const Interface> InterfacePage::getInterface() const
		{
			return _interface;
		}

		InterfacePage::~InterfacePage()
		{

		}

		void InterfacePage::setCode( const std::string& code )
		{
			_code = code;
		}

		const std::string& InterfacePage::getCode() const
		{
			return _code;
		}

		InterfacePage::InterfacePage()
		{

		}

		string InterfacePage::getValue( const ParametersVector& parameters , VariablesMap& variables , const void* object /*= NULL */, const server::Request* request /*= NULL  */ ) const
		{
			stringstream s;
			display(s, parameters, variables, object, request);
			return s.str();
		}
	}
}

