
/** ActionResultHTMLTable class header.
	@file ActionResultHTMLTable.h

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

#ifndef SYNTHESE_ActionResultHTMLTable_h__
#define SYNTHESE_ActionResultHTMLTable_h__

#include "05_html/ResultHTMLTable.h"

namespace synthese
{
	namespace html
	{
		/** ActionResultHTMLTable class.
			@ingroup m05
		*/
		class ActionResultHTMLTable : public ResultHTMLTable
		{
		protected:
			HTMLForm	_actionForm;
			std::string	_selectName;

		public:
			
			/** Constructor.
				@param header : vector of pairs field code / col caption. If field code is non empty then the searchRequest can be reloaded with an ordering by the column.
				@param searchRequest The request to use to fill a similar table with other parameters
				@param actionRequest A request to launch by the table content (NULL = no action request)
				@param selectFieldName A first col with radio buttons will be drawn, named by the parameter. If empty then no radio button.
			*/
			ActionResultHTMLTable(const HeaderVector& header
				, const HTMLForm& searchForm
				, RequestParameters requestParameters
				, ResultParameters resultParameters
				, const HTMLForm& actionForm
				, std::string selectName=std::string()
				, std::string iconPath=std::string());

			std::string open();
			std::string close();
			HTMLForm& getActionForm();
			std::string row(std::string value, std::string className);
			std::string row(std::string value=std::string());

		};
	}
}

#endif // SYNTHESE_ActionResultHTMLTable_h__
