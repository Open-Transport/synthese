
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
#include "Registry.h"
#include "Named.h"
#include "Importable.h"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// Transport network class.
		/// TRIDENT OK
		///	@ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class TransportNetwork:
			virtual public util::Registrable,
			public util::Named,
			public impex::Importable
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<TransportNetwork> Registry;

		protected:

		 public:

			TransportNetwork (
				util::RegistryKeyType id = 0,
				std::string name = std::string()
			);
			~TransportNetwork ();

			//! @name Getters
			//@{
			//@}
				
			//! @name Setters
			//@{
			//@}

			//! @name Services
			//@{
				virtual std::string getRuleUserName() const { return "Réseau " + getName(); }
			//@}
		};
	}
}

#endif
