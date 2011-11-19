
/** HTMLList class implementation.
	@file HTMLList.cpp

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

#include "HTMLList.h"

#include <sstream>

using namespace std;

namespace synthese
{
	namespace html
	{


		HTMLList::HTMLList()
			: _element(false)
		{

		}

		std::string HTMLList::closeElement()
		{
			if (_element)
			{
				_element = false;
				return "</li>";
			}
			return string();
		}

		std::string HTMLList::open()
		{
			return "<ul>";
		}

		std::string HTMLList::element( std::string className /*= std::string()*/ )
		{
			stringstream s;
			s << closeElement();
			s << "<li";
			if (!className.empty())
				s << " class=\"" << className << "\"";
			s << ">";
			_element = true;
			return s.str();
		}

		std::string HTMLList::close()
		{
			stringstream s;
			s << closeElement();
			s << "</ul>";
			return s.str();
		}
	}
}
