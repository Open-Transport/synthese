
/** WebPageInterfacePage class header.
	@file WebPageInterfacePage.h
	@author Hugues
	@date 2010

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

#ifndef SYNTHESE_WebPageInterfacePage_H__
#define SYNTHESE_WebPageInterfacePage_H__

#include <string>
#include <ostream>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace cms
	{
		class Webpage;

		//////////////////////////////////////////////////////////////////////////
		/// 36.11 Interface : Web page display.
		///	@ingroup m56Pages refPages
		///	@author Hugues Romain
		///	@date 2010
		//////////////////////////////////////////////////////////////////////////
		/// @copydoc WebPageInterfacePage::Display
		class WebPageInterfacePage
		{
		public:
			static const std::string DATA_TITLE;
			static const std::string DATA_CONTENT;
			static const std::string DATA_ABSTRACT;
			static const std::string DATA_IMAGE;
			static const std::string DATA_PUBLICATION_DATE;
			static const std::string DATA_FORUM;
			static const std::string DATA_IS_CURRENT;
			static const std::string DATA_DEPTH;

			//////////////////////////////////////////////////////////////////////////
			/// Display of a web page content by a template.
			///	@param stream Stream to write on
			/// @param templatePage the template to use for the display of the page.
			/// @param request The current request
			///	@param page The page to display
			///	@param edit True if the display must include inline edition forms (not yet implemented)
			/// @param displayContent if false, then the content is not rendered
			//////////////////////////////////////////////////////////////////////////
			/// <h3>Parameters sent to the display template</h3>
			///	<ul>
			///		<li>title : title of the page</li>
			///		<li>content : content of the page</li>
			///		<li>roid : id of the page</li>
			///		<li>abstract : abstract of the page</li>
			///		<li>image : image of the page</li>
			///		<li>date : date of beginning of publication of the page</li>
			///		<li>forum : 1 if the page has a forum</li>
			///		<li>is_the_current_page : 1 if the displayed page is the current one</li>
			///		<li>depth : depth of the page in the tree</li>
			///	</ul>
			static void Display(
				std::ostream& stream,
				const Webpage& templatePage,
				const server::Request& request,
				const Webpage& page,
				bool edit,
				bool displayContent = true
			);
		};
	}
}

#endif // SYNTHESE_WebPageInterfacePage_H__
