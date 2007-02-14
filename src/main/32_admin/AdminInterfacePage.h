
/** AdminInterfacePage class header.
	@file AdminInterfacePage.h

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

#ifndef SYNTHESE_AdminInterfacePage_H__
#define SYNTHESE_AdminInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace admin
	{
		class AdminInterfaceElement;

		/** Admin template page.
		@code admin @endcode
		*/
		class AdminInterfacePage : public interfaces::InterfacePage
		{
		public:

			/** Display of the admin page.
			@param stream Stream to write on
			@param pageKey Key of the AdminInterfaceElement to display
			@param parameters Parameters to transmit to the AdminInterfaceElement to display
			@param site Displayed site
			*/
			void display( std::ostream& stream
				, const AdminInterfaceElement* page
				, const uid objectId
				, const server::Request* request = NULL ) const;

		};
	}
}

#endif // SYNTHESE_AdminInterfacePage_H__
