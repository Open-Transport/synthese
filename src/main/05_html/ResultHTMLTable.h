
/** ResultHTMLTable class header.
	@file ResultHTMLTable.h

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

#ifndef SYNTHESE_ResultHTMLTable_h__
#define SYNTHESE_ResultHTMLTable_h__

#include <utility>

#include "05_html/HTMLTable.h"
#include "05_html/HTMLForm.h"

namespace synthese
{
	namespace html
	{
		/** ResultHTMLTable class.
			@ingroup m05
		*/
		class ResultHTMLTable : public HTMLTable
		{
		protected:
			HTMLForm	_searchForm;

		public:
			typedef std::vector<std::pair<std::string, std::string> > HeaderVector;

			/** Constructor.
				@param header : vector of pairs field code / col caption. If field code is non empty then the searchRequest can be reloaded with an ordering by the column.
				@param searchRequest The request to use to fill a similar table with other parameters
				@param actionRequest A request to launch by the table content (NULL = no action request)
				@param selectFieldName A first col with radio buttons will be drawn, named by the parameter. If empty then no radio button.
			*/
			ResultHTMLTable(const HeaderVector& header
				, const HTMLForm& searchForm
				, std::string currentOrderField
				, bool RaisingOrder
				, std::string iconPath="");

		};
	}
}

#endif // SYNTHESE_ResultHTMLTable_h__
