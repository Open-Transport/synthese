
/** TransportNetwork class header.
	@file TransportNetwork.h

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

#ifndef SYNTHESE_ENV_TRANSPORTNETWORK_H
#define SYNTHESE_ENV_TRANSPORTNETWORK_H

#include "Registrable.h"


#include <string>

namespace synthese
{
	namespace env 
	{
		/** Transport network.
		TRIDENT OK
			@ingroup m35
		*/
		class TransportNetwork
		:	public util::Registrable
		{
			std::string _name;

		 public:

			TransportNetwork (
				util::RegistryKeyType id = UNKNOWN_VALUE,
				std::string name = std::string()
			);
			~TransportNetwork ();

			//! @name Getters/Setters
			//@{
				const std::string& getName () const;
				void setName( const std::string& name);
			//@}
		};
	}
}

#endif
