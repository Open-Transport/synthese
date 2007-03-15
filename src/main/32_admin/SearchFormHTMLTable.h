
/** SearchFormHTMLTable class header.
	@file SearchFormHTMLTable.h

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

#ifndef SYNTHESE_SearchFormHTMLTable_h__
#define SYNTHESE_SearchFormHTMLTable_h__

#include "01_util/HtmlTable.h"

namespace synthese
{
	namespace admin
	{
		class AdminRequest;

		/** SearchFormHTMLTable class.
			@ingroup m32
		*/
		class SearchFormHTMLTable : public util::HtmlTable
		{
			const AdminRequest* const _request;
		public:
			SearchFormHTMLTable(const AdminRequest* request);

			std::string open();
			std::string close();

			std::string cell(const std::string& label, const std::string& content, std::string id="");
		};
	}
}

#endif // SYNTHESE_SearchFormHTMLTable_h__
