
/** Interface class implementation.
	@file Interface.cpp

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

#include "Interface.h"

#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/InterfacePageException.h"

#include <string>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;


	namespace util
	{
		template<> typename Registrable<uid,interfaces::Interface>::Registry Registrable<uid,interfaces::Interface>::_registry;
	}

	namespace interfaces
	{

		Interface::Interface( const uid& id )
			: Registrable<uid,synthese::interfaces::Interface> (id)
		{
		}


		const InterfacePage* Interface::getPage(const std::string& index) const
		{
			PagesMap::const_iterator it = _pages.find(index);
			if (it == _pages.end())
				throw InterfacePageException("No such interface page " + index);
			return it->second;
		}



		void Interface::addPage(const string& code, InterfacePage* page )
		{
			page->setInterface(getRegisteredSharedPointer());
			_pages.insert(make_pair( code, page ));
		}



		void Interface::removePage( const std::string& page_code )
		{
			/** @todo Add a removal of each link to the page */
			PagesMap::const_iterator it = _pages.find(page_code);
			if (it != _pages.end())
			{
				_pages.erase( page_code );
			}
		}



		void Interface::setNoSessionDefaultPageCode( const std::string& code)
		{
			_noSessionDefaultPageCode = code;
		}

		const std::string& Interface::getNoSessionDefaultPageCode() const
		{
			return _noSessionDefaultPageCode;
		}



		void Interface::setName( const std::string& name )
		{
			_name = name;
		}



		const std::string& Interface::getName() const
		{
			return _name;
		}
	}
}
