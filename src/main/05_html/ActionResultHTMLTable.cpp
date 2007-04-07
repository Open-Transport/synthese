
/** ActionResultHTMLTable class implementation.
	@file ActionResultHTMLTable.cpp

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

#include <sstream>

#include "05_html/ActionResultHTMLTable.h"

using namespace std;

namespace synthese
{
	namespace html
	{
		ActionResultHTMLTable::ActionResultHTMLTable( const HeaderVector& header , const HTMLForm& searchForm , std::string currentOrderField , bool raisingOrder , const HTMLForm& actionForm, std::string selectName/*="" */, std::string iconPath/*=""*/ )
			: ResultHTMLTable(header, searchForm, currentOrderField, raisingOrder, iconPath), _actionForm(actionForm), _selectName(selectName)
		{
			stringstream s;
			if (!_selectName.empty())
				s << "<th>Sel</th>";
			s << _headers;
			_headers = s.str();
		}

		std::string ActionResultHTMLTable::open()
		{
			stringstream s;
			s << _actionForm.open() << ResultHTMLTable::open();
			return s.str();
		}

		std::string ActionResultHTMLTable::close()
		{
			stringstream s;
			s << ResultHTMLTable::close() << _actionForm.close();
			return s.str();
		}

		HTMLForm& ActionResultHTMLTable::getActionForm()
		{
			return _actionForm;
		}

		std::string ActionResultHTMLTable::row(std::string value, std::string className )
		{
			stringstream s;
			s << HTMLTable::row(className);
			if (!_selectName.empty())
			{
				s << col();
				if (!value.empty())
					s << _actionForm.getRadioInput(_selectName, value, string(), string());
			}
			return s.str();
		}

		std::string ActionResultHTMLTable::row(std::string value)
		{
			return row(value, string());
		}

	}
}
