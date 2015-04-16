
/** EditArea class header.
	@file EditArea.hpp

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

#ifndef SYNTHESE_html_EditArea_hpp__
#define SYNTHESE_html_EditArea_hpp__

#include <ostream>
#include <string>
#include <boost/optional.hpp>

namespace synthese
{
	namespace html
	{
		class HTMLForm;

		/** EditArea class.
			@ingroup m35
		*/
		class EditArea
		{
		private:
			void _getScript(
				std::ostream& stream,
				const std::string& id,
				boost::optional<const std::string&> saveCallBack = boost::optional<const std::string&>()
			) const;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Displays the javascript header
			EditArea(std::ostream& stream);



			void getTextArea(
				std::ostream& stream,
				HTMLForm& form,
				const std::string& parameterName,
				const std::string& value,
				std::size_t rows,
				std::size_t cols
			);



			void getAjaxForm(
				std::ostream& stream,
				const std::string& ajaxSaveURL,
				const std::string& parameterName,
				const std::string& value,
				std::size_t rows,
				std::size_t cols
			);
		};
}	}

#endif // SYNTHESE_pt_EditArea_hpp__

