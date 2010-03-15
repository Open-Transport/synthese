
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

#include "InterfacePage.h"
#include "FactorableTemplate.h"

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
				- 0 : title
				- 1 : content 1
				- 2 : include 1
				- 3 : content 2
				- 4 : include 2
				- 5 : content 3
				- 6 : page id

			Object : WebPage
		*/
		class WebPageInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, WebPageInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param page The page to display
				@param edit True if the display must include inline edition forms
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const WebPage& page,
				bool edit,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;
			
			WebPageInterfacePage();
		};
	}
}

#endif // SYNTHESE_WebPageInterfacePage_H__
