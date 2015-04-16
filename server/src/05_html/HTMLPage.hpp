
/** HTMLPage class header.
	@file HTMLPage.hpp

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

#ifndef SYNTHESE_html_HTMLPage_hpp__
#define SYNTHESE_html_HTMLPage_hpp__

#include <string>
#include <sstream>

namespace synthese
{
	namespace html
	{
		/** HTMLPage class.
			@ingroup m05
		*/
		class HTMLPage
		{
		private:
			std::string _inlineCSS;
			std::string _title;
			std::stringstream _body;

		public:
			HTMLPage();

			void setInlineCSS(const std::string& value){ _inlineCSS = value; }
			void setTitle(const std::string& value){ _title = value; }
			
			void setDefaultInlineCSS();

			void output(std::ostream& str) const;

			HTMLPage& operator<<(const std::string& s);
		};
	}
}

#endif // SYNTHESE_html_HTMLPage_hpp__

