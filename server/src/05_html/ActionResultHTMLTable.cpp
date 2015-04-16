
/** ActionResultHTMLTable class implementation.
	@file ActionResultHTMLTable.cpp

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

#include <sstream>

#include "ActionResultHTMLTable.h"

using namespace std;
using namespace boost;

namespace synthese
{
	namespace html
	{
		std::string ActionResultHTMLTable::open()
		{
			stringstream s;
			s << _actionForm.open() << ResultHTMLTable::open();
			return s.str();
		}

		std::string ActionResultHTMLTable::close()
		{
			stringstream s;
			_selectName.erase();
			s << ResultHTMLTable::close() << _actionForm.close();
			return s.str();
		}



		std::string ActionResultHTMLTable::row(
			optional<string> value,
			std::string className
		){
			stringstream s;
			s << HTMLTable::row(className);
			if (!_selectName.empty())
			{
				s << col();
				if (value)
				{
					s << _actionForm.getRadioInput(_selectName, value, optional<string>(), string());
				}
			}
			return s.str();
		}
}	}
