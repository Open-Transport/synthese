
/** RedirectInterfacePage class header.
	@file RedirectInterfacePage.h

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

#ifndef SYNTHESE_RedirectInterfacePage_H__
#define SYNTHESE_RedirectInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace interfaces
	{
		/** Redirection interface page.
			
			Available data :
				- url : HTTP url to redirect

			@ingroup m11Pages refPages
		*/
		class RedirectInterfacePage : public util::FactorableTemplate<InterfacePage,RedirectInterfacePage>
		{
		public:
			static const std::string DATA_URL;

			RedirectInterfacePage();

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object :

				@param stream Stream to write on
				@param request Source request

				The redirection is made for launching the specified request
			*/
			void display(std::ostream& stream, VariablesMap& vars, const server::Request* request = NULL) const;


			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object :

				@param stream Stream to write on
				@param url URL to redirect after the action
				@param request Source request
			*/
			void display(std::ostream& stream, VariablesMap& vars, const std::string& url, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_RedirectInterfacePage_H__
