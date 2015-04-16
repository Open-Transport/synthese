
/** PropertiesHTMLTable class implementation.
	@file PropertiesHTMLTable.cpp

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

#include "PropertiesHTMLTable.h"

using namespace std;

namespace synthese
{
	namespace html
	{
		const string PropertiesHTMLTable::CSS_CLASS("propertysheet");



		PropertiesHTMLTable::PropertiesHTMLTable(
			const HTMLForm form
		):	HTMLTable(2, CSS_CLASS),
			_form(form)
		{}



		std::string PropertiesHTMLTable::open()
		{
			stringstream s;
			s	<< _form.open()
				<< HTMLTable::open()
				<< "<col class=\"property\" /><col class=\"value\" />";
			return s.str();
		}



		std::string PropertiesHTMLTable::close()
		{
			stringstream s;
			if (_form.getUpdateRight())
			{
				s	<< row();
				s	<< col(2, string(), true) << _form.getSubmitButton("Enregistrer les modifications");
			}
			s	<< HTMLTable::close() << _form.close();
			return s.str();
		}



		HTMLForm& PropertiesHTMLTable::getForm()
		{
			return _form;
		}



		std::string PropertiesHTMLTable::cell( const std::string& label, const std::string& content)
		{
			stringstream s;
			s	<< row();
			s	<< col() << label;
			s	<< col() << content;
			return s.str();
		}



		std::string PropertiesHTMLTable::title( const std::string& title )
		{
			stringstream s;
			s	<< row();
			s	<< col(2, string(), true) << title;
			return s.str();
		}
}	}
