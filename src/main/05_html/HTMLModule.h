
/** HTMLModule class header.
	@file HTMLModule.h

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

#ifndef SYNTHESE_HTMLModule_H__
#define SYNTHESE_HTMLModule_H__

#include <string>

#include "01_util/ModuleClass.h"

namespace synthese
{
	/** @defgroup m05 05 HTML Library
		@{
	*/

	/** 05 HTML Library module namespace.
	*/
	namespace html
	{

		/** HTML module class.
		*/
		class HTMLModule : public util::ModuleClass
		{
		public:
			/** Link displayed as a button.
				@param url URL to go on click on the link
				@param caption Caption on the link
				@param confirm Confirmation message : if empty, the link goes directly to the url. If non empty, then a confirm popup asks the user to validate the click after reading the provided message. Warning : the confirm message must not contain any " or ' character. Use &quot; and &#39; or \' instead
				@return HTML code of the link

				To use the button displayed link in an HTML page, be aware to define 3 css styles :
					- a.linkbutton : the normal appearance of the button
					- a.activatedlinkbutton : the appearance of the button when the pointer is on it
					- a.clickedlinkbutton : the apperance of the button while the user clicks on it

				The relief is drawed by the styles. Without them the link will display normally.

				Example of css :
				@code
a.linkbutton { display:inline-block; padding:2px 4px 2px 4px; text-decoration:none; color:black; background-color:#C0C0C0; border-style:solid; border-width:1px 2px 2px 1px; border-color:#000060; cursor:auto; }
a.activatedlinkbutton { display:inline-block; padding:2px 4px 2px 4px; border-style:solid; text-decoration:none; color:black; background-color:#0080E0; border-width:1px 2px 2px 1px; cursor:hand; } 
a.clickedlinkbutton { display:inline-block; padding:2px 4px 2px 4px; border-style:solid; color:black; text-decoration:none; background-color:#0080E0; border-width:2px 1px 1px 2px; cursor:hand; }
				@endcode

				It is recommended to design the real form buttons with a similar style.

				Example of corresponding css :
				@code
input[type=submit] {background-color:#C0C0C0; font-family:verdana,helvetica; border-width:1px 2px 2px 1px; padding:0px; border-color:#000066; cursor:hand; }
input[type=submit]:hover {background-color:#0080E0;}
				@endcode
			*/
			static std::string getLinkButton(const std::string& url, const std::string& caption, const std::string confirm="", const std::string icon="");

			/** Simple HTML link generator.
				@param url URL to link
				@param content Content of the link
				@return std::string The generated HTML code
				@author Hugues Romain
				@date 2007				
			*/
			static std::string getHTMLLink(const std::string& url, const std::string& content);

			static std::string getHTMLImage(const std::string& url, const std::string& alt);

			
		};


	}

	/** @} */
}

#endif // SYNTHESE_HTMLModule_H__
