
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

#include "01_util/Html.h"

#include "32_admin/ResultHTMLTable.h"
#include "32_admin/AdminRequest.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace admin
	{


		ResultHTMLTable::ResultHTMLTable(const ResultHTMLTable::HeaderVector& header, AdminRequest* searchRequest
			, string currentOrderField, bool raisingOrder
			, AdminRequest* actionRequest, std::string selectName/*=""*/, std::string iconPath )
			: HtmlTable(header.size(), "adminresults")
			, _actionRequest(actionRequest)
			, _searchRequest(searchRequest)
			, _selectName(selectName)
		{
			stringstream s;
			s << "<tr>";
			if (!_selectName.empty())
				s << "<th>Sel</th>";
			for (HeaderVector::const_iterator it = header.begin(); it != header.end(); ++it)
			{
				s << "<th>" << it->second;
				if (!it->first.empty() && !iconPath.empty())
					s << "&nbsp;<img src=\"" << iconPath << "down.png\" alt=\"Trier par ordre décroissant\" />&nbsp;<img src=\"" << iconPath << "up.png\" alt=\"Trier par ordre croissant\" />";
				s << "</th>";
			}
			s << "</tr>";
			_headers = s.str();
		}

		std::string ResultHTMLTable::open()
		{
			stringstream s;
			if (_actionRequest != NULL)
				s << _actionRequest->getHTMLFormHeader("adminmainaction");
			s << HtmlTable::open();
			return s.str();
		}

		std::string ResultHTMLTable::close()
		{
			stringstream s;
			s << HtmlTable::close();
			if (_actionRequest != NULL)
				s << "</form>";
			return s.str();
		}

		std::string ResultHTMLTable::row(std::string value, std::string className )
		{
			stringstream s;
			s << HtmlTable::row(className);
			if (!_selectName.empty())
			{
				s << col();
				if (!value.empty())
					s << Html::getRadioInput(_selectName, value, string(), string());
			}
			return s.str();
		}

		std::string ResultHTMLTable::row(std::string value)
		{
			return row(value, string());
		}

	}
}
