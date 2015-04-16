
/** SearchFormHTMLTable class header.
	@file SearchFormHTMLTable.h

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

#ifndef SYNTHESE_SearchFormHTMLTable_h__
#define SYNTHESE_SearchFormHTMLTable_h__

#include "05_html/HTMLTable.h"
#include "05_html/HTMLForm.h"

namespace synthese
{
	namespace html
	{
		/** SearchFormHTMLTable class.
			@ingroup m05
		*/
		class SearchFormHTMLTable : public HTMLTable
		{
			HTMLForm	_form;

		public:
			SearchFormHTMLTable(const HTMLForm&);

			std::string open();
			std::string close();

			HTMLForm& getForm();

			std::string cell(const std::string& label, const std::string& content, std::string id="");
		};
	}
}

#endif // SYNTHESE_SearchFormHTMLTable_h__
