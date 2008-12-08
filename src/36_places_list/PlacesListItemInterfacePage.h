
/** PlacesListItemInterfacePage class header.
	@file PlacesListItemInterfacePage.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_PlacesListItemInterfacePage_H__
#define SYNTHESE_PlacesListItemInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace transportwebsite
	{
		/** PlacesListItemInterfacePage Interface Page Class.
			@ingroup m36Pages refPages

			Parameters :
			 - 0 : n rank of the item
			 - 1 : name of the idem
			 - 2 : id of the item
		*/
		class PlacesListItemInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage,PlacesListItemInterfacePage>
		{
		public:
			PlacesListItemInterfacePage();

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& variables
				, int n
				, const std::string& name
				, uid id
				, const server::Request* request = NULL
				) const;
		};
	}
}

#endif // SYNTHESE_PlacesListItemInterfacePage_H__
