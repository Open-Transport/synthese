
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

	namespace transportwebsite
	{
		class WebPage;

		/** WebPageInterfacePage Interface Page Class.
			@ingroup m56Pages refPages
			@author Hugues
			@date 2010

			@code web_page @endcode

			Parameters :
				- title
				- content
				- roid : page id
		*/
		class WebPageInterfacePage
		{
		public:
			static const std::string DATA_TITLE;
			static const std::string DATA_CONTENT;
			
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param page The page to display
				@param edit True if the display must include inline edition forms
				@param variables Execution variables
				@param request Source request
			*/
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const WebPage> templatePage,
				const server::Request& request,
				boost::shared_ptr<const WebPage> page,
				bool edit
			);
		};
	}
}

#endif // SYNTHESE_WebPageInterfacePage_H__
