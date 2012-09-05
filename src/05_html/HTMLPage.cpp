
/** HTMLPage class implementation.
	@file HTMLPage.cpp

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

#include "HTMLPage.hpp"

namespace synthese
{
	namespace html
	{
		HTMLPage::HTMLPage()
		{

		}



		void HTMLPage::output(
			std::ostream& str
		) const	{

			str << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" <<
				"<html xmlns=\"http://www.w3.org/1999/xhtml\"><head>";
			if(!_title.empty())
			{
				str << "<title>" << _title << "</title>";
			}
			if(!_inlineCSS.empty())
			{
				str << "<style type=\"text/css\">" << _inlineCSS << "</style>";
			}
			str << "</head><body>";

			str << _body.str();

			str << "</body></html>";
		}



		void HTMLPage::setDefaultInlineCSS()
		{
			setInlineCSS("body { font-family:Arial, Helvetica; }");
		}



		HTMLPage& HTMLPage::operator<<( const std::string& s )
		{
			_body << s;
			return *this;
		}
}	}

