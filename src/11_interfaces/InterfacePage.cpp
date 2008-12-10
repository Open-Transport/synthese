
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

#include "InterfacePage.h"
#include "Registry.h"

#include "LineLabelInterfaceElement.h"
#include "GotoInterfaceElement.h"
#include "CommentInterfaceElement.h"
#include "InterfacePageException.h"

#include <sstream>

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<interfaces::InterfacePage>::KEY("InterfacePage");
	}

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
			BOOST_FOREACH(shared_ptr<LibraryInterfaceElement> component, _components)
			{
				if (label_to_go.empty() || component->getLabel() == label_to_go)
				{
					label_to_go = component->display(stream, parameters, vars, object, request);
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
					lie = LibraryInterfaceElement::create(line, this);
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

				// Store other types of interface elements
				_components.push_back(lie);
			}
		}



		void InterfacePage::setInterface(const Interface* value )
		{
			_interface = value;
		}



		const Interface* InterfacePage::getInterface() const
		{
			return _interface;
		}



		InterfacePage::~InterfacePage()
		{

		}



		InterfacePage::InterfacePage(
			RegistryKeyType key
		):  Registrable(key),
			FactoryBase()
		{

		}



		string InterfacePage::getValue( const ParametersVector& parameters , VariablesMap& variables , const void* object /*= NULL */, const server::Request* request /*= NULL  */ ) const
		{
			stringstream s;
			display(s, parameters, variables, object, request);
			return s.str();
		}

		void InterfacePage::setPageCode( const std::string& code )
		{
			_pageCode = code;
		}

		const std::string& InterfacePage::getPageCode() const
		{
			return _pageCode;
		}

		void InterfacePage::setDirectDisplayAllowed( bool value )
		{
			_directDisplayAllowed = value;
		}

		bool InterfacePage::getDirectDisplayAllowed() const
		{
			return _directDisplayAllowed;
		}
	}
}
