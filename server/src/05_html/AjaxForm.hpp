
/** AjaxForm class header.
	@file AjaxForm.hpp

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

#ifndef SYNTHESE_html_AjaxForm_hpp__
#define SYNTHESE_html_AjaxForm_hpp__

#include "HTMLForm.h"

namespace synthese
{
	namespace html
	{
		//////////////////////////////////////////////////////////////////////////
		/// AJAX HTML form.
		/// Input fields without name are ignored.
		//////////////////////////////////////////////////////////////////////////
		/// @author Hugues Romain
		/// @since 3.2.1
		/// @date 2010
		///	@ingroup m05
		class AjaxForm:
			public HTMLForm
		{
		public:
			////////////////////////////////////////////////////////////////////
			/// Ajax Form constructor.
			///	@param name Name of the form in the HTML document
			///	@param action Action to run at the form submit
			///	@author Hugues Romain
			///	@date 2007
			AjaxForm(const std::string& name, const std::string& action);



			virtual std::string open(
				std::string htmlComplement = std::string()
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Generates the opening tag of the form.
			/// @param sendJSComplement Javascript code to add to the JS send_ajax
			/// function. The code can operate on the content which will be sent to
			/// the server by accessing to the content variable.
			/// @param htmlComplement HTML code to add inside the <form> tag.
			std::string openWithJSComplement(
				const std::string& sendJSComplement,
				std::string htmlComplement = std::string(),
				bool closeJSTag = true
			) const;



			/** Standard submit button generator (input type=submit).
				@param caption text displayed on the button
				@return std::string the HTML generated code
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getSubmitButton(
				const std::string& caption,
				std::string confirm = std::string(),
				std::string icon = std::string()
			);
		};
	}
}

#endif // SYNTHESE_html_AjaxForm_hpp__
