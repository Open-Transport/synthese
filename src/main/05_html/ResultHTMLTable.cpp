
/** ResultHTMLTable class implementation.
	@file ResultHTMLTable.cpp

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

#include "05_html/ResultHTMLTable.h"

using namespace std;

namespace synthese
{
	namespace html
	{


		ResultHTMLTable::ResultHTMLTable(
			const ResultHTMLTable::HeaderVector& header
			, const HTMLForm& searchForm
			, string currentOrderField, bool raisingOrder
			, std::string iconPath )
			: HTMLTable(header.size(), "adminresults")
			, _searchForm(searchForm)
		{
			stringstream s;
			for (HeaderVector::const_iterator it = header.begin(); it != header.end(); ++it)
			{
				s << "<th>" << it->second;
				if (!it->first.empty() && !iconPath.empty())
					s << "&nbsp;<img src=\"" << iconPath << "down.png\" alt=\"Trier par ordre décroissant\" />&nbsp;<img src=\"" << iconPath << "up.png\" alt=\"Trier par ordre croissant\" />";
				s << "</th>";
			}
			_headers = s.str();
		}

	}
}
