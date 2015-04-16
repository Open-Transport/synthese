
/** ActionResultHTMLTable class header.
	@file ActionResultHTMLTable.h

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

#ifndef SYNTHESE_ActionResultHTMLTable_h__
#define SYNTHESE_ActionResultHTMLTable_h__

#include "ResultHTMLTable.h"

namespace synthese
{
	namespace html
	{
		//////////////////////////////////////////////////////////////////////////
		/// Extension of the search result display table (ResultHTMLTable) integrating the launch of an action with a parameter selected in the result list.
		/// @ingroup m05
		/// @author Hugues Romain
		class ActionResultHTMLTable:
			public ResultHTMLTable
		{
		protected:
			HTMLForm	_actionForm;
			std::string	_selectName;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param header : vector of pairs field code / col caption. If field code is non empty then the searchRequest can be reloaded with an ordering by the column.
			/// @param searchForm : reference to the form which can relaunch the search of the results (used to do an other sorting method)
			/// @param requestParameters : options of search
			/// @param v : search result (type defined by template class)
			/// @param actionForm : reference to the form to launch to do the associated action
			/// @param selectName : the code of the field containing the selected object in the search result list, as it will be transmitted to the action. If empty, the first column with radio buttons is not displayed.
			/// @param iconPath : path to icons
			template<class T>
			ActionResultHTMLTable(
				const HeaderVector& header,
				const HTMLForm& searchForm,
				RequestParameters requestParameters,
				T& v,
				const HTMLForm& actionForm,
				std::string selectName=std::string(),
				std::string iconPath=std::string("/admin/img/")
			):	ResultHTMLTable(header, searchForm, requestParameters, v, iconPath),
				_actionForm(actionForm),
				_selectName(selectName)
			{
				std::stringstream s;
				if (!_selectName.empty())
				{
					s << "<th>Sel</th>";
					++_cols;
				}
				s << _headers;
				_headers = s.str();
			}

			//! @name Getters
			//@{
				HTMLForm& getActionForm() { return _actionForm; }
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Generates the HTML code of table and form opening.
				std::string open();



				//////////////////////////////////////////////////////////////////////////
				/// Generates the HTML code of table and form closing.
				std::string close();



				//////////////////////////////////////////////////////////////////////////
				/// Generates the HTML code of a row opening.
				/// @param value if defined and if the selectName parameter is defined for the table, then the row begins with a radio input which sends the specified value to the parameter named by selectName. If defined, the value must be a string.
				/// @param className CSS class to associate to the generated <b>tr</b> tag. If empty, the class tag is not added to the tag.
				/// @author Hugues Romain
				std::string row(
					boost::optional<std::string> value = boost::optional<std::string>(),
					std::string className = std::string()
				);
			//@}
		};
	}
}

#endif // SYNTHESE_ActionResultHTMLTable_h__
