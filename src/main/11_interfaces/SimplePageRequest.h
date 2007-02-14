
/** SimplePageRequest class header.
	@file SimplePageRequest.h

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

#ifndef SYNTHESE_SimplePageRequest_H__
#define SYNTHESE_SimplePageRequest_H__

#include "11_interfaces/RequestWithInterface.h"

namespace synthese
{
	namespace interfaces
	{
		class InterfacePage;

		/** Request for direct displaying an interface page.
			@ingroup m11
		*/
		class SimplePageRequest : public RequestWithInterface
		{
			static const std::string PARAMETER_PAGE;

			//! \name Request parameters
			//@{
			const interfaces::InterfacePage* _page;
			Request::ParametersMap _parameters;
			//@}


			/** Conversion from attributes to generic parameter maps.
			*/
			server::Request::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void setFromParametersMap(const server::Request::ParametersMap& map);

		public:
			SimplePageRequest();

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream) const;

			void setPage(const interfaces::InterfacePage* page);
		};
	}
}

#endif // SYNTHESE_SimplePageRequest_H__
