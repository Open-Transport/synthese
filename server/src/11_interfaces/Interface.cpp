
/** Interface class implementation.
	@file Interface.cpp

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

#include "Interface.h"

#include "InterfacePage.h"
#include "InterfacePageException.h"
#include "Registry.h"

#include <string>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;


	namespace util
	{
		template<> const string Registry<interfaces::Interface>::KEY("Interface");
	}

	namespace interfaces
	{

		Interface::Interface(RegistryKeyType id )
			: Registrable(id)
		{
		}


		const InterfacePage* Interface::getPage(const string& classCode, string pageCode) const
		{
			PagesMap::const_iterator it = _pages.find(classCode);
			if (it == _pages.end())
				throw InterfacePageException("No such interface page class " + classCode);
			const PagesMap::mapped_type& map2(it->second);
			PagesMap::mapped_type::const_iterator it2(map2.find(pageCode));
			if (it2 == map2.end())
			{
				// If index not found and empty, return the first element
				if (pageCode.empty())
				{
					return map2.begin()->second;
				}
				throw InterfacePageException("No such interface page key " + pageCode + " for class " + classCode);
			}
			return it2->second;
		}



		bool Interface::hasPage(const string& classCode, string pageCode) const
		{
			PagesMap::const_iterator it = _pages.find(classCode);
			if (it == _pages.end())
			{
				return false;
			}
			if (pageCode.empty())
			{
				return true;
			}
			return it->second.find(pageCode) != it->second.end();
		}


		void Interface::addPage(InterfacePage* page )
		{
			PagesMap::iterator it(_pages.find(page->getFactoryKey()));
			if (it == _pages.end())
			{
				it = _pages.insert(make_pair(page->getFactoryKey(), PagesMap::mapped_type())).first;
			}
			it->second.insert(make_pair(page->getPageCode(), page));
		}



		void Interface::removePage(const string& classCode, const string& pageCode)
		{
			PagesMap::iterator it = _pages.find(classCode);
			if (it == _pages.end())
				throw InterfacePageException("No such interface page class " + classCode);
			PagesMap::mapped_type& map2(it->second);
			PagesMap::mapped_type::iterator it2(map2.find(pageCode));
			if (it2 == map2.end())
				throw InterfacePageException("No such interface page key " + pageCode + " for class " + classCode);
			map2.erase(it2);
			if (map2.empty())
				_pages.erase(it);
		}



		void Interface::setNoSessionDefaultPageCode(const string& code)
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



		std::string Interface::getName() const
		{
			return _name;
		}

		void Interface::setDefaultClientURL( const std::string& value )
		{
			_defaultClientURL = value;
		}

		const std::string& Interface::getDefaultClientURL() const
		{
			return _defaultClientURL;
		}
	}
}
